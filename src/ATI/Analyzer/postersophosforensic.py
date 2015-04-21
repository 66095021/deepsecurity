#!/usr/bin/env python
import httplib
import subprocess
import  time
import sys
#this script checks the job queue and send the file to capture agent if there is job.
#it can run forever

#the capture listens on 58080 

import magic
import sys

import json 
from base64rw import *
#./postersophosforensic.py  cloud_ip localfile  1.1.1.1 2.2.2.2 www.test.com virus
forensic_data_json_info ={

"client_ip": "10.228.20.27",
"destination_ip":"8.8.8.8",
"incident_url":   "http://www.test.com/av.exe",
"name":"av.exe",
"body":"base64 encode of body", 
"type":"virus",
"time": "seconds UTC",
"usage agent": "IE/Firefox",
}
incident_log_json_info ={

"client_ip": "10.228.20.27",
"destination_ip":"8.8.8.8",
"incident_url":   "http://www.test.com/av.exe",
"type":"virus",
"time": "seconds UTC",
"usage agent": "IE/Firefox",
}


def send_forensic_data_to_server(server,file):
    global forensic_data_json_info
    try:
        conn=httplib.HTTPConnection(server,9999)
        headers={"infotype":"forensic"}
        forensic_data_json_info["body"]=get_base64_encode_string(file)
        
        print  str(forensic_data_json_info)
    #conn.request("POST", "/", open(filepath, "rb"),headers)
        conn.request("POST", "/", json.JSONEncoder().encode(forensic_data_json_info) ,headers)





        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
        print remote_file
    except Exception,ex:
        print  "sdad",ex
#send_file_to_capture_agent("/tmp/default.conf")
def send_incident_log_to_server(server):
    global incident_log_json_info
    try:
        conn=httplib.HTTPConnection(server,9999)
        headers={"infotype":"incident"}
        conn.request("POST", "/", json.JSONEncoder().encode(incident_log_json_info) ,headers)





        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
        print remote_file
    except Exception,ex:
        print  "sdad",ex
#send_file_to_capture_agent("/tmp/default.conf")


if __name__ == "__main__":
           global forensic_data_json_info 
           forensic_data_json_info["client_ip"]=sys.argv[3]
           forensic_data_json_info["destination_ip"]=sys.argv[4]
           forensic_data_json_info["incident_url"]=sys.argv[5]
           forensic_data_json_info["type"]=sys.argv[6]
           forensic_data_json_info["time"]=time.ctime()
           forensic_data_json_info["source"]="sophos"
           send_forensic_data_to_server(sys.argv[1], sys.argv[2])
