#!/bin/bash

#DIR_DATE=$1

cd /home/tetraelc/workspace/
sshpass -p "123456" scp -r photos tetraelc@192.168.10.99:/home/tetraelc/workspace/

exit 0
