#!/usr/bin/env python
import httplib
import subprocess
from time import sleep
from sbxredis import * 

from base64rw import * 
import sys
sys.path.append("/root/sbx")
sys.path.append("/root/github/src/cloudserver")
#this script checks the job queue and send the file to capture agent if there is job.
#it can run forever

#the capture listens on 58080 

import magic
import sys




log_forensic_json_info ={

"client_ip": "10.228.20.27",
"destination_ip":"8.8.8.8",
"incident_url":   "http://www.test.com/av.exe",
"name":"av.exe",
"body":"base64 encode of body", 
"type":"virus",
"time": "seconds UTC",
"usage agent": "IE/Firefox",
"record_type":"log"
}
def send_log_or_forensic_to_server(server):
    global log_forensic_json_info
    try:
        conn=httplib.HTTPConnection(server,9999)
        headers={"test":"test"}
        log_forensic_json_info["body"]=get_base64_encode_string("/tmp/7za.exe")
        
        print  str(log_forensic_json_info)
    #conn.request("POST", "/", open(filepath, "rb"),headers)
        conn.request("POST", "/", str(log_forensic_json_info) ,headers)





        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
        print remote_file
    except Exception,ex:
        print  "sdad",ex
#send_file_to_capture_agent("/tmp/default.conf")


if __name__ == "__main__":
           send_log_or_forensic_to_server(sys.argv[1])
