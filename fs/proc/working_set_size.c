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
#include <linux/pagewalk.h>
#include <linux/pagemap.h>
#include <linux/delay.h>


#define MAXTRACE      100
#define PGSIZE        (4096)
#define PGROUNDUP(X)  ((X+PGSIZE-1)/PGSIZE)

static int registered_pid[MAXTRACE]={0};
static int reg_size=0;
static unsigned long ref = 0;

static int clear_access_pte_range(pmd_t *pmd, unsigned long addr,
				unsigned long end, struct mm_walk *walk)
{
	int op = (int)walk->private;
	struct vm_area_struct *vma = walk->vma;
	pte_t *pte, ptent;
	spinlock_t *ptl;
	struct folio *folio;

	ptl = pmd_trans_huge_lock(pmd, vma);
	if (ptl && op==0) {	
		if (!pmd_present(*pmd))
			goto out;

		folio = pmd_folio(*pmd);

		pmdp_test_and_clear_young(vma, addr, pmd);
		folio_test_clear_young(folio);
		folio_clear_referenced(folio);
out:
		spin_unlock(ptl);
		return 0;
	}

	pte = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	if (!pte) {
		walk->action = ACTION_AGAIN;
		return 0;
	}
	for (; addr != end; pte++, addr += PAGE_SIZE) {
		ptent = ptep_get(pte);	

		if (!pte_present(ptent))
			continue;

		folio = vm_normal_folio(vma, addr, ptent);
		if (!folio)
			continue;

		if(op == 0)
		{
			ptep_test_and_clear_young(vma, addr, pte);
			folio_test_clear_young(folio);
			folio_clear_referenced(folio);
		}
		else if(op!=0 && pte_young(ptent)) ref++;
	}
	pte_unmap_unlock(pte - 1, ptl);
	cond_resched();
	return 0;
}

static int clear_access_test_walk(unsigned long start, unsigned long end,
				struct mm_walk *walk)
{
	struct vm_area_struct *vma = walk->vma;

	if (vma->vm_flags & VM_PFNMAP)
		return 1;
	return 0;
}

static const struct mm_walk_ops clear_access_walk_ops = {
	.pmd_entry		= clear_access_pte_range,
	.test_walk		= clear_access_test_walk,
	.walk_lock		= PGWALK_WRLOCK,
};

static void print_working_set_info(struct seq_file *m, struct mm_struct *mm)
{
	ref = 0;
	if(mmap_write_lock_killable(mm) == 0)
	{
		walk_page_range(mm, 0, -1, &clear_access_walk_ops, (void*)1);
		mmap_write_unlock(mm);
	}

	seq_printf(m, "\t\ttotal page: %lu Pages, ref page(last 1s): %lu Pages\n", mm->total_vm, ref);	
	seq_printf(m, "\t\ttotal mmap count: %d\n", mm->map_count);	
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
		task = find_get_task_by_vpid(registered_pid[i]);
		if(task != NULL) 
		{
			seq_printf(m, "\ttask id: %d, task name: %s\n", registered_pid[i], task->comm);
			mm = get_task_mm(task);
			if(mm != NULL && mmap_write_lock_killable(mm) == 0) 
			{	
				walk_page_range(mm, 0, -1, &clear_access_walk_ops, (void*)0);
				mmap_write_unlock(mm);
				msleep(1000);
				print_working_set_info(m, mm);	
				mmput(mm);
			}
			else seq_printf(m, "\t\tno mm.\n");	
			mm = NULL;
			put_task_struct(task);
		}
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
