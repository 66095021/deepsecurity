#!/usr/bin/env python
import json
import  os 
import base64
import sys
sys.path.append("/opt/ATI")
from logger import * 
#filename /opt/bro/**/fuid.type, so get the fuid firstly

#deal a speical case, if the files is from uncompression, so there is no fuid,no meta
#we have to connect the uncompression file to the original compression file.


from analyzer_config import * 
def get_line_contain_fuid(filename):
    if not os.path.exists(file_meta_info_dir):
        logger.debug("file meta file does NOT existt")
        return 
#normal case,
    if not "/tmp/decompression" in filename:
       fuid=filename.split('/')[-1].split('.')[0]
       logger.debug("the file %s fuild is %s" %(filename,fuid))
       f=open(file_meta_info_dir,'r')

       for line in f:
           d=json.JSONDecoder().decode(line)
           if str(d["fuid"])==fuid:
               f.close()
               return line.strip('\n')
       f.close()
       return 
#compression case /tmp/decompression/FElMyD460R1MCjJ2Pg.application/PUTTY.EXE
    else:
       fuid=filename.split('/')[3].split('.')[0]
       logger.debug("it is a specical case, uncompression file, the original file  fuild is %s" %(fuid))
       f=open(file_meta_info_dir,'r')

       for line in f:
           d=json.JSONDecoder().decode(line)
           if str(d["fuid"])==fuid:
               f.close()
               return line.strip('\n')
       f.close()
       return 
def get_real_name_from_fuid(filename):
    if not os.path.exists(file_meta_info_dir):
        logger.debug("file meta file does NOT existt")
        return 
#normal case 
    if not "/tmp/decompression" in filename:
        fuid=filename.split('/')[-1].split('.')[0]
        logger.debug("the fuild is %s" %(fuid))
        f=open(file_meta_info_dir,'r')

        for line in f:
            d=json.JSONDecoder().decode(line)
            if str(d["fuid"])==fuid:
                f.close()
                return  d["desc"].split('/')[-1]
        f.close()
        return 
#compression case,just return the uncompression file name 

    else:
        return filename.split('/')[-1]

if __name__== "__main__":
    print get_line_contain_fuid(  sys.argv[1])
    #print get_real_name_from_fuid(sys.argv[1])
