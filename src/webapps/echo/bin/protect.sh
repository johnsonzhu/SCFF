#!/bin/sh

PROC_NAME="echo"

#进程名，用于确认进程是否存在
ulimit -c unlimited
ServiceHomeDir=$(cd "$(dirname "$0")"; cd ../; pwd)
FCGI="$ServiceHomeDir/cgi/$PROC_NAME.fcgi"
#进程数,少于这个数，则重启服务
PROC_NUM=10
#服务重启命令
PROC_RESTART="./service_manager.sh restart"

#被监控程序及脚本路径，两者需在同一目录下
BIN_DIR="./"
#重启日志
RESTART_LOG="./restart.log"

#######################################################################

# Param: restart_prog KeyWord AttrId ProcessNum StartCmd
restart_prog()
{
    if [ $# -lt 3 ];then
        echo "please give prog_name and start_up_script!" >> ${RESTART_LOG}
        return
    fi

    result=`ps ax| grep "$1" |  grep -v grep | grep -v restart | awk '{print $1}' | wc -l`
    if [ $result -lt $2 ];then
        killall "$1" >> ${RESTART_LOG}
        $3
        echo `date` " Startup: " "$1"  >> ${RESTART_LOG}
    fi
}

cd ${BIN_DIR}

restart_prog ${FCGI} ${PROC_NUM} "${PROC_RESTART}"
