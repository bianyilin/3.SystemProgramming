#!/bin/bash

time_now=`date +"%Y-%m-%d__%H:%M:%S"`
pid=( `ps aux | awk '{if($3 > 50 || $4 > 50) {print $2}}'` )

#nchod; ${#pid[@]}
#for (( o=0;o<${#pid[@]};o++ ));do
#    echo ${pid[o]}
#done

if [[ $pid ]];then
    sleep 5
    for ((i = 0; i < ${#pid[@]}; i += 1 ));do
        process=( `ps aux | awk '{if($3 > 50 || $4 > 50) {printf("%s %s %s %.2f%% %.2f%%\n", $11, $2, $1, $3, $4)}}'` ) 
        echo $time_now ${process[0 + i * 5]} ${process[1 + i * 5]} ${process[2 + i * 5]} ${process[3 + i * 5]} ${process[4 + i * 5]}
    done
else
    echo "没发现恶意进程"
fi

