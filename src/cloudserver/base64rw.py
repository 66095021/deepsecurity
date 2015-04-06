#!/usr/bin/env python
import base64
#f=open('/tmp/7za.exe','r') #..........
#ls_f=base64.b64encode(f.read()) #..........base64.. 
#print ls_f
#d=open("/tmp/testxx","w")
#d.write(ls_f)
#d.close()
#f.close()
#e=open("/tmp/testxx","r")
#foo=base64.b64decode(e.read())
#a=open("/tmp/testxx.back","w")
#a.write(foo)
#
#a.close()

def get_base64_encode_string(file):
    f=open(file,'r')
    f_encode=base64.b64encode(f.read())
    f.close()
    return f_encode

def get_file_from_base64_string(content,file):
    decode_string=base64.b64decode(content)
    a=open(file,"w")
    a.write(decode_string)
    a.close()


if __name__ == "__main__":

   encode=get_base64_encode_string("/tmp/7za.exe")
   get_file_from_base64_string(encode,"/tmp/77.exe")
