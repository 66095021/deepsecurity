#!/usr/bin/env python
import httplib
import subprocess
import hashlib

#this is a replacement of uploadnew.pl 


def send_url_to_clound_from_agent(server, url):
    headers = {
	"url": url
    }



    try:
        conn = httplib.HTTPConnection(server, 80)
        conn.request("PUT", "/url",   " "  , headers)

        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
        print remote_file
    except Exception,ex:
        print  "sdad",ex

if __name__ == "__main__":
    send_url_to_clound_from_agent("127.0.0.1","http://www.test.com/av.exe")
