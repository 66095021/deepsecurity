#!/usr/bin/env python
import httplib
import subprocess
from time import sleep
from sbxredis import * 
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

def send_file_to_capture_agent(server, filepath,filename):
    filetype=getfiletype(filepath)
    #if pdf, hacked, add the pdf extenstion
    if is_pdf(filetype):
        filename=filename+'.pdf'
    if is_pe(filetype):
        filename=filename+'.exe'
    headers = {
	"Filename":filename,
        "Content-type": "application/octet-stream",
        "Accept": "text/plain",
	"Filetype": filetype
	
    }

    print "add the extension %s\n" %filename
    #conn = httplib.HTTPConnection("www.encodable.com/uploaddemo/")
    conn = httplib.HTTPConnection(server, 58080)
    #directly read whole file 
    #conn.request("POST", "/", open(filepath, "rb"),headers)
    conn.request("POST", "/file/"+filename, open(filepath, "rb"),headers)

    response = conn.getresponse()
    remote_file = response.read()
    conn.close()
    print remote_file

#send_file_to_capture_agent("/tmp/default.conf")


if __name__ == "__main__":
    while True:
       jobid=remove_job_queue()
       info = get_info(jobid)
       if  info != {}:
           filepath=info["filepath"]
           filename=info["filename"]
           print "I will send %s %s" %(filepath, filename)
           send_file_to_capture_agent(sys.argv[1],filepath,filename)
       else:
           print "there is nothing in the job queue"
       sleep(5)
