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

# ./extract.py test1/ behavior-json-test/

import time
import json
import threading
import time
import signal
import sys
from logger import * 
from dirutil import * 
from networkinfo import *
from analysis import * 
from geturl import * 
from map_table import * 
from gl import * 



def do_url_rank_job(url,pid_info):
        logger.debug("url event, %s will get the ranks for PID %s process %s!"%(url,pid_info["processId"],pid_info["process"]))
        res=get_url_rank(url)      
        logger.debug("list of url rank info is %s"%(res))
        pid_info["url_scan_code"]=res[1]
        pid_info["url_alexa_code"]=res[2]
        logger.debug("scan rank result  info is %d: alexa_rank is %d "%(res[1],res[2]))
	logger.debug("set PID %s process %s  rank result  info scan_rank is %d: alexa_rank is %d "%(pid_info["processId"],pid_info["process"],pid_info["url_scan_code"],pid_info["url_alexa_code"]))

def add_meta_to_pid_info(pid_info,meta):
    pid_info["information"].append(meta)
    if "hash_info" not in pid_info.keys():
        pid_info["hash_info"]={}
    if meta["type"] not in pid_info["hash_info"].keys():
        pid_info["hash_info"][meta["type"]]={}
    if meta["action"] not in pid_info["hash_info"][meta["type"]].keys():
        pid_info["hash_info"][meta["type"]][meta["action"]]=[]

    pid_info["information"].append(meta)
    pid_info["hash_info"][meta["type"]][meta["action"]].append(meta)
    if meta["type"] == "url":
         url=meta["object1"]
         logger.debug("url event, I will fork other thread to get the rank to avoid blocking reading log")
         url_worker=MyThread3(url, pid_info) 
         url_worker.setName("Thread3-urlranker")
         url_worker.start()
#        url=meta["object1"] 
#        logger.debug("url event, %s will get the ranks for PID %s process %s!"%(url,pid_info["processId"],pid_info["process"]))
#        res=get_url_rank(url)      
#        logger.debug("list of url rank info is %s"%(res))
#        pid_info["url_scan_code"]=res[1]
#        pid_info["url_alexa_code"]=res[2]
#        logger.debug("scan rank result  info is %d: alexa_rank is %d "%(res[1],res[2]))
#	logger.debug("set PID %s process %s  rank result  info scan_rank is %d: alexa_rank is %d "%(pid_info["processId"],pid_info["process"],pid_info["url_scan_code"],pid_info["url_alexa_code"]))
def update_svm_input(pid_info):
    logger.debug("update_svm_input for input ")
    code=pid_info["behavior_code"]
    bin=pid_info["behavior_result"]
    logger.debug("code is %s , bin is %s" %(code, bin))
    if  "svm_input" not in pid_info.keys():
        pid_info["svm_input"]=[]
    pid_info["svm_input"]=[]


    i=0
    while i<len(bin):
        if bin[i] == 0 or bin[i] == '0':
            pid_info["svm_input"].append(0)
        else:
            pid_info["svm_input"].append(code[i])
        i+=1

# add other 
    pid_info["svm_input"].append(pid_info["filetype_code"])
    pid_info["svm_input"].append(pid_info["pwd_code"])
    pid_info["svm_input"].append(pid_info["url_alexa_code"])
    pid_info["svm_input"].append(pid_info["url_scan_code"])
    pid_info["svm_input"].append(pid_info["origin_url_alexa_code"])
    pid_info["svm_input"].append(pid_info["origin_url_scan_code"])
    logger.debug("update_svm_input done,the new input is %s"%(pid_info["svm_input"]))
#get file type from  name 
def set_file_type_code(pid_info):
    process_name=pid_info["process"] 
    for i in   exe_map.keys():
        if i in process_name:
            pid_info["filetype_code"]=file_type_code_map[exe_map[i]]
            return
#default is exe
    pid_info["filetype_code"]=1



def set_process_pwd_code(pid_info):
    process_name=pid_info["process"] 
    index=process_name.rfind('\\')
    if index == -1:
        return 
    
    pwd=process_name[0:index]
    for i  in pwd_process.keys():
        if pwd ==  pwd_process[i]:
            pid_info["pwd_code"]=pwd_process_code_map[i]
            return
    if pwd == 'C:\\':
        pid_info["pwd_code"]=5
    else:
        pid_info["pwd_code"]=6

