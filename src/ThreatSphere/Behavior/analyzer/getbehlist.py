#!/usr/bin/env python

import json
import sys
from dirutil import *

#one indicator return one list  result ,sub-indicators is a list 

def  cal_indicator(ioc_info,process_info):
	ret=[]
# first get item value list under the current indicator,this list will merge the indicator value later 
	if "IndicatorItem"   in ioc_info.keys():
		for i in cal_indicatoritem(ioc_info,process_info):
			ret.append(i)
#  two case here, a) one sub-indicator in current indicator b) many sub-indicator  
	if  "Indicator" in ioc_info.keys():
# one sub , directly do it 
		if type(ioc_info["Indicator"]) is dict:
			ret.append(cal_indicator(ioc_info["Indicator"],process_info))
# put every sub-indicator  value into list 
		else:
			for i in ioc_info["Indicator"]:
				ret.append(cal_indicator(i,process_info))
# 
	op=ioc_info["@operator"]
	print "The OP is %s"%(op)
	beh=0
	if op == "AND":
		beh=0xfffff
		for  i in ret:
			beh=beh&i	
			print "the i is %s" %(i)
		return beh
	if op == "OR":
		beh=0
		for i  in ret:
			beh=beh|i
		return beh 


type_map={
"FileItem": "file",
"ProcessItem":"process",
"RegistryItem": "registry"

}

action_map={
"CreateFile":"Created",
"CreateProcess":"created",
"SetRegistry":"SetValueKey"


}

#item is a dict of rule, return [0|1] matches this rule
def match_rule(item, process_info):
	matched =0
	condition=item["@condition"]
	type=item["Context"]["@document"]
	act=item["Context"]["@search"]
	content=item["Content"]["#text"]
	print condition, type, act, content
	match_type=type_map[type]
	match_action=action_map[act.split('\\')[1]]
	match_porperty= act.split('\\')[2]
	for   i    in  process_info["information"]:
		if  i["type"] ==match_type and i["action"] == match_action :
			print "we found  a type/action line %s,will calculate the rule,the match_type is %s, match_action is %s , match_porperty is %s , content is %s " %(i,match_type,match_action,match_porperty,content)
		#file stuff
			if match_type == "file":
				if condition == "regex_i":
					if content in i["object1"]:
						matched=1

				if condition == "is":
					if content == i["object1"]:
						matched=1
		

		#reg stuff
			if match_type == "registry":
				if condition == "regex_i":
					if content in i["object1"]:
						matched=1

				if condition == "is":
					if content == i["object1"]:
						matched=1
	return matched
#return a list [ 1, 0, 1 , 1... ] each is a return value of ruleN

def cal_indicatoritem(ioc_info,process_info):
	ret=[]
#if the ioc_info is dict, it means that there is only one item 
	if type(ioc_info["IndicatorItem"]) is dict:
		print "ther is only  one item", ioc_info["IndicatorItem"] 
		ret.append(match_rule(ioc_info["IndicatorItem"], process_info))
		print "the indicator item list value is %s" %(ret)
		return   ret
# multi items 
	print "there are items ", ioc_info["IndicatorItem"],  type(ioc_info["IndicatorItem"]), len(ioc_info["IndicatorItem"])
	for i in ioc_info["IndicatorItem"]:
		ret.append(match_rule(i,process_info))
	print "the indicator item list value is %s" %(ret)
	return ret


def get_process_behavior_list(process_info):
	ret =[]
	name=[]
	for i  in  get_files_for_dir('./behavior-json'):
		f=open(i, 'r')
		j=json.load(f)
		name.append(j["iob"]["description"])
		print "dealing with behavior %s" %(j["iob"]["description"])
		foo=cal_indicator(j["iob"]["definition"]["Indicator"], process_info)
		print  "the behavior is %d " %foo
		ret.append(foo)
	print  "behavior name list  %s" %(name)
	print  "behavior result %s"%(ret)
	return ret
