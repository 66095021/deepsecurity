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
from logger import * 
def get_meta_of_line(line):
    meta={}
    line=line.replace("<system-event ","")
    line=line.replace("/>","")
    line=line.replace(" ","-",1)
    str_split=line.split('"')
    logger.debug(str_split)
    for i in str_split:
        if len(i) and i[-1]=="=":
            meta[i.replace("=", "").strip()]=str_split[str_split.index(i)+1]
        #meta[i.split("=")[0]]=i.split("=")[1].replace('"','')
    logger.debug(meta)
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
    pname=meta["process"]
    logger.debug ("%s %s %s" %(pname, pid,meta))
    #some situations here,
    # 1) if the process name is contains Captureclient, then we get a) the pid and path in object1/2,  we just set up a dict contains the pid/name, statusinto the extract_list
    # 2) if not, it is a normal process which is monitored,
    #  2a)  we need to put its action into extract_list if the delta time is smaller than 15 ,and there is no terminate action 
    #  2b) if we met the terminate action from Captureclient, we need to change the status to done. push it into queue 
    if pname.find("CaptureClient.exe") != -1 and meta["type"] == "process" and meta["action"]=="created":
       behavior={}
       behavior["processId"]=meta["object1"]
       behavior["process"]=meta["object2"]
       behavior["start_time"]=meta["time"]
       behavior["start_utc_time"]=meta["utc_time"]
       
if __name__ == "__main__":
    get_pid_action('<system-event time="25/4/2015 2:15:1.543" type="process" processId="612" process="C:\WINDOWS\system32\smss.exe" action="created" object1="2696" object2="C:\WINDOWS\system32\csrss.exe"/>')
    get_pid_action('<system-event time="24/4/2015 18:7:51.347" type="process" processId="3224" process="C:\Program Files\Capture\CaptureClient.exe" action="created" object1="4088" object2="C:\WINDOWS\Temp\PUTTY.EXE.exe"/>')
