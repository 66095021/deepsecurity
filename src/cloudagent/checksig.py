#!/usr/bin/env python
import os

def verify_sig(filename):
    command="/opt/mono/bin/chktrust "+ filename + " 1>/dev/null 2>/dev/null"
    a=os.system(command)
    # os.system return 16 bits, and high 8 bits is status code, lucky, chktrust verify ok return 0, no  sig or untrusted cert are 1:)
    if not a:
        return 0
    else:
        return 1


if __name__== '__main__':
    a=verify_sig("/tmp/7za.exe")
    print a 
    b=verify_sig("/tmp/SoftwareUpdate.exe")
    print b
    c=verify_sig("/root/FF.exe")
    print c

        