if __name__== "__main__":
	#f=open(sys.argv[1], 'r')
	#j=json.load(f)
	process_info={"status": "done", "processId": "2776", "information": [{"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Yixin\\EasyChat\\YIXIN_SETUP_LOG.TXT", "object2": "-1", "time": "23/4/2015-17:53:5.11", "action": "Write", "type": "file", "utc_time": 1429782785.0}, {"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Yixin\\EasyChat\\YIXIN_SETUP_LOG.TXT", "object2": "-1", "time": "23/4/2015-17:53:5.11", "action": "Write", "type": "file", "utc_time": 1429782785.0}, {"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Yixin\\EasyChat\\YIXIN_SETUP_LOG.TXT", "object2": "-1", "time": "23/4/2015-17:53:5.27", "action": "Write", "type": "file", "utc_time": 1429782785.0}, {"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "192.168.150.9:37857", "object2": "46.43.34.31:80", "time": "23/4/2015-17:53:5.27", "action": "tcp-connection", "type": "connection", "utc_time": 1429782785.0}, {"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Yixin\\EasyChat\\YIXIN_SETUP_LOG.TXT", "object2": "-1", "time": "23/4/2015-17:53:5.27", "action": "Write", "type": "file", "utc_time": 1429782785.0}, {"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "HKEY_LOCAL_MACHINE\\SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", "object2": "-1", "time": "23/4/2015-17:53:5.11", "action": "SetValueKey", "type": "registry", "utc_time": 1429782785.0},{"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\\Local AppData", "object2": "-1", "time": "23/4/2015-17:53:5.11", "action": "SetValueKey", "type": "registry", "utc_time": 1429782785.0}, {"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Yixin\\EasyChat\\YIXIN_SETUP_LOG.TXT", "object2": "-1", "time": "23/4/2015-17:53:15.199", "action": "Write", "type": "file", "utc_time": 1429782795.0}, {"processId": "2776", "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "object1": "C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Yixin\\EasyChat\\YIXIN_SETUP_LOG.TXT", "object2": "-1", "time": "23/4/2015-17:53:15.199", "action": "Write", "type": "file", "utc_time": 1429782795.0}], "process": "C:\\WINDOWS\\Temp\\yixin_V2.5.1141.0_setup.1425285780.exe", "start_time": "23/4/2015-17:53:4.246", "start_utc_time": 1429782784.0, "stop_time": "23/4/2015-17:53:15.214", "network_information": [{"trans_depth": 1, "uid": "CczIfu2eGFeKOAtUsk", "request_body_len": 0, "response_body_len": 300, "id.orig_p": 37857, "id.resp_h": "46.43.34.31", "status_msg": "Found", "tags": [], "ts": 1429127877.4808271, "resp_fuids": ["FoSAqAOrP4ZUb93Xa"], "uri": "/~sgtatham/putty/latest/x86/putty.exe", "id.orig_h": "192.168.150.9", "id.resp_p": 80, "host": "the.earth.li", "resp_mime_types": ["text/html"], "user_agent": "Wget/1.12 (linux-gnu)", "status_code": 302, "method": "GET"}, {"trans_depth": 2, "uid": "CczIfu2eGFeKOAtUsk", "request_body_len": 0, "response_body_len": 524288, "id.orig_p": 37857, "id.resp_h": "46.43.34.31", "status_msg": "OK", "tags": [], "ts": 1429127877.835995, "resp_fuids": ["FDkoXl1oBNwvHO9xRa"], "uri": "/~sgtatham/putty/0.64/x86/putty.exe", "id.orig_h": "192.168.150.9", "id.resp_p": 80, "host": "the.earth.li", "resp_mime_types": ["application/x-dosexec"], "user_agent": "Wget/1.12 (linux-gnu)", "status_code": 200, "method": "GET"}], "stop_utc_time": 1429782795.0}
	#print "the beh result is %d" %cal_indicator(j["iob"]["definition"]["Indicator"], process_info)
	#print "the beh result is %d" %cal_indicator(j["definition"]["Indicator"])
	get_process_behavior_list(process_info)
