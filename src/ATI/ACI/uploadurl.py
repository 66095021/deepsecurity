#!/usr/bin/env python
import httplib
import subprocess
import hashlib
from logger import * 
#this is used for sending a url to cloud server


def send_url_to_clound_from_agent(server, url):
    headers = {
	"url": url
    }



    try:
        conn = httplib.HTTPConnection(server, 80)
        conn.request("PUT", "/url",   " "  , headers)
        logger.debug("send url %s to server %s" %(url,server))
        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
    except Exception,ex:
        logger.debug(  "there is a connection issue"+str(ex))

if __name__ == "__main__":
    send_url_to_clound_from_agent("127.0.0.1","http://www.test.com/av.exe")
