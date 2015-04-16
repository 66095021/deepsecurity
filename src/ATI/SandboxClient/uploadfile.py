#!/usr/bin/env python
import httplib
import subprocess
import hashlib
import sys
sys.path.append("/opt/ATI")

from logger import *  
from analyzer_config import * 
from httpmeta import * 
#this is used for upload file to cloud



#filepath is local file full path,  filename is the name will be saved in clound
def send_file_to_clound_from_agent(filepath):
    meta=get_line_contain_fuid(filepath)
    filename=get_real_name_from_fuid(filepath)
    logger.debug("the uploading file %s meta is %s" %(filepath,meta))
    logger.debug("the uploading file  %s realname is %s"%(filepath,filename))
    print type(meta)
    if meta == None or filename == None:
        logger.debug("the uploading file meta or realname is none, discard")
        return
    f=open(filepath, 'r')
    s=f.read()
    f.close()
    sha1sum_value=hashlib.sha1(s).hexdigest()
    headers = {

    "meta":meta,
    "X-Files": sha1sum_value+','+filepath,

    "Filename":filename,

     "sha1sum": sha1sum_value
    }

    logger.debug("the uploading server is %s" %(cloud_server))
    try:
        conn = httplib.HTTPConnection(cloud_server, 80)
        conn.request("PUT", "/",   open(filepath, "rb") , headers)
        logger.debug("send file %s to cloud,header %s" %(filepath,headers))
        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
    except Exception,ex:
        logger.debug( "There is connection issue "+str(ex))

if __name__ == "__main__":
    send_file_to_clound_from_agent(sys.argv[1])
