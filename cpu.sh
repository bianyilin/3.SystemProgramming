#!/bin/bash

NowTime=`date +"%Y-%m-%d__%H:%M:%S"`

LoadAvg=`cut -d " " -f 1-3 /proc/loadavg`

CpuTemp=`cut /sys/class/thermal/thermal_zone0/temp`
CpuTemp=`echo "scale=2;${CpuTemp}/1000" | bc `
eval `head -n 1 /proc/stat | awk -v sum1= 0 -v idle1=0 '{for(i=2;i<=8;i++){sum1+=$i} printf("sum1=%.0f;idle1=%.0f;",sum1,$5)}'`

sleep 0.5

eval `head -n 1 /proc/stat | awk -v sum2= 0 -v idle2=0 '{for(i=2;i<=8;i++){sum2+=$i} printf("sum2=%.0f;idle2=%.0f;",sum2,$5)}'`

CpuUsedPerc=`echo "scale=4;(1-($idle2-$idle1)/($sum2-$sum1))*100" | bc`

CpuUsedPerc=`printf "%.2f" $CpuUsedPerc`

if [[ `echo $CpuTemp '>=' 70 | bc -l` == 1 ]]; then 
    WarnLevel="warning"
elif [[ `echo $CpuTemp '>='50 | bc -l` == 1 ]];then
    WarnLevel="note"
fi

echo "${NowTime} ${LoadAvg} ${CpuUsedPerc} ${CpuTemp} ${WarnLevel}"

