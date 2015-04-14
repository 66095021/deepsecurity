#!/usr/bin/env python

import json
#sbx_ip_list=[line.strip() for line in open ('/opt/sbx/sbxserver.config', 'r')]
import time
import sys
sys.path.append("/opt/ThreatSphere")
from logger import * 
f=file('./sbxserver.config', 'r')
jsonobj=json.load(f)
#print type(jsonobj)
#print jsonobj.keys()
#print jsonobj["server"][0]


# get the policy type

policy=jsonobj["rule"]
servers=jsonobj["server"]
exe_servers=jsonobj["exe"]
pdf_servers=jsonobj["pdf"]
doc_servers=jsonobj["doc"]

#to support filetype from the same client ip , send to the same node 
# if a exe is from one client, then all exe from that client should be sent to same node
exe_client_ip_map_node={}
pdf_client_ip_map_node={}
doc_client_ip_map_node={}


count=0
exe_count=0
pdf_count=0
doc_count=0

# it will return a IP 
def get_valid_server(filetype,client_ip):
    global count
    global exe_count
    global pdf_count
    global doc_count
    global client_ip_map_node
    if policy == "rr":
        print len(servers)
        print servers[(count%len(servers))]
        count=count+1
        return  servers[(count%len(servers))]

    if  policy == "filetype":
        if filetype == "exe":
            if client_ip in exe_client_ip_map_node:
                logger.debug("exe type,client %s has match the exe_client_ip_map_node %s"%(client_ip,exe_client_ip_map_node))
                return exe_client_ip_map_node[client_ip] 
            exe_count=exe_count+1
            exe_client_ip_map_node[client_ip]=exe_servers[(exe_count%len(exe_servers))] 
            return  exe_servers[(exe_count%len(exe_servers))] 
        if filetype == "pdf":
            if client_ip in pdf_client_ip_map_node:
                logger.debug("pdf type,client %s has match the pdf_client_ip_map_node %s"%(client_ip,pdf_client_ip_map_node))
                return pdf_client_ip_map_node[client_ip] 
            pdf_count=pdf_count+1
            pdf_client_ip_map_node[client_ip]=pdf_servers[(pdf_count%len(pdf_servers))]
            return  pdf_servers[(pdf_count%len(pdf_servers))] 
        if filetype == "doc":
            if client_ip in doc_client_ip_map_node:
                logger.debug("doc type,client %s has match the doc_client_ip_map_node %s"%(client_ip,doc_client_ip_map_node))
                return doc_client_ip_map_node[client_ip] 
            doc_count=doc_count+1
            doc_client_ip_map_node[client_ip]=doc_servers[(doc_count%len(doc_servers))]
            return  doc_servers[(doc_count%len(doc_servers))] 
        # in the filetype mode, if the filetype is NOT config, then we randomly it 
        count=count+1
        return  servers[(count%len(servers))]     

if __name__=="__main__":
    
    while True:
        a=get_valid_server("exe","1.2.2.1")
        b=get_valid_server("pdf","8.228.202.3")
        c=get_valid_server("pdf","10.228.202.4")
	print b
        print a,c
        time.sleep(3)
    
    
