#!/bin/bash

GREEN="\033[32m"
RESET="\033[0m"
ALIVE_TIME=1
EXECUTE_FILE="./webserv"

execute() {
  LOG="${2}.log"
  rm -f ${LOG}
  (${EXECUTE_FILE} ${1} 2>${LOG}) & sleep ${ALIVE_TIME}

  if [ -s ${LOG} ]; then
    echo -e "${GREEN}[[ ${2} ]]${RESET}\nOK\n"
  else
    echo -e "${GREEN}[[ ${2} ]]${RESET}\nNG\n"
  fi
}


# config/error/内のファイルでwebservを実行
error_configs=`find ./config/error -type f`
for filepath in ${error_configs}; do
  NAME=`basename ${filepath}`
  execute ${filepath} ${NAME} & sleep 0.1
done

execute "no_such_file.conf" "no_such_file"
wait
killall ${EXECUTE_FILE} 2>/dev/null

# ログをlog.txtに写す
logfile="log.txt"
rm -f ${logfile}
for filepath in ${error_configs}; do
  NAME=`basename ${filepath}`
  LOG="${NAME}.log"
  echo -e "[[ ${LOG} ]]" >> log.txt
  cat $LOG >> log.txt
  echo >> log.txt
  rm $LOG
done

echo -e "[[ no_such_file.log ]]" >> log.txt
cat no_such_file.log >> log.txt
echo >> log.txt
