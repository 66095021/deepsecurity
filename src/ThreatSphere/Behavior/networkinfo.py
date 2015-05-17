#!/usr/bin/env python 
import json
from logger import *
#data/input/10.228.34.199/logs/2015-04-24_16-35:12:<system-event time="24/4/2015 16:36:15.628" type="connection" processId="324" process="C:\Program Files\Capture\CaptureClient.exe" action="tcp-connection" object1="0.0.0.0:58080" object2="10.228.34.169:33705"/>

#meta is line-based dict  from sbx log entry,extract four element if has
def extract_four_element_from_meta(meta):
#tcp connection
    if meta["type"]=="connection" and meta["action"]=="tcp-connection":    
        src_info=meta["object1"]    
        dst_info=meta["object2"]
        (src_ip,src_port)=src_info.split(":")
        (dst_ip,dst_port)=dst_info.split(":")
        return ("tcp",src_ip,src_port,dst_ip,dst_port)
        
    if meta["type"]=="connection" and meta["action"]=="udp-connection":    
        src_info=meta["object1"]    
        dst_info=meta["object2"]
        (src_ip,src_port)=src_info.split(":")
        (dst_ip,dst_port)=dst_info.split(":")
        return ("udp",src_ip,src_port,dst_ip,dst_port)
    return None 
#every line in log is a meta,update network information to pid_info in extract_list 
#,"id.orig_h":"192.168.150.9","id.orig_p":37857,"id.resp_h":"46.43.34.31","id.resp_p":80
#update http/dns info to pid_info network_information which is a list 
#pid_info["network_information"=[{line of http.log}, {line of http.log}, {line of dns.log}]
#which each element contains the same four element 

def get_network_information(meta, pid_info):
    four_elem=extract_four_element_from_meta(meta)
    if four_elem == None:
        return
    # extract network information,append it to whole PID dict
    f=open('networklog/http.03:57:57-04:00:00.log','r')
    for line in f:
        d=json.JSONDecoder().decode(line)
        logger.debug("the %s %s %s %s" %(type(d["id.orig_h"]),d["id.orig_h"], type(d["id.orig_p"]),d["id.orig_p"]))
        logger.debug("the %s %s %s %s" %(type( four_elem[1]),  four_elem[1], type( four_elem[3]), four_elem[3]))
        # type from json is int , but the four_elem is string
        if four_elem[1] == d["id.orig_h"] and four_elem[2]==str(d["id.orig_p"])  and  four_elem[3]==d["id.resp_h"] and four_elem[4] ==str(d["id.resp_p"]):
            if  "network_information" not in pid_info.keys():
                pid_info["network_information"]=[]
            pid_info["network_information"].append(d)

    f.close()
