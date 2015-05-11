#!/usr/bin/env python

#<system-event time="25/4/2015 2:15:1.543" type="process" processId="612" process="C:\WINDOWS\system32\smss.exe" action="created" object1="2696" object2="C:\WINDOWS\system32\csrss.exe"/>

#sed -i  's/\t/    /g' extract.py

#insert pid action into pid  dict in the list 

#pid_list contains the pid done or not done information, each element is a dict
pid_list={}

#the dict should be like  following
# type is the key, the value is a other dict.  action is the key of the other dict.
#the value of the other dict  is a list of different information

#"pid":123,
#
#"process_name": C:\WINDOWS\system32\smss.exe,
#"status": extracting
#"process":
#            { "create":    [{"pid":pid, "process_name":"C:\WINDOWS\system32\smss.exe", "time":when}, 
#                        {"pid":pid, "process_name":"C:\WINDOWS\system32\smss.exe"},
#
#                            ],
#
#                          "terminate": {"pid":pid, "process_name":"C:\WINDOWS\system32\smss.exe"}
#            },
#
#       
# "file":             { "create": [ {"filepath":"c:\windwo\fo...", "time":"2015/232.."}, {"filepath":"c:\windwo\fo...", "time":"2015/232.."}, ]  ,
#
#                           "read": {},
#            
#
#      },
#
#"registry": { 
#
#
#
#}
#
#
#
#
#                    
#
#
#
#  }
#
# 
#
#
#it will return dict of a line 
import time
def get_meta_of_line(line):
    meta={}
    line=line.replace("<system-event ","")
    line=line.replace("/>","")
    line=line.replace(" ","-",1)
    str_split=line.split()
    for i in str_split:
        meta[i.split("=")[0]]=i.split("=")[1].replace('"','')
    tup_birth = time.strptime(meta["time"].split('.')[0], "%d/%m/%Y-%H:%M:%S");
    birth_secds = time.mktime(tup_birth)   
    meta["utc_time"]=birth_secds
    return meta

#extract_list contains the all the extracting PID dict
extract_list=[]

def get_pid_action(line):
    global extract_list
    meta=get_meta_of_line(line)
    pid=meta["processId"]
    #some situations here,
    # 1) if the process name is contains Captureclient, then we get a) the pid and path in object1/2,  we just set up a dict contains the pid/name, statusinto the extract_list
    # 2) if not, it is a normal process which is monitored,
    #  2a)  we need to put its action into extract_list if the delta time is smaller than 15 ,and there is no terminate action 
    #  2b) if we met the terminate action from Captureclient, we need to change the status to done. push it into queue 
if __name__ == "__main__":
    print get_meta_of_line('<system-event time="25/4/2015 2:15:1.543" type="process" processId="612" process="C:\WINDOWS\system32\smss.exe" action="created" object1="2696" object2="C:\WINDOWS\system32\csrss.exe"/>')
