#!/usr/bin/env python
import httplib
import subprocess
from time import sleep
from sbxredis import * 
from policy  import * 
from logger  import * 
import sys
sys.path.append("/root/sbx")
#this script checks the job queue and send the file to capture agent if there is job.
#it can run forever

#the capture listens on 58080 

import magic
import sys


def is_pe(filemagic):
    if filemagic.find("PE32") != -1:
        return 1
    else: 
        return 0
def is_pdf(filemagic):
    if filemagic.find("PDF") != -1:
        return 1
    else:
        return 0 
def is_compression(filemagic):
    #tar.bz2
    if filemagic.find("bzip2 compressed data") != -1:
        return 1
    #tar.gz
    if filemagic.find("gzip compressed data")  != -1:
        return 1      
    # zip
    if  filemagic.find("Zip archive data")  != -1:
        return  1
    # tar 
    if  filemagic.find("POSIX tar archive")  != -1:
        return  1
    return  0

def getfiletype(filename):
    ms=magic.open(magic.MAGIC_NONE)
    ms.load("/usr/share/misc/all.mgc")
    return ms.file(filename)
def send_url_to_capture_agent( url):
    try:
        headers ={"Accept": "text/plain"}
        server=get_valid_server('url')
        logger.debug("choose server %s, url link  %s" %(server, url))
        conn = httplib.HTTPConnection(server, 58080)
        conn.request("POST", "/url/"+"IE"+"/"+url,"foo",headers)

        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
        print remote_file
    except Exception,ex:
        print  "sdad",ex

#add the job info, so that we can get client ip of this file
def send_file_to_capture_agent( filepath,filename,info,jobid):
    filetype=getfiletype(filepath)
    #if pdf, hacked, add the pdf extenstion
    if is_pdf(filetype):
        if '.pdf' not in  filename:
            filename=filename+'.pdf'
    if is_pe(filetype):
       if '.exe' not in filename:
            filename=filename+'.exe'
    headers = {
    "Filename":filename,
        "Content-type": "application/octet-stream",
        "Accept": "text/plain",
    "Filetype": filetype
    
    }

    print "add the extension %s\n" %filename
    #conn = httplib.HTTPConnection("www.encodable.com/uploaddemo/")

    try:
        #get server by policy, pass WCG_EXE etc
        tmp=getfiletype(filepath).split()[0]
        type=""
        if tmp == "WCG_EXE":
            type="exe"
        if tmp  == "WCG_DOC":
            type="doc"
        if tmp  == "WCG_PDF":
            type="pdf"

        server=get_valid_server(type,info["client_ip"])
        logger.debug("choose server %s, type %s" %(server, type))
        conn = httplib.HTTPConnection(server, 58080)
    #directly read whole file 
    #conn.request("POST", "/", open(filepath, "rb"),headers)
        conn.request("POST", "/file/"+type+"/"+filename, open(filepath, "rb"),headers)
        logger.debug("will post to capture filename is %s, local filename is  %s, full url is %s "%(filename,filepath,"/file/"+type+"/"+filename))

        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
        print remote_file
    except Exception,ex:
        print  "sdad",ex
        add_job_queue(jobid)
#send_file_to_capture_agent("/tmp/default.conf")


if __name__ == "__main__":
    while True:
       jobid=remove_job_queue()
       info = get_info(jobid)
       if  info != {}:
           if info["job_type"] == "file":
               filepath=info["filepath"]
               filename=info["filename"]
               logger.debug("job quque has job , will send local file path %s filename %s" %(filepath,filename))
               #server is configurable, remove the  hardcode now
               send_file_to_capture_agent(filepath,filename,info,jobid)
               
           else:
               send_url_to_capture_agent(info["url"])
       else:
           print "there is nothing in the job queue"
           logger.debug("job queue is nothing")
       sleep(5)
