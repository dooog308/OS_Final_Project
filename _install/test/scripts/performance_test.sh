FILE1=cpu_bound_test
FILE2=memory_bound_test
FILE3=io_bound_test
TEM=tem.txt
TEST_COUNT=5
AVG_USER_TIME=0
AVG_SYS_TIME=0
AVG_CPU_PERCENT=0
RESULT=performance_result.txt

RunTest() {
	time -v -o ${TEM} ./${1}
	touch tmp2.txt 

	cat $TEM|grep -E "User time|System time|Percent of CPU"| while read line
	do
		echo $line | cut -d ':' -f2 >> tmp2.txt
	done

	count=1
	while read line
   	do
		line=$( echo $line | cut -d ':' -f2)
	    case ${count} in
			"1")
				AVG_USER_TIME=$( echo $AVG_USER_TIME + $line | bc -l)
				;;

			"2")
				AVG_SYS_TIME=$( echo $AVG_SYS_TIME + $line | bc -l)
				;;
			"3")
				line=$( echo $line | cut -d '%' -f1 )
				AVG_CPU_PERCENT=$( echo $AVG_CPU_PERCENT + $line | bc -l)
				;;
		esac 
		count=$( echo $count+1 | bc )
	done < tmp2.txt
	rm tmp2.txt
}

TestLoop(){
	echo $1 test start

	AVG_USER_TIME=0
	AVG_SYS_TIME=0
	AVG_CPU_PERCENT=0

	touch $TEM
	for i in $(seq 1 ${TEST_COUNT});
	do
		RunTest $1
	done
	rm tem.txt

	AVG_USER_TIME=$( echo $AVG_USER_TIME / $TEST_COUNT | bc -l)
	AVG_SYS_TIME=$( echo $AVG_SYS_TIME / $TEST_COUNT | bc -l)
	AVG_CPU_PERCENT=$( echo $AVG_CPU_PERCENT / $TEST_COUNT | bc -l)
	echo AVG_USER_TIME=${AVG_USER_TIME}s
	echo AVG_SYS_TIME=${AVG_SYS_TIME}s
	echo AVG_CPU_PERCENT=$AVG_CPU_PERCENT%
	echo $1 test done
}


cd /test

echo performance test start
TestLoop $FILE1
TestLoop $FILE2
TestLoop $FILE3
echo performance test done