def set_origin_url_code(pid_info):
    process_name=pid_info["process"] 
    imagename=process_name[process_name.rfind('\\')+1:len(process_name)]
    logger.debug("the process %s base is  %s"%(process_name, imagename))
    url=get_orgin_url(imagename)
    if url==None:
	logger.debug("Can't find the original url, it is not downloaded by client")
        return
    logger.debug("Find the original url %s "%(url))
    result=get_url_rank(url)
    pid_info["origin_url_scan_code"]=result[1]
    pid_info["origin_url_alexa_code"]=result[2]
   


def get_meta_of_line(line):
    meta={}
    logger.debug("the original line is %s" %line)
    if line.find("<") == -1 or line.find("/>") == -1:
        logger.debug("the line is not pretty, ignore it")
        return None
    if 'process=""' in line:
        logger.debug("no process information, ignore it")
        return None
    line=line.replace("<system-event ","")
    line=line.replace("/>","")
    line=line.replace(" ","-",1)
    str_split=line.split('"')
    logger.debug("format the line to %s" %(str_split))
    for i in str_split:
        if len(i) and i[-1]=="=":
            meta[i.replace("=", "").strip()]=str_split[str_split.index(i)+1]
        #meta[i.split("=")[0]]=i.split("=")[1].replace('"','')
    logger.debug("the dict of the line is %s" %(meta))
    tup_birth = time.strptime(meta["time"].split('.')[0], "%d/%m/%Y-%H:%M:%S");
    birth_secds = time.mktime(tup_birth)   
    meta["utc_time"]=birth_secds
    return meta

#extract_list contains the all the extracting PID dict
#extract_list=[]

def get_pid_action(line):
    global extract_list
    meta=get_meta_of_line(line)
    if meta == None: 
        return
    pid=meta["processId"]
    pname=meta["process"]
    #logger.debug ("%s %s %s" %(pname, pid,meta))
    #some situations here,
    # 1) if the process name is contains Captureclient, then we get a) the pid and path in object1/2,  we just set up a dict contains the pid/name, statusinto the extract_list
    # 2) if not, it is a normal process which is monitored,
    #  2a)  we need to put its action into extract_list if the delta time is smaller than 15 ,and there is no terminate action 
    #  2b) if we met the terminate action from Captureclient, we need to change the status to done. push it into queue 

    # start up the program by sbx 
    if pname.find("CaptureClient.exe") != -1:
        logger.debug("it is a Captureclient event log ")
        if  meta["type"] == "process" and meta["action"]=="created":
            behavior={}
            behavior["processId"]=meta["object1"]
            behavior["process"]=meta["object2"]
            behavior["start_time"]=meta["time"]
            behavior["start_utc_time"]=meta["utc_time"]
            behavior["status"]="running"
            behavior["start_directly_by_sbx"]=1
            behavior["parent_pid"]=meta["processId"]
            behavior["parent_img"]=meta["process"]
            behavior["analysis_done_action"]=0
            # support type , url, original url, process work dir code 
            behavior["filetype_code"]=-1
            behavior["url_alexa_code"]=-1
            behavior["url_scan_code"]=-1
            behavior["origin_url_alexa_code"]=-1
            behavior["origin_url_scan_code"]=-1
            behavior["pwd_code"]=-1
            
            set_file_type_code(behavior)
            set_process_pwd_code(behavior)

            logger.debug("it is a initial monitor PID %s started by sbx " %(behavior))
            extract_list.append(behavior)
            logger.debug("the extract_list is appened with new one monitor %s" %(extract_list))
             #we do not care the sbx itself , so return it 
            return
        else:
            logger.debug("discard all other events of CaptureClient.exe ")
            return  

    else:
        res_exist=0
        for i in extract_list:
            if i["processId"]==meta["processId"] and i["process"]==meta["process"]:
                res_exist=1

        if  res_exist == 1:

            for i in extract_list:
                if i["processId"]==meta["processId"] and i["process"]==meta["process"]:
                # if no information, it is a new monitor pid without any type/action info, so make a list to store    
                    if "information" not in i.keys():
                        i["information"]=[]
                # has now, so store the meta into it
                    #i["information"].append(meta) 
                    i["last_log_time"]=meta["time"]
                    i["last_log_utc_time"]=meta["utc_time"]  
        
                    add_meta_to_pid_info(i,meta)
                    i["current_action_number"]=len(i["information"])
                # update network information
                    #get_network_information(meta,i)
                logger.debug("it is nomral event log for PID %s prcoss %s , the PID can be found in extract_list, update information done "%(i["processId"], i["process"]))
        else:
               
            logger.debug("it is nomral event log  , the PID can be  NOT found in extract_list")
            behavior={}
            behavior["processId"]=meta["processId"]
            behavior["process"]=meta["process"]
            #behavior["start_time"]=meta["time"]
            #behavior["start_utc_time"]=meta["utc_time"]
            behavior["status"]="running"
            #behavior["start_directly_by_sbx"]=0
            #behavior["parent_pid"]=meta["processId"]
            #behavior["parent_img"]=meta["process"]
            #behavior["analysis_done_action"]=0
            # support type , url, original url, process work dir code 
            behavior["filetype_code"]=-1
            behavior["url_alexa_code"]=-1
            behavior["url_scan_code"]=-1
            behavior["origin_url_alexa_code"]=-1
            behavior["origin_url_scan_code"]=-1
            behavior["pwd_code"]=-1
    
            set_file_type_code(behavior)
            set_process_pwd_code(behavior)
            set_origin_url_code(behavior) 
            behavior["information"]=[]
            add_meta_to_pid_info(behavior,meta)
            extract_list.append(behavior)
            logger.debug("the extract_list is appened with new one monitor %s" %(extract_list))

        if  meta["action"]=="created" and meta["type"]== "process":
            
            
           res_exist=0
           for i in extract_list:
               if i["processId"]==meta["object1"] and i["process"]==meta["object2"]:
                   res_exist=1
           if res_exist ==1:
                    
               for i  in extract_list:
                   if i["processId"] == meta["object1"] and i["process"] == meta["object2"]:
                       logger.debug("it is a create process event, but there is already in extract_list,just update parent etc")
                       i["start_time"]=meta["time"]
                       i["start_utc_time"]=meta["utc_time"]
                       i["status"]="running"
                       i["start_directly_by_sbx"]=0
                       i["parent_pid"]=meta["processId"]
                       i["parent_img"]=meta["process"]
                       i["analysis_done_action"]=0
           else:
               
               behavior={}
               behavior["processId"]=meta["object1"]
               behavior["process"]=meta["object2"]
               behavior["start_time"]=meta["time"]
               behavior["start_utc_time"]=meta["utc_time"]
               behavior["status"]="running"
               behavior["start_directly_by_sbx"]=0
               behavior["parent_pid"]=meta["processId"]
               behavior["parent_img"]=meta["process"]
               behavior["analysis_done_action"]=0
               # support type , url, original url, process work dir code 
               behavior["filetype_code"]=-1
               behavior["url_alexa_code"]=-1
               behavior["url_scan_code"]=-1
               behavior["origin_url_alexa_code"]=-1
               behavior["origin_url_scan_code"]=-1
               behavior["pwd_code"]=-1
       
               set_file_type_code(behavior)
               set_process_pwd_code(behavior)
               set_origin_url_code(behavior) 
               extract_list.append(behavior)
               logger.debug("the extract_list is appened with new one monitor %s" %(extract_list))
                            
