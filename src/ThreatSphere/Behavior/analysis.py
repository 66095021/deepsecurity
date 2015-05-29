#!/usr/bin/env python

import json
import sys
from dirutil import *
from logger import * 
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
"CreateFile":"Create",
"CreateProcess":"created",
"SetRegistry":"SetValueKey"


}

#A\B\C
def break_search_text(search):
	type=type_map[search.split('\\')[0]]
	action=action_map[search.split('\\')[1]]
	property=search.split('\\')[2]
	logger.debug("break the %s into %s %s %s"%(search,type,action,property))
	return (type, action, property)

#return a list contains the value matched, used for cross_match value
def get_value_list(process_info,match_type, match_action,match_property=None):
	logger.debug("try to find out the result from process info ")
	ret=[]
	for   i    in  process_info["information"]:
		if  i["type"] ==match_type and i["action"] == match_action :
			if match_type == "file":
				ret.append(i["object1"])
			# object1 is PID, object2 is the path
			if match_type == "process":
				ret.append(i["object2"])
			if match_type  ==  "registry":
				ret.append(i["object1"])
	logger.debug("the value list for cross_match is %s"%(ret))
	return ret 


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
	match_property= act.split('\\')[2]
	logger.debug("in match_rule function, the match information is type %s, action %s, property %s" %(match_type,match_action,match_property))
	for   i    in  process_info["information"]:
		if  i["type"] ==match_type and i["action"] == match_action :
			logger.debug( "we found  a type/action line %s,will calculate the rule,the match_type is %s, match_action is %s , match_property is %s , content is %s " %(i,match_type,match_action,match_property,content))
		#file stuff
			if match_type == "file":
				if condition == "regex":
					if content in i["object1"]:
						matched=1

				if condition == "is":
					if content == i["object1"]:
						matched=1
		
				if condition == "regex_i":
					if content.upper() in i["object1"].upper():
						matched=1

		

		#reg stuff
			if match_type == "registry":
				if condition == "regex":
					if content in i["object1"]:
						matched=1

				if condition == "is":
					if content == i["object1"]:
						matched=1
				if condition == "regex_i":
					if content.upper() in i["object1"].upper():
						matched=1
		#process stuff
			if match_type == "process":
				logger.debug("bobo match the process rule,the process path is %s"%(i["object2"]))
				if condition == "regex":
					if content in i["object1"]:
						matched=1

				if condition == "is":
					if content == i["object1"]:
						matched=1
				if condition == "regex_i":
					if content.upper() in i["object1"].upper():
						matched=1
				# it means the content value is other rule value instead of literal text
				if condition ==  "cross_match":
					#find matched items to get the value 
					(type, action, property)=break_search_text(content)				
					value_list=get_value_list(process_info,type, action)
					for   foo  in value_list:
						if i["object2"] == foo:
							matched =1 
				if condition ==  "cross_match_i":
					(type, action, property)=break_search_text(content)				
					#find matched items to get the value 
					value_list=get_value_list(process_info,type, action)
					for   foo in value_list:
						if i["object2"].upper() == foo.upper():
							matched =1 
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
	for i  in  get_files_for_dir('./analyzer/behavior-json'):
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


