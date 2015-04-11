#!/usr/bin/env python
import os 
import time
from datetime import * 
from logger import * 
def verify_file_is_using(filename):
    start_time=datetime.now()
    logger.debug("%s to verify file %s if the file is using by process"%(start_time,filename))
    command="lsof  "+ filename + " 1>/dev/null 2>/dev/null"
    a=os.system(command)
    stop_time=datetime.now()
    logger.debug("%s verify done" %((stop_time)))
    # os.system return 16 bits, and high 8 bits is status code, lucky, chktrust verify ok return 0, no  sig or untrusted cert are 1:)
    if not a:
        return 0
    else:
        return 1



if __name__== '__main__':
    a=verify_file_is_using("/tmp/7za.exe")
    a=verify_file_is_using("/usr/lib64/libsavi.so.3")
    print a 


