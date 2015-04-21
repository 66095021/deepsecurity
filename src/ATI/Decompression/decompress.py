#!/usr/bin/env python

import tarfile 
import zipfile
import rarfile
import sys
import os
sys.path.append("/opt/ATI")
from dirutil import * 
from logger  import * 

#tar = tarfile.open("/root/osslsigncode-1.7.1.tar.gz", "r")
#tar = tarfile.open("/root/test.tar.bz2", "r")

#tar.extractall("/tmp/fuck/")

#find all files  in /tmp/decompression/foo/, if it is 
def recursion_dir(pathname):
    while True:
        files=get_files_for_dir(pathname)
        logger.debug("files in pathname is %s" %(files))
        for i  in files:
            if tarfile.is_tarfile(i):
                tarfile.open(i).extractall(pathname)
                os.remove(i)
            if  zipfile.is_zipfile(i):
                zipfile.ZipFile(i).extractall(pathname)
                os.remove(i)
            if  rarfile.is_rarfile(i):
                logger.debug("%s is a rar,extract it to %s "%(i,pathname))
                rarfile.RarFile(i).extractall(pathname)
                os.remove(i)
                logger.debug("rm -rf "+i)
        #can we exit now 
        
        compression=0
        for  i  in get_files_for_dir(pathname):
            if  tarfile.is_tarfile(i) or zipfile.is_zipfile(i) or  rarfile.is_rarfile(i):
                compression=1
        #still have compression file, keep while 
        if compression==1:
            continue   
        return

    


#this function should uncompression, and return a list contains all decompressed file 
def uncompression(filename):
    try:
    #we can do it by tarfile
        if tarfile.is_tarfile(filename):
            logger.debug("file %s is tar file"%(filename))
            tar = tarfile.open(filename, "r")
            tar.extractall("/tmp/decompression/"+filename.split('/')[-1])
            recursion_dir("/tmp/decompression/"+filename.split('/')[-1])
            ret=get_files_for_dir("/tmp/decompression")    
            return ret
        if  zipfile.is_zipfile(filename):
            zfile = zipfile.ZipFile(filename)
            zfile.extractall("/tmp/decompression/"+filename.split('/')[-1])
            recursion_dir("/tmp/decompression/"+filename.split('/')[-1])
            return get_files_for_dir("/tmp/decompression")

        if  rarfile.is_rarfile(filename):
            rfile = rarfile.RarFile(filename)
            rfile.extractall("/tmp/decompression/"+filename.split('/')[-1])

            logger.debug("unrar it to /tmp/decompression/%s" %(filename.split('/')[-1]))
            recursion_dir("/tmp/decompression/"+filename.split('/')[-1])
            return get_files_for_dir("/tmp/decompression")

    except:
          return  None
if __name__ =='__main__':
	ret=uncompression(sys.argv[1])
        print ret
