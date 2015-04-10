#!/usr/bin/env python
import cgi
import sys
sys.path.append("/root/sbx")

from sbxredis import *
from sbxutil import *

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
        #if the put file 
        if self.environ["REQUEST_METHOD"] ==  "POST":
            print self.environ
            return  self.save_file("foo")
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

    def save_file(self, filename):
        self.start('200 OK', [('Content-Type','text/html')])
	len=self.environ['CONTENT_LENGTH']
	request_body_size=int(len)
	f = open('/var/forensic/'+filename, 'a')
	body=self.environ['wsgi.input'].read(request_body_size)
	f.write(body)
        f.write('\n')
	f.close
	yield self.environ["REQUEST_METHOD"]+self.environ["PATH_INFO"]
        
