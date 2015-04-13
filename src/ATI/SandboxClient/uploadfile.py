#!/usr/bin/env python
import httplib
import subprocess
import hashlib
import sys
sys.path.append("/opt/ATI")

from logger import *  

from httpmeta import * 
#this is used for upload file to cloud




def send_file_to_clound_from_agent(server, filepath,filename):
    meta=get_line_contain_fuid(filepath)
    logger.debug("the uploding file meta is %s" %(meta))
    print type(meta)
    f=open(filepath, 'r')
    s=f.read()
    f.close()
    sha1sum_value=hashlib.sha1(s).hexdigest()
    headers = {

    "meta":meta,
    "X-Files": sha1sum_value+','+filepath,

    "Filename":"foo",

     "sha1sum": sha1sum_value
     #"meta":"caomima"
    }


    try:
        conn = httplib.HTTPConnection(server, 80)
        conn.request("PUT", "/",   open(filepath, "rb") , headers)
        logger.debug("send file %s to cloud,header %s" %(filepath,headers))
        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
    except Exception,ex:
        logger.debug( "There is connection issue "+str(ex))

if __name__ == "__main__":
    send_file_to_clound_from_agent("127.0.0.1","./FCPvev3SqrR7AngQ5j.unknown", "foo")
