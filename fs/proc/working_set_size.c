// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/utsname.h>
#include "internal.h"
#include "linux/sched.h"
#include "linux/rcupdate.h"

#define MAXTRACE      100
#define PGSIZE        (4096)
#define PGROUNDUP(X)  ((X+PGSIZE-1)/PGSIZE)

static int registered_pid[MAXTRACE]={0};
static int reg_size=0;

static void print_working_set_info(struct seq_file *m, struct mm_struct *mm)
{
	seq_printf(m, "\t\ttotal size: %lu Pages\n", mm->total_vm);	
	seq_printf(m, "\t\ttotal mmap: %d\n", mm->map_count);	
	seq_printf(m, "\t\tcode size: %lu Pages\n", mm->exec_vm);	
	seq_printf(m, "\t\tdata size: %lu Pages\n", mm->data_vm);	
	seq_printf(m, "\t\tbrk size: %lu Pages\n", PGROUNDUP(mm->brk-mm->start_brk));	
	seq_printf(m, "\t\tstack size: %lu Pages\n", mm->stack_vm);
#if COUNT_PAGE_FAULT == 1	
	seq_printf(m, "\t\ttotal page fault: %lu \n", mm->total_pgfault);	
#endif
}

static int working_set_size_proc_show(struct seq_file *m, void *v)
{
	struct task_struct *task=NULL;
	struct mm_struct *mm=NULL;

	seq_printf(m, "Working Set Size:\n");
	for(int i=0;i<reg_size;i++)
	{
		rcu_read_lock();
		task = find_task_by_vpid(registered_pid[i]);
		if(task != NULL) 
		{
			seq_printf(m, "\ttask id: %d, task name: %s\n", registered_pid[i], task->comm);
			mm = get_task_mm(task);
			if(mm != NULL) print_working_set_info(m, mm);	
			else seq_printf(m, "\t\tno mm.\n");	
		}
		rcu_read_unlock();
		task = NULL;
	}
	return 0;
}


static int working_set_size_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, working_set_size_proc_show, NULL);
}

static ssize_t working_set_size_proc_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
	char kbuf[64]={0};

	if(*off != 0) return -EFAULT;

	if(copy_from_user(kbuf, buf, size)) return -EFAULT;
	
	kbuf[size-1] = '\0';
	kstrtoint(kbuf, 0, &registered_pid[reg_size++]);
	return size;
}

static const struct proc_ops working_set_size_proc_ops = {
	.proc_read = seq_read,
	.proc_write = working_set_size_proc_write,
	.proc_open = working_set_size_proc_open,
};


static int __init proc_working_set_size_init(void)
{
	proc_create("working_set_size", 0, NULL, &working_set_size_proc_ops);
	return 0;
}
fs_initcall(proc_working_set_size_init);
