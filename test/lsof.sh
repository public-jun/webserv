#!/bin/bash
echo ------------ps--------------
ps | tee ps_current_process.txt | sed '/grep/d' | GREP_COLOR='0;35' egrep --color=auto '.*webserv.*|$'
echo ------------ps--------------
pid=`cat ps_current_process.txt | grep webserv | awk 'NR==1 {print $1}'`
echo -e "\033[35mPID->${pid}\033[m"
sleep 2
rm ps_current_process.txt
clear="\033[1;1H\033[0J"

while [ 1 ]
do
echo -e ${clear}
lsof -p $pid
sleep 1
done
