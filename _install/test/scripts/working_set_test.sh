FILE1=no_mmap_malloc_test
FILE2=malloc_test
FILE3=mmap_test
OUTPUT=/proc/working_set_size

cd /test
./$FILE1&
./$FILE2&
./$FILE3&

ps|grep -E "$FILE1|$FILE2|$FILE3" | while read line
do
	echo $line | cut -d ' ' -f1 > $OUTPUT
done

cat $OUTPUT
