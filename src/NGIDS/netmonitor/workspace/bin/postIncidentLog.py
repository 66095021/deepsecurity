#!/usr/bin/env python
import httplib
import subprocess
import time
import sys
import getopt
import json

def send_incident_log_to_server(server, body):
	#print server,dic
	try:
		conn=httplib.HTTPConnection(server, 9999)
		headers={"infotype":"incident"}
		conn.request("POST", "/", json.JSONEncoder().encode(body) ,headers)

		response = conn.getresponse()
		remote_file = response.read()
		conn.close()
		print remote_file
	except Exception,ex:
		print  "failed to send incident log to server",ex

def usage():
	print "usage:"
	print "   -S incident server"
	print "   -I src ip"
	print "   -i dst ip"
	print "   -P src port"
	print "   -p dst port"
	print "   -H request header"
	print "   -h response header"
	print "   -L request body length"
	print "   -l response body length"
	print "   -M http method"
	print "   -U URL"
	print "   -V http version"
	print "   -c response code"
	print "   -t incident type"
	print "   -D detector, 0 maliciou URL; 1 sophos; 2 sandbox"
	print "   -d directon, 0 inbound; 1 outbound"



if __name__ == "__main__":
	opts, args = getopt.getopt(sys.argv[1:], "S:I:i:P:p:H:h:L:l:M:U:V:c:t:D:d:", ["help"])
	incidenti_log_server="127.0.0.1"
	dic = { "src_ip":"8.8.8.8",
		"dst_ip":"9.9.9.9",
		"src_port":"1234",
		"dst_port":"80",
		"req_hdr":r"GET / HTTP1.1\u000a\u000dHost: www.baidu.com\u000a\u000d\u000a\u000d",
		"res_hdr":r"200 OK\u000a\u000dContent-Length: 5\u000a\u000d\u000a\u000dabcde",
		"req_body_len":"0",
		"res_body_len":"5",
		"method":"GET",
		"version":"HTTP/1.1",
		"res_code":"200",
		"type":"MAL_HOSTNAME1",
		"detector":"0",
		"direction":"1",
		}
 
	for op, value in opts:
		if op == "-S":
			incident_server = value
		elif op == "-I":
			dic["src_ip"] = value
		elif op == "-i":
			dic["dst_ip"] = value
		elif op == "-P":
			dic["src_port"] = value
		elif op == "-p":
			dic["dst_port"] = value
		elif op == "-H":
			dic["req_hdr"] = value
		elif op == "-h":
			dic["res_hdr"] = value
		elif op == "-L":
			dic["req_body_len"] = value
		elif op == "-l":
			dic["res_body_len"] = value
		elif op == "-M":
			dic["method"] = value
		elif op == "-V":
			dic["version"] = value
		elif op == "-c":
			dic["res_code"] = value
		elif op == "-t":
			dic["type"] = value
		elif op == "-D":
			dic["detector"] = value
		elif op == "-d":
			dic["direction"] = value
		elif op == "--help":
			usage()
			sys.exit()
	#print dic
	send_incident_log_to_server(incidenti_log_server, dic)
	sys.exit()
