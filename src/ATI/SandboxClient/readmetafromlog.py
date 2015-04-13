#!/usr/bin/python
import json
import sys
sys.path.append("/opt/ATI")
from logger import * 
#filename /**/***/fuid.type, so get the fuid firstly

def get_client_ip_from_metalog(hashvalue):
    f=open('/var/ATI/uploadfile.meta','r')

    for line in f:
        d=json.JSONDecoder().decode(line)
        if str(d["sha1sum"]) == hashvalue:
            f.close()
            #meta is a JSON, get the client ip from details KEY 
            k=json.JSONDecoder().decode(d["meta"])
            print type(k["details"])
	    return str(k["details"][0]).split(',')[0].split('=')[-1]
	


if __name__== "__main__":
    print get_client_ip_from_metalog("ab7a4bd613f132682f1166060bb6b456305556f0")

