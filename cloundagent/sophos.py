#!/usr/bin/env python
import os 

import time


#return 1 if virus, else 0
#there is a FIFO which is readed by really sophos virus program we made.
# there is output /tmp/sophos_output. read it to get whether virus
def sophos_it(filename):
#send the name to sophos program
    command="echo "+ filename+">/tmp/sophos_fifo &";
    print "send the name to sophos fifo\n"
    os.system(command)
#check the result 
    time.sleep(6)
# 2 means waiting the result, 0  means no virus, 1 means virus
    ret=2
#iterate the file
    f=open('/tmp/sophos_out','r')
    for line in f:
        print "iterate the sophos_out %s to find the result \n" %line
        #print    type(line)
        if line.split(" ")[0].strip()== filename:
            ret=int(line.split(" ")[1].strip())
            print "Now, we match the result %s  the ret is %d\n" %(line,ret)
            break
    f.close()
    return ret
if __name__== '__main__':
    print sophos_it("/tmp/7za.exe")
    print sophos_it("/root/sophos-av/ADVENT.COM")

    