def get_pid_info_from_file(file):
    f=open(file,'r')
    logger.debug("now extract information from file %s" %(file))
    for line in f:
        get_pid_action(line)
    f.close()

number_loop=0;
#contain the current files 
filelist=[]
#contain the files is  done
filedone=[]
def run_loop(self,dir):
    global number_loop
    while True:
        number_loop=number_loop+1
        #filelist=list(os.listdir(monitordir))
        filelist=get_files_for_dir(dir)
   #print filelist
        self.logger.debug("the loop number is %d, the total list is %s\n" %(number_loop,filelist))
        for i  in filelist:
            if not i in filedone:
                get_pid_info_from_file(i)
                self.logger.debug("I will add %s done list \n" %i)
                filedone.append(i)
            else:
                logger.debug( "it is in the done list now")
                #logger.debug( "the extract_list %s" %(extract_list))
                #f=open('/tmp/xxx','w')
                #json.dump(extract_list,f)
                #f.close()
                 #print i
        time.sleep(1)


def  analysis_it(self,path_behavior):
    global extract_list
    while True:
	
        time.sleep(10)
        for i in extract_list:
            if "information" not in i.keys():
                f=extract_list.index(i)
                logger.debug("dealing with  extract_list len %d ,the seq %d,  it  has no log information" %(len(extract_list),f))
                continue
#first behavior on it
            if "behavior_log_number" not in i.keys():
                f=extract_list.index(i)
                logs_number=len(i["information"])
                logger.debug("dealing with  extract_list len %d ,the seq %d, log number is %d"%(len(extract_list),f,logs_number))
                self.logger.debug("dealing with the element %s  now" %(i))
 #               (name,ret,code)=get_process_behavior_list(i,"./behavior-json-fix")
                (name,ret,code)=get_process_behavior_list(i,path_behavior)
                i["behavior_name_list"]=name
                i["behavior_result"]=ret
    	        i["behavior_code"]=code
      	        i["behavior_log_number"]=logs_number
                update_svm_input(i)
    	        logger.debug("first time to do behavior: current pid seq %d behavior  name:%s bin:%s code: %s svm_input: %s, pid is %s process name is %s  "%(f,name,ret,code,i["svm_input"],i["processId"],i["process"]))
                #f=open('/tmp/xxx','a')
                #json.dump(extract_list,f)
                #f.close()
