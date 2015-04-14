#!/usr/bin/env python
import json
import base64
import sys
sys.path.append("/opt/ATI")
from logger import * 
#filename /opt/bro/**/fuid.type, so get the fuid firstly
from analyzer_config import * 
def get_line_contain_fuid(filename):
    fuid=filename.split('/')[-1].split('.')[0]
    logger.debug("the fuild is %s" %(fuid))
    f=open(file_meta_info_dir,'r')

    for line in f:
        d=json.JSONDecoder().decode(line)
        if str(d["fuid"])==fuid:
            f.close()
            return line.strip('\n')
    f.close()
	
def get_real_name_from_fuid(filename):
    fuid=filename.split('/')[-1].split('.')[0]
    logger.debug("the fuild is %s" %(fuid))
    f=open(file_meta_info_dir,'r')

    for line in f:
        d=json.JSONDecoder().decode(line)
        if str(d["fuid"])==fuid:
            f.close()
            return  d["desc"].split('/')[-1]
    f.close()

if __name__== "__main__":
    print get_line_contain_fuid(  sys.argv[1])
    print get_real_name_from_fuid(sys.argv[1])
