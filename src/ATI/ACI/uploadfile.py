#!/usr/bin/env python
import httplib
import subprocess
import hashlib

#this is used for upload file to cloud


def send_file_to_clound_from_agent(server, filepath,filename):
    f=open("/tmp/77.exe", 'r')
    s=f.read()
    sha1sum_value=hashlib.sha1(s).hexdigest()
    headers = {
    "X-Files": sha1sum_value+','+filepath,
    "Filename":filename,
     "sha1sum": sha1sum_value
    }


    try:
        conn = httplib.HTTPConnection(server, 80)
        conn.request("PUT", "/",   open(filepath, "rb") , headers)

        response = conn.getresponse()
        remote_file = response.read()
        conn.close()
        print remote_file
    except Exception,ex:
        print  "sdad",ex

if __name__ == "__main__":
    send_file_to_clound_from_agent("127.0.0.1","/tmp/7za.exe","7za.exe")
