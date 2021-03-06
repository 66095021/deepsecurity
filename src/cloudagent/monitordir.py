#!/usr/bin/env python

#1) read a file from special dir
#2) get the file type
#2a) if exe, using python based sophos api to scan it firstly.
#      2aa) if issue, return 
#      2ab) if no issue ,go to 2b

#2b)verifysig the exe, if no sig or sig is untrusted, send to capture.
#   if sig is ok, just return.


#3) if the doc/ pdf etc, just send to capture
#4)  if the compression,  decompression it and for each file, do the action from 2) again.


import os 
import time

import sys
from dirtest import *
from filetype import * 
from utils  import  * 
from sophos   import * 
from checksig import * 
from decompress import * 
from  uploadtocloud import * 
from  fileusing     import * 

sys.path.append("/root/filterfile/code")

monitordir="/tmp/"
#contain the current files 
filelist=[]
#contain the files is  done
filedone=[]


#debug file
log_file='/tmp/debug-monitor.log'


def log_debug(msg):
    f=open(log_file,'a+')
    f.write(msg)
    f.close()
def filter_file(filename):
    filetype=getfiletype(filename)
    print "checking the %s type, type is %s" %(filename, filetype)
#if PE, sophos it, virus, log it and return
#if no virus, verify the sig, no sig, send to clound. if sig ok, log and return.
    if is_pe(filetype):
        print "%s is PE, will let sophos run it" %filename
        ret=sophos_it(filename)
        if ret:
            log_virus(filename)
            return
#no virus, check the sig, 0 means sig check ok,  1 means no sig or sig cert is untrusted
        ret=verify_sig(filename)
        if ret == 0:
            log_sig_ok(filename)
            return
        else:
            send_to_clound(filename)
            return
# uncompression should return a tuple contains the uncompression file list.
    if is_compression(filetype):
        uncompression_list=uncompression(filename)
        print "who is compression %s" %filename
        if uncompression_list == None:
            return
        for i in uncompression_list:
            filter_file(i)

    test=is_pdf(filetype)
    print "the %s value of %d\n" %(filename,test)
    if is_pdf(filetype):
        print "%s is a pdf, will send it \n" %filename
        send_to_clound(filename)
        return 
#other type, just return
    return
 

global number_loop;

def run_loop(dir):
    number_loop=0;
    while True:
        number_loop=number_loop+1
        #filelist=list(os.listdir(monitordir))
        filelist=get_files_for_dir(dir)     
   #print filelist
        print "the loop number is %d, the total list is %s\n" %(number_loop,filelist)
        for i  in filelist:
        #check whether it is done before 0 means using  by other process, 1 means NO
            if not verify_file_is_using(i):
                print "the file %s is using by other process, leave it " %(i)
                continue
            if not i in filedone:
                print type(i)
                filedone.append(i)
                filter_file(i)
                 #time.sleep(1)
                print "I will add %s done list \n" %i
            else:
                print "it is in the done list now"
                 #print i
        time.sleep(5)
    

if __name__== '__main__':
    run_loop(sys.argv[1])

