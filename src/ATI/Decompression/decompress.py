#!/usr/bin/env python

import tarfile 
import zipfile
import rarfile
import sys

sys.path.append("/opt/ATI")
from dirutil import * 
from logger  import * 

#tar = tarfile.open("/root/osslsigncode-1.7.1.tar.gz", "r")
#tar = tarfile.open("/root/test.tar.bz2", "r")

#tar.extractall("/tmp/fuck/")


#this function should uncompression, and return a list contains all decompressed file 
def uncompression(filename):
    #we can do it by tarfile
    if tarfile.is_tarfile(filename):
        logger.debug("file %s is tar file"%(filename))
        tar = tarfile.open(filename, "r")
        tar.extractall("/tmp/decompression/"+filename.split('/')[-1])
        info="/tmp/decompression/"+filename.split('/')[-1]
        logger.debug("untar it to %s" %(info))
        ret=get_files_for_dir("/tmp/decompression")    
        logger.debug(ret)
        return ret
    if  zipfile.is_zipfile(filename):
        zfile = zipfile.ZipFile(filename)
        zfile.extractall("/tmp/decompression/"+filename.split('/')[-1])
        ret=get_files_for_dir("/tmp/decompression")
        return ret

    if  rarfile.is_rarfile(filename):
        rfile = rarfile.RarFile(filename)
        rfile.extractall("/tmp/decompression/"+filename.split('/')[-1])

        logger.debug("unrar it to /tmp/decompression/filename")
        ret=get_files_for_dir("/tmp/decompression")
        for i in ret:
            logger.debug("the list is %s" %ret)
            #uncompression(i)
        return ret

if __name__ =='__main__':
	ret=uncompression(sys.argv[1])
        print ret
