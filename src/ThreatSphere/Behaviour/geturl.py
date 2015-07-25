#!/usr/bin/env python 

import urllib2
import sys
import json
from bs4 import BeautifulSoup
from urlparse import urlparse
import tldextract

def get_orgin_dst_ip(filename):
	
	try:
		f=open("/var/ThreatSphere/uploadfile.meta", 'r')
		s=f.readlines()
		for i in s:
			i.strip()
			j=json.loads(i)
			if filename == j["filename"]:
				foo=   json.loads(j["meta"])
				return str(foo["details"][0]).split('\r\n')[0].strip('[]').split(',')[2].split("=")[1]
	except:
		return None
def get_orgin_client_ip(filename):
	
	try:
		f=open("/var/ThreatSphere/uploadfile.meta", 'r')
		s=f.readlines()
		for i in s:
			i.strip()
			j=json.loads(i)
			if filename == j["filename"]:
				foo=   json.loads(j["meta"])
				return str(foo["details"][0]).split('\r\n')[0].strip('[]').split(',')[0].split("=")[1]
	except:
		return None

def get_orgin_url(filename):
	try:
		f=open("/var/ThreatSphere/uploadfile.meta", 'r')
		s=f.readlines()
		for i in s:
			i.strip()
			j=json.loads(i)
			if filename == j["filename"]:
				foo=   json.loads(j["meta"])
				return foo["desc"]
	except:
		return None
def get_url_rank(url):
	try:
		o=urlparse(url)
		host=o.hostname
		#req=urllib2.Request('http://www.urlvoid.com/scan/xvideos.com/')
		#remove www. 
		#if 'www' in host:
		#	host=host[4:]
		domain=tldextract.extract(url).domain
		suffix=tldextract.extract(url).suffix
		host=domain+'.'+suffix
		print host
		req=urllib2.Request('http://www.urlvoid.com/scan/'+host)
		
		res=urllib2.urlopen(req) 
		
		tag=BeautifulSoup(res)
		
		table=tag.find("table", attrs={"class":"table table-bordered"})
		
		#print table
		# it likes 
		#[u'Analysis Date', u'12 hours ago', u'Safety Reputation', u'1/30', u'Domain 1st Registered', u'1997-12-30 (18 years ago)', u'Server Location', u' (NL) Netherlands', u'Google Page Rank', u'', u'Alexa Traffic Rank', u'44']
		result=[]
		for row in table.find_all("tr")[0:]:
			#print type(row.find_all("td"))
			for td in row.find_all("td"):
				result.append(td.get_text())
		
		
		print result
		scan_item=result[3]
		alexa_item=result[-1]
		if alexa_item  ==  'Unknown':
			alexa_item='5555555555'
		scan_rank=int(scan_item[0:scan_item.find('/')])
		alexa_rank=int(alexa_item.replace(',', ''))
		return [result, scan_rank, alexa_rank]
	except:
		return [None, -2, -2]


#argv shoule be scheme://hostname/url../
if __name__ == '__main__':
	print get_orgin_client_ip("putty.exe")
	print get_orgin_dst_ip("putty.exe")
	print get_orgin_url("putty.exe")
