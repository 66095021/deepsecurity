#!/usr/bin/env python
import cgi
import sys
import json
import os
sys.path.append("/opt/ThreatSphere")

from sbxredis import *
from sbxutil import *
from logger  import * 
def check_root():
    start_response('200 OK', [('Content-Type','text/html')])
    return  ["What? root?"]

def upload_file():
    return ["what? uploadfile"]

# return ip from string of meta  of self.environ
def get_ip_info(d):
#make it to dict
    j=json.JSONDecoder().decode(d)
    logger.debug(j)
    logger.debug(type(j["details"]))
    logger.debug(j["details"])
    return(j["ts"], str(j["details"][0]).split(',')[0].split('=')[-1], str(j["details"][0]).split(',')[2].split('=')[-1])

class application:
    def __init__(self, environ, start_response):
        self.environ = environ
        self.start = start_response

    def __iter__(self):
        path = self.environ['PATH_INFO']
        #if the put file /,   put url  /url 
        if self.environ["REQUEST_METHOD"] ==  "PUT":
            if  path== "/":
                logger.debug("get a FILE request")
                logger.debug(self.environ)
                logger.debug(type(self.environ))
                print self.environ
                #print self.environ["HTTP_X_FILES"]
                #print self.environ["HTTP_FILENAME"]
                jobkey=get_uuid()
                add_job_queue(jobkey)
                add_history_queue(jobkey)
                jobinfo={}
                jobinfo['status']='uploaded'
                jobinfo['job_type']='file'
                jobinfo['filename']=self.environ["HTTP_FILENAME"]
                jobinfo['filepath']="/var/ThreatSphere/uploadfiles/"+self.environ["HTTP_FILENAME"]
                jobinfo['sha1sum']=self.environ["HTTP_SHA1SUM"]
                #add the meta  and client ip 
                jobinfo['meta'] = self.environ["HTTP_META"]
                k=json.JSONDecoder().decode(self.environ["HTTP_META"])
                client_ip=str(k["details"][0]).split(',')[0].split('=')[-1]
                jobinfo['client_ip']=client_ip
                add_info(jobkey,jobinfo)
                return  self.save_file(self.environ["HTTP_X_FILES"], self.environ["HTTP_FILENAME"])
            if path == "/url":
                logger.debug("get a  URL request")    
                logger.debug(self.environ)
                logger.debug("the url is %s" %(self.environ["HTTP_URL"]))
                #url is also a job to upload to clound
                jobkey=get_uuid()
                add_job_queue(jobkey)
                add_history_queue(jobkey)
                jobinfo={}
                jobinfo['url']=self.environ["HTTP_URL"]   
                jobinfo['job_type']='url'
                add_info(jobkey, jobinfo)
                #need more info from sniffer !!!!
                #(ts,src,dst)=get_ip_info(self.environ["HTTP_META"])
                info_dict={"url": self.environ["HTTP_URL"], 
		 #  "client_ip":   src,
		 #  "dst_ip" :dst  ,
		 #  "ts" : ts             
                }
                logger1.debug(info_dict)
                return self.GET_index()
        else:
            return self.notfound()

    def GET_index(self):
        status = '200 OK'
        response_headers = [('Content-type', 'text/plain')]
        self.start(status, response_headers)
        yield "Welcome!\n"

    def GET_hello(self):
        status = '200 OK'
        response_headers = [('Content-type', 'text/plain')]
        self.start(status, response_headers)
        yield "Hello world!\n"

    def notfound(self):
        status = '404 Not Found'
        response_headers = [('Content-type', 'text/plain')]
        self.start(status, response_headers)
        yield "Not Found\n"

    def save_file(self,info, filename):
        self.start('200 OK', [('Content-Type','text/html')])
	len=self.environ['CONTENT_LENGTH']
	request_body_size=int(len)
        os.system("mkdir -p /var/ThreatSphere/uploadfiles/")
	f = open('/var/ThreatSphere/uploadfiles/'+filename, 'w')
	body=self.environ['wsgi.input'].read(request_body_size)
	f.write(body)
	f.close()
        # we also write the meta into file, so that we can match information later
        tmp_dict={"filename": self.environ["HTTP_FILENAME"], 
                  "x-files":  self.environ["HTTP_X_FILES"],
                   "sha1sum": self.environ["HTTP_SHA1SUM"],
                   "meta"  : self.environ["HTTP_META"]
                 }
        k=json.JSONEncoder().encode(tmp_dict)
        print k
        f=open('/var/ThreatSphere/uploadfile.meta', 'a')
        f.write(k)
        f.write('\n')
        f.close()
        (ts,src,dst)=get_ip_info(self.environ["HTTP_META"])
        info_dict={"filename": self.environ["HTTP_FILENAME"], 
                   "sha1sum": self.environ["HTTP_SHA1SUM"],
		   "client_ip":   src,
		   "dst_ip" :dst  ,
		   "ts" : ts             
     }
        logger1.debug(info_dict)
	yield self.environ["REQUEST_METHOD"]+self.environ["PATH_INFO"]
        
