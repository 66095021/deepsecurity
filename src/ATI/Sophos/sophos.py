#!/usr/bin/env python
import os 
import hashlib
import time
import sys
sys.path.append("/opt/ATI")
from logger import * 

def get_sha1sum(file):
    f=open(file, 'r')
    s=f.read()
    sha1sum_value=hashlib.sha1(s).hexdigest()
    f.close()
    return  sha1sum_value
#return 1 if virus, else 0
#there is a FIFO which is readed by really sophos virus program we made.
# there is output /tmp/sophos_output. read it to get whether virus
def sophos_it(filename):
#send the name/SHA1sum to new sophos program
    hashvalue=get_sha1sum(filename)
    command="echo "+ filename+" " +hashvalue+">/tmp/sophos_fifo &";
    logger.debug( "send the file %s sha1sum %s to sophos fifo"%(filename,hashvalue))
    os.system(command)
#check the result 
    time.sleep(2)
# 2 means waiting the result, 0  means no virus, 1 means virus
    ret=2
#iterate the file
    f=open('/tmp/sophos_out','r')
    for line in f:
        logger.debug( "iterate the sophos_out %s to find the result " %line)
        #print    type(line)
        if line.split(" ")[1].strip()== hashvalue:
            ret=int(line.split(" ")[2].strip())
            logger.debug ("Now, we match the result %s  the ret is %d\n" %(line,ret))
            break
        else:
            logger.debug("this line does NOT match,continue");
    f.close()
    return ret

#get the virus name if the filename is scanned as virus 
def sophos_virus_name(filename):
#use hashvalue to match 
    f=open('/tmp/sophos_out','r')
    hashvalue=get_sha1sum(filename)
    for line in f:
        logger.debug( "iterate the sophos_out %s to find the virus name result " %line)
        #print    type(line)
        if line.split(" ")[1].strip()== hashvalue:
            ret=(line.split(" ")[3].strip())
            logger.debug ("Now, we match the result %s  the virus name is %s\n" %(line,ret))
            break
        else:
            logger.debug("this line does NOT match,continue");
    f.close()
    return ret
if __name__== '__main__':
    print sophos_it("/tmp/7za.exe")
    print sophos_it("/root/sophos-av/ADVENT.COM")
    print sophos_virus_name("/tmp/ADVENT.COM")

    