#next behavior on it.  if no logs append, then ignore it
            else:
	        if i["behavior_log_number"] == len(i["information"]):
                    logger.debug("seq %d log number is NOT changed, will handle the url code  then continue to next "%(extract_list.index(i)))
                    logger.debug("seq %d the current pid %s  scan and alexa rank is %d ,%d , the input is %d %d  "%(extract_list.index(i),  i["processId"],i["url_scan_code"],i["url_alexa_code"],  i["svm_input"][-3],  i["svm_input"][-4]))
                    i["svm_input"][-3]=i["url_scan_code"]
                    i["svm_input"][-4]=i["url_alexa_code"]
                    logger.debug("After replacement , the current %s scan and alexa code is %d %d"%(i["processId"],i["url_scan_code"],i["url_alexa_code"]))
                    #logger.debug("whole input is %s"%(i["svm_input"]))
    	            logger.debug("log is the same just update url rank: current pid seq %d behavior  name:%s bin:%s code: %s svm_input: %s, pid is %s process name is %s  "%(extract_list.index(i),name,ret,code,i["svm_input"],i["processId"],i["process"]))
                    continue

                else:
                      

                    f=extract_list.index(i)
                    logs_number=len(i["information"])
    	            logger.debug("log increase: current pid seq %d behavior name:%s bin:%s code:%s svm_input: %s, pid is %s process name is %s  "%(f,name,ret,code,i["svm_input"],i["processId"],i["process"]))
                    self.logger.debug("dealing with the element %s  now" %(i))
                    #(name,ret,code)=get_process_behavior_list(i,"./behavior-json-fix")
                    (name,ret,code)=get_process_behavior_list(i,path_behavior)
                    i["behavior_name_list"]=name
                    i["behavior_result"]=ret
    	            i["behavior_code"]=code
      	            i["behavior_log_number"]=logs_number
    	            #logger.debug("current pid seq %d behavior %s %s %s"%(f,name,ret,code))
    	            logger.debug("log increase: the new info current pid seq %d behavior name:%s bin:%s code: %s svm_input:%s, pid is %s process name is %s  "%(f,name,ret,code,i["svm_input"],i["processId"],i["process"]))
	
class MyThread(threading.Thread):
     def __init__(self):  
        threading.Thread.__init__(self)  
        #@self.num = num
        self.logger=logger
     def run(self):
        run_loop(self,sys.argv[1])   


class MyThread2(threading.Thread):
     def __init__(self):  
        threading.Thread.__init__(self)  
        self.logger=logger
        #self.num = num
     def run(self):
         self.logger.debug("analysis_it working now")
         analysis_it(self,sys.argv[2])

class MyThread3(threading.Thread):
     def __init__(self, url ,pid_info ):  
        threading.Thread.__init__(self)  
        self.logger=logger
        self.url=url
        self.pid_info=pid_info
        #self.num = num
     def run(self):
         self.logger.debug("get url rank is  working now")
         do_url_rank_job(self.url,self.pid_info)

def handler(signum, frame):
    print "receive a signal %d"%(signum)
    sys.exit(0)

if __name__ == "__main__":

    #get_pid_action('<system-event time="25/4/2015 2:15:1.543" type="process" processId="612" process="C:\WINDOWS\system32\smss.exe" action="created" object1="2696" object2="C:\WINDOWS\system32\csrss.exe"/>')
    #get_pid_action('<system-event time="24/4/2015 18:7:51.347" type="process" processId="3224" process="C:\Program Files\Capture\CaptureClient.exe" action="created" object1="4088" object2="C:\WINDOWS\Temp\PUTTY.EXE.exe"/>')
    #get_pid_info_from_file(sys.argv[1])
    #run_loop(sys.argv[1])
    #try to mthread
    signal.signal(signal.SIGINT, handler)
    signal.signal(signal.SIGTERM, handler)
    t=[]
    extract=MyThread()
    extract.setDaemon(True)
    extract.setName("Thread1-extract")
    t.append(extract)
    analysis=MyThread2()
    analysis.setDaemon(True)
    analysis.setName("Thread2-analysis")
    t.append(analysis)
    extract.start()
    analysis.start()
    while 1:
        alive = False
        alive =  t[0].isAlive() or t[1].isAlive()
        if not alive:
            break
