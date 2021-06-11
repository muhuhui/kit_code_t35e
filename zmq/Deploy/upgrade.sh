#!/bin/bash

MAIN_PROCESS_NAME='ORSlave'
MAIN_PROCESS_PATH='/home/rpdzkj/NewSlave/ORSlave'
#MAIN_PROCESS_NAME='hello'
#MAIN_PROCESS_PATH='/home/rpdzkj/liming/test/hello'

SAVE_PROCESS_NAME='saveProcess.sh'
SAVE_PROCESS_PATH='/home/rpdzkj/NewSlave/Bootup/saveProcess.sh'
#SAVE_PROCESS_NAME='SSave'
#SAVE_PROCESS_PATH='/home/rpdzkj/liming/test/SSave'

SW_PATH="/home/rpdzkj/NewSlave"
CONFIG_PATH="/home/rpdzkj/NewSlave/Config"
SYS_LIB="/usr"
#SW_PATH="/home/rpdzkj/liming/TT/NewSlave"
#CONFIG_PATH="/home/rpdzkj/liming/TT/NewSlave/Config"
#SYS_LIB="/home/rpdzkj/liming/TT"


function ErrorExit ()
{
    echo ">FAILED";
    exit -1;
}

function SuccessExit ()
{
    echo ">SUCCESS";
    exit 0;
}

function ShowUsage ()
{
    echo "----------------------     Usage     --------------------"
    echo "./upgrade.sh <-f pacakge_path | -c config_file>
          package_path : path of the pakage generatd by 'tar czf'
          config_file  : path of the ParametersConfig.ini 
          e.g. ./upgrade.sh -p /home/xxx/new.tgz 
               ./upgrade.sh -c /home/xxx/config"
    echo "---------------------------------------------------------"
    exit -1
}


function restartProcess ()
{
    echo " ---- Restart ----"
    #########################
    # kill running processslave
    #########################
    pid=`ps -ef | grep ${MAIN_PROCESS_NAME} | grep -v grep | awk '{print $2}'`
    #echo -e "${MAIN_PROCESS_NAME} is running, pid is \c"
    #echo "Main process pid: $pid"
    pidSave=${pid:=0}
    if [ $pid -gt 0 ]
    then
        kill $pid
        if [ $? == 0 ]
        then
            echo "Kill main process ${MAIN_PROCESS_NAME} ($pid) [OK]"
        else
            echo "Kill main process ${MAIN_PROCESS_NAME} ($pid) [NG]"
            ErrorExit
        fi
    else
        echo "${MAIN_PROCESS_NAME} is not alive"
    fi

    #########################
    # Save process restart
    #########################
    pidSave=`ps -ef | grep ${SAVE_PROCESS_NAME}| grep -v grep | awk '{print $2}'`
    #echo "Save process pid: $pidSave"
    pidSave=${pidSave:=0}
    if [ $pidSave -gt 0 ]
    then
        kill $pidSave
        if [ $? == 0 ]
        then
            echo "Stop save process ${SAVE_PROCESS_NAME} ($pidSave) [OK]"
        else
            echo "Stop save process ${SAVE_PROCESS_NAME} ($pidSave) [NG]"
            ErrorExit
        fi
    else
        echo "${SAVE_PROCESS_NAME} is not alive"
    fi
    
    #restart
    echo -e "Start save process ${SAVE_PROCESS_NAME} \c"
    ( $SAVE_PROCESS_PATH >/dev/null 2>&1 ) &
    sleep 1
    pidNew=`ps -ef | grep ${SAVE_PROCESS_NAME}| grep -v grep | awk '{print $2}'`
    if [[ ${pidNew} -ne 0 ]]
    then
        echo " (${pidNew}) [OK]"
    else
        echo " (${pidNew}) [NG]"
        ErrorExit
    fi
    echo "New process \"${MAIN_PROCESS_NAME}\" will be launched by \"${SAVE_PROCESS_NAME}\""
    SuccessExit
}

#########################
# check the parameter
#########################
if [ $# != 2 ]
then
    echo "$#"
    ShowUsage
    ErrorExit
fi

while getopts "p:c:" OPT; do
    case $OPT in
        p)
            sourceFile="$OPTARG"
#            echo "pacakge: $sourceFile"
        ;;
        c)
            configFile="$OPTARG"
#            echo "config : $configFile"
        ;;
#        v)
#            verbose=1
#        ;;
        ?)
           ShowUsage
         ;;
    esac
done

#if [ $verbose==1 ]
#then
#    set -x
#else
#   unset -x
#fi

#########################
# check config file 
#########################
if [[ -n ${configFile} ]]
then
    echo -e "1) Config     : ${configFile} -> $CONFIG_PATH \c"
    docker cp grobserver:${configFile} ${CONFIG_PATH}
    if [ $? != 0 ]
    then
        echo "[NG]"
        ErrorExit
    else
        echo "[OK]"
        restartProcess
    fi
fi

#########################
# check package file
#########################
if [[ -n ${sourceFile} ]]
then
    file=/tmp/Slave.tgz
    if [ -e $file ]
    then
        rm -rf $file
    fi
    docker cp grobserver:${sourceFile} ${file%/*}
    if [[ -e ${file} && -f ${file} ]]
    then
        echo "1) Source [OK]: ${file}"
    else
        echo "1) Source [NG]: ${file}"
        ErrorExit
    fi
else
   echo "No ${sourceFile}"
   ErrorExit
fi


#########################
# check dest dir
#########################
destDir=$SW_PATH
if [ -e ${destDir} ]
then
    echo "2) Dest   [OK]: ${destDir}"
else
    echo "2) Dest   [NG]: ${destDir}"
    ErrorExit
fi


#########################
# back up dest dir
#########################
destBak="${destDir}.bak"
if [ -e ${destBak} ]
then
    echo "*) Remove [OK]: ${destBak}"
    rm -rf ${destBak}
fi

mv ${destDir} ${destBak}

if [ $? == 0 ]
then
    echo "3) Backup [OK]: ${destDir} -> ${destBak}"
else
    echo "3) Backup [NG]: ${destDir} -> ${destBak}"
    ErrorExit
fi

#########################
# decompression
#########################
dir=${destDir%/*}
echo -e "4) Decompress : \"${file}\" -> \"$dir\" \c"
`tar xzf ${file} -C ${dir}`

if [ !$? ]
then
    echo "[OK]"
else
    echo "[NG]"
    ErrorExit
fi


#########################
# chmod 
#########################
echo -e "5) Chmod      : $MAIN_PROCESS_PATH \c"
chmod +x "$MAIN_PROCESS_PATH"
if [ $? == 0 ]
then
    echo "[OK]"
else
    echo "[NG]"
    ErrorExit
fi

#########################
# lib update 
#########################
echo -e "6) Update lib : ${dir}/NewSlave/lib -> $SYS_LIB \c"
cp -r "${dir}/NewSlave/lib" $SYS_LIB

if [ $? == 0 ]
then
    echo "[OK]"
else
    echo "[NG]"
    ErrorExit
fi

#########################
# restart
#########################
restartProcess

