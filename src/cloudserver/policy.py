#!/usr/bin/env python

import json
#sbx_ip_list=[line.strip() for line in open ('/opt/sbx/sbxserver.config', 'r')]


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


count=0
exe_count=0
pdf_count=0
doc_count=0

# it will return a IP 
def get_valid_server(filetype):
    global count
    global exe_count
    global pdf_count
    global doc_count
    if policy == "rr":
        print len(servers)
        print servers[(count%len(servers))]
        count=count+1
        return  servers[(count%len(servers))]

    if  policy == "filetype":
        if filetype == "exe":
            exe_count=exe_count+1
            return  exe_servers[(exe_count%len(exe_servers))] 
        if filetype == "pdf":
            pdf_count=pdf_count+1
            return  pdf_servers[(pdf_count%len(pdf_servers))] 
        if filetype == "doc":
            doc_count=doc_count+1
            return  doc_servers[(doc_count%len(doc_servers))] 
        # in the filetype mode, if the filetype is NOT config, then we randomly it 
        count=count+1
        return  servers[(count%len(servers))]     

if __name__=="__main__":
    
    while True:
        a=get_valid_server("url")
        print a
    
    
