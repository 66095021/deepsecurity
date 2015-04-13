#!/usr/bin/env python
import json
import base64
import sys
sys.path.append("/opt/ATI")
from logger import * 
#filename /**/***/fuid.type, so get the fuid firstly

def get_line_contain_fuid(filename):
    fuid=filename.split('/')[-1].split('.')[0]
    logger.debug("the fuild is %s" %(fuid))
    f=open('./files-HTTP.log','r')

    for line in f:
        d=json.JSONDecoder().decode(line)
        if str(d["fuid"])==fuid:
            return line.strip('\n')
    f.close()
	


if __name__== "__main__":
    print get_line_contain_fuid("FF2mYu2Kz7OEsUg8lg.text")
