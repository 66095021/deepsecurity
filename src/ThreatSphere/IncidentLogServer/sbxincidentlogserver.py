#!/usr/bin/env python
import cgi
import sys
import os
import json
import base64
sys.path.append("/opt/ThreatSphere")

from sbxredis import *
from sbxutil import *
from logger import * 
def check_root():
    start_response('200 OK', [('Content-Type','text/html')])
    return  ["What? root?"]

def upload_file():
    return ["what? uploadfile"]



class application:
    def __init__(self, environ, start_response):
        self.environ = environ
        self.start = start_response

    def __iter__(self):
        path = self.environ['PATH_INFO']
         
        os.system("mkdir -p  /var/forensic/file_repos")
        os.system("mkdir -p  /var/incident")
        
        print self.environ
        #log or forensic is diffed by  information_type in POST header, information is JSON in the body
        if self.environ["REQUEST_METHOD"] ==  "POST":
            if self.environ["HTTP_INFOTYPE"] == "forensic":
                logger.debug("receive the forensic request, will save it and extract file")
                return self.save_forensic_data()
            if self.environ["HTTP_INFOTYPE"] == "incident":
                logger.debug("receive incident log, will save it")
                return self.save_incident_log()
            #return  self.save_incident_or_forensic("foo")
        if path == "/":
            print self.environ
            return self.GET_index()
        elif path == "/hello":
            return self.GET_hello()
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

    def save_incident_or_forensic(self, filename):
        self.start('200 OK', [('Content-Type','text/html')])
	len=self.environ['CONTENT_LENGTH']
	request_body_size=int(len)
	f = open('/var/forensic/'+filename, 'a')
	body=self.environ['wsgi.input'].read(request_body_size)
	f.write(body)
        f.write('\n')
	f.close
	yield self.environ["REQUEST_METHOD"]+self.environ["PATH_INFO"]
     # for forensic, 1) save the JSON to json_data 2)extract the file to /var/forensic/file_repos/ timely   
    def save_forensic_data(self):
        self.start('200 OK', [('Content-Type','text/html')])
	len=self.environ['CONTENT_LENGTH']
	request_body_size=int(len)
	f = open('/var/forensic/'+"json_data", 'a')
	body=self.environ['wsgi.input'].read(request_body_size)
	f.write(body)
        f.write('\n')
	f.close
        #extract the file 
        decode_content=json.JSONDecoder().decode(body)
        #kick off the u char
        filename=str(decode_content["name"])
        a=open("/var/forensic/file_repos/"+filename, "w")
        #file content is base64 encode in JSON body section, so decode it 
        filebody=base64.b64decode(decode_content["body"])
        a.write(filebody)
        a.close()
	yield self.environ["REQUEST_METHOD"]+self.environ["PATH_INFO"]

    #for incident log, just save it to log file /var/incident/json_data
    def save_incident_log(self):
        self.start('200 OK', [('Content-Type','text/html')])
	len=self.environ['CONTENT_LENGTH']
	request_body_size=int(len)
	f = open('/var/incident/json_data', 'a')
	body=self.environ['wsgi.input'].read(request_body_size)
	f.write(body)
        f.write('\n')
	f.close
	yield self.environ["REQUEST_METHOD"]+self.environ["PATH_INFO"]

