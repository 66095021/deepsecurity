#!/usr/bin/env python 
import json
import base64
f=open('/var/forensic/foo','r')

for line in f:
    d=json.JSONDecoder().decode(line)
    filename=str(d["name"])
    print filename
    #print d["body"]
f.close()
decode_string=base64.b64decode(d["body"])
a=open("/var/forensic/file/"+filename,"w")
a.write(decode_string)
a.close()
