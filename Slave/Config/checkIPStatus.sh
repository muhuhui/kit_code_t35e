#! /bin/bash 
 
Ret=999
ping -c 2 $1  | grep -Ei  "([^0-9][0-9][0-9]|[^0-9][0-9]|^[0-9]?[0-9])% packet loss" && Ret=0 || Ret=-1

#echo $Ret
exit $Ret

