#!/usr/bin/env python
import os 

import time
def verify_file_is_using(filename):
    command="lsof  "+ filename + " 1>/dev/null 2>/dev/null"
    a=os.system(command)
    # os.system return 16 bits, and high 8 bits is status code, lucky, chktrust verify ok return 0, no  sig or untrusted cert are 1:)
    if not a:
        return 0
    else:
        return 1



if __name__== '__main__':
    a=verify_file_is_using("/tmp/7za.exe")
    a=verify_file_is_using("/usr/lib64/libsavi.so.3")
    print a 


