#!/usr/bin/env python
import os 
import time


#this is just to send to cloud pyuwsgi (local)
def send_to_clound(filename,url='http://127.0.0.1'):
    command="/root/sbx/uploadnew.pl -u "+' ' +  url + ' -f ' + filename
    #./uploadnew.pl  -u http://10.236.4.44 -f /tmp/7za.exe
    print "call external command %s to upload to clound \n" %command
    a=os.system(command)
    # os.system return 16 bits, and high 8 bits is status code, lucky, chktrust verify ok return 0, no  sig or untrusted cert are 1:)
    if not a:
            return 0
    else:
            return 1

if __name__== '__main__':
    send_to_clound("/tmp/7za.exe")
