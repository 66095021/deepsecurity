#!/usr/bin/env python

import json
import sys
from dirutil import *
from logger import * 

from map_table import * 


#one indicator return one list  result ,sub-indicators is a list 

def  cal_indicator(ioc_info,process_info,parent_code):
	logger.debug("go into cal_indicator")
	if "@mode"  not in  ioc_info.keys() or  ioc_info["@mode"]==0 or ioc_info["@mode"] == "0":
		logger.debug("there is no mode or mode =0 in cal_indicator")
#ret is the op bool value, code is the rating if the indicator is 1 
		ret=[]
		current_sub=[]
		if "@code" not in ioc_info.keys():
			code=parent_code
			ioc_info["@code"]=parent_code
		else:
			code=ioc_info["@code"]
# first get item value list under the current indicator,this list will merge the indicator value later 
		if "IndicatorItem"   in ioc_info.keys():
			for i in cal_indicatoritem(ioc_info,process_info):
				current_sub.append(i)
				ret.append(i[0])
#  two case here, a) one sub-indicator in current indicator b) many sub-indicator  
		if  "Indicator" in ioc_info.keys():
# one sub , directly do it 
			if type(ioc_info["Indicator"]) is dict:
				j=cal_indicator(ioc_info["Indicator"],process_info,code)
				ret.append(j[0])
				current_sub.append(j)
# put every sub-indicator  value into list 
			else:
				for i in ioc_info["Indicator"]:
					k=cal_indicator(i,process_info,code)
					current_sub.append(k)
					ret.append(k[0])
#find the max code,  return [0|1, code], 0|1 is oped on ret,  code is oped on current_sub
		logger.debug("the current indicator sub value and code is %s"%(current_sub))
		for  i   in  current_sub:
			if i[0]==1:
				if i[1] > code:
					code=i[1]

 
		op=ioc_info["@operator"]
		print "The OP is %s"%(op)
		beh=0
		if op == "AND":
			beh=0xfffff
			for  i in ret:
				beh=beh&i	
				print "the i is %s" %(i)
			return [beh,code]
		if op == "OR":
			beh=0
			for i  in ret:
				beh=beh|i
			return [beh,code] 
#		if op == "++":
#			i=0
#			for  j in  ret:
#				if j == 1:
#					i+=1
#			if i ==0: 
#				return  [i,0]
#			else:
#				return   [1,i]

#if ++,  return the number sum of each  match  times for  items.  item1 match log 10, item2 match log 20. then 
#return  [30,30]   
		if   op  == "++":
			logger.debug("it is an ++ operator, calculate the times of match")
			rest=cal_indicatoritem_times(ioc_info,process_info)
			return [rest, rest]
	logger.debug("there is specical mode  in cal_indicator")
#spec case,  use every log to iter the indicator and sub-indicator, if there is 1 for any log, the indicator is 1
#ret contains the result list of the indicator when log1 log2 ... 
#ret is or binary calculating, current_sub  contains [0|1, code]
#	ret=[]
#	current_sub=[]
#	code=None
#	and_list=[]
#	or_max=0

	for   i    in  process_info["information"]:
		ret=[]
		current_sub=[]
		code=None
		if "@code" not in ioc_info.keys():
			code=parent_code
			ioc_info["@code"]=parent_code
		else:
			code=ioc_info["@code"]
		and_list=[]
		or_max=0
		logger.debug("using log  %s to calculating the now "%(i) )
		for j  in (call_items_log(ioc_info, i,process_info)):
			logger.debug("the current item result is %s"%j)
			ret.append(j[0])
			current_sub.append(j)
#  two case here, a) one sub-indicator in current indicator b) many sub-indicator  
		if  "Indicator" in ioc_info.keys():
# one sub , directly do it 
			if type(ioc_info["Indicator"]) is dict:
				foo=call_indicator_log(ioc_info["Indicator"],i,process_info, code)
				ret.append(foo[0])
				current_sub.append(foo)
# put every sub-indicator  value into list 
			else:
				for j in ioc_info["Indicator"]:
					foo=call_indicator_log(j,i,process_info, code)
					ret.append(foo[0])
					current_sub.append(foo)

		logger.debug("the current sub elements result is %s"%(current_sub))		
#calculate the OP value when log rolling 
		k=op_it(ioc_info,ret)
#calculate  the code of AND and OR 
		if ioc_info["@operator"] == "AND":
			for  i  in current_sub:
				and_list.append(i[1])
			logger.debug("the and_list now is %s"%(and_list))
		if ioc_info["@operator"] == "OR":
			for  i  in current_sub:
				if i[1]> or_max:
					or_max=i[1]


#once the OP is 1, return  right now 
		if k == 1:
			if ioc_info["@operator"] == "AND":
				return [1,and_list]
			 
			
			if ioc_info["@operator"] == "OR":
				return [1,or_max]

#go through all logs, but no lucky, return  0 
	if ioc_info["@operator"] == "AND":
		return [0, and_list] 
	if ioc_info["@operator"] == "OR":
		return [0, or_max]


# get the indicator value , ret contains all items and directly sub-indicators
def op_it(node, ret):
	print (type(ret))
	print ret
	op=node["@operator"]
	beh=0
	if op == "AND":
		beh=0xfffff
		for  i in ret:
			print "the i is %s" %(i)
			beh=beh&i	
		return beh
	if op == "OR":
		beh=0
		for i  in ret:
			print "the i is %s" %(i)
			print type(i)
			beh=beh|i
		return beh 

#when log item roll in , the indicator value return [0|1] when log roll 
def call_indicator_log(node, log, process_info, parent_code):

#ret is binary result ,  current_sub contains binary and code value 
	ret=[] 
	code=None
	if "@code" not in node.keys():
		code=parent_code
		node["@code"]=parent_code
	else:
		code=node["@code"]
	and_list={}
	or_max=0
	current_sub=[]
	logger.debug("go into cal_indicator_log the indicator OP is %s"%(node["@operator"]))
	for i  in call_items_log(node,log,process_info):
		ret.append(i[0])
		current_sub.append(i)
	if "Indicator" in node.keys():
		if node["Indicator"].__class__.__name__ == "dict": 
			j=call_indicator_log(node["Indicator"] , log,code)
			ret.append(j[0])
			current_sub.append(j)
		else:
			for i  in  node["Indicator"]:
				j=call_indicator_log(i,log,code)
				ret.append(j[0])
				current_sub.append(j)



#if it is AND, return a list   OR retuen max


	k=op_it(node, ret)
	if node["@operator"] == "AND":
		for  i  in current_sub:
			and_list.append(i[1])
		logger.debug("the and_list in call_indicator_log is %s"%(and_list))
	if node["@operator"] == "OR":
		for  i  in current_sub:
			if i[1]> or_max:
				or_max=i[1]
		logger.debug("in call_indicator_log  the OR or_max is %d" %or_max)


	if k == 1:
		if node["@operator"] == "AND":
			logger.debug("It is AND, will return %d %s"%(k,and_list))
			return [1,and_list]
		 
			
		if node["@operator"] == "OR":
			logger.debug("It is OR,  will return %d %d"%(k,or_max))
			return [1,or_max]


	if k == 0:
		if node["@operator"] == "AND":
			logger.debug("It is AND, will return %d %s"%(k,and_list))
			return [k,and_list]
		 
			
		if node["@operator"] == "OR":
			logger.debug("It is OR, will return %d %d"%(k,or_max))
			return [k,or_max]


# return list of indicator items value when log roll in
def call_items_log(ioc_info, log,process_info):
	ret=[]
#if the ioc_info is dict, it means that there is only one item 
	if type(ioc_info["IndicatorItem"]) is dict:
		print "ther is only  one item", ioc_info["IndicatorItem"] 
		ret.append(match_rule_log(ioc_info["IndicatorItem"], process_info,log,ioc_info["@code"]))
		print "the indicator item list value is %s" %(ret)
		return   ret
# multi items 
	print "there are items ", ioc_info["IndicatorItem"],  type(ioc_info["IndicatorItem"]), len(ioc_info["IndicatorItem"])
	for i in ioc_info["IndicatorItem"]:
		ret.append(match_rule_log(i,process_info,log, ioc_info["@code"]))
	print "the indicator item list value is %s" %(ret)
	return ret



type_map={
"FileItem": "file",
"ProcessItem":"process",
"RegistryItem": "registry"

}

action_map={
"CreateFile":"Create",	
"WriteFile":"Write",
"ReadFile":"Read",
"CreateProcess":"created",
"SetRegistry":"SetValueKey",
"QueryRegistry":"GetValueKey"

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


#get values matched A\B\C from logs,return a list  
def cal_container_value(item, process_info):
	(type, action, property)=break_search_text(item)
	return get_value_list(process_info,type,action,property)

# return [0|1] matches this rule when one log roll in 
def match_rule_log(item, process_info,log,parent_code):


	matched =0
	condition=item["@condition"]
	type=item["Context"]["@document"]
	act=item["Context"]["@search"]
#support array 
	content_array_flags=0
	content=None
	content_list=[]
# code store this item rating, code_item is for arary item . content_list is [[ a1, code] ,[a2,code]...]
	code=None
	container_list=[]
	code_item=None
#code_return is whether normal item rating ,or a rating from array 
	code_return=None
	if item["Content"]["@type"] == "string":
		content=item["Content"]["#text"]
		content=format_content(content)
		if "@code" not in item["Content"]:
			code=parent_code
		else:
			code=item["Content"]["@code"]
		print condition, type, act, content
#only one element, it is a dict instead of list 
	elif item["Content"]["@type"] == "array" and item["Content"]["Item"].__class__ is dict:
		content=item["Content"]["Item"]["#text"]
		if "@code" not in item["Content"]["Item"]:
			code=parent_code
		else:
			code=item["Content"]["Item"]["@code"]


	elif item["Content"]["@type"] == "array" and item["Content"]["Item"].__class__ is list :
		content_array_flags=1
		print item["Content"]
		for i  in item["Content"]["Item"]:
			if "@code" not in item["Content"]["Item"]:
				code_item=parent_code
			else:
				code_item=item["Content"]["Item"]["@code"]

			if "#text"  in  i.keys():
				org=i["#text"]
				org=format_content(org)
				content_list.append([org,code_item])

#support container, content_list now contains all matched log from container if has. 
#content_list is still [[match1,code] , [match2,code]... ] list 
	elif item["Content"]["@type"] == "container": 

		logger.debug("it is a container  type")
		content_array_flags = 1 
		container_list=cal_container_value(item["Content"]["#text"],process_info)

#deal code to form content_list
		if "@code" not in item["Content"].keys():
			code_item=parent_code
		else:
			code_item=item["Content"]["@code"]
		for g in  container_list:
			content_list.append([g,code_item])
	#print condition, type, act, content
	match_type=type_map[type]
	match_action=action_map[act.split('\\')[1]]
	match_property= act.split('\\')[2]

######
	logger.debug("in match_rule function, the match information is type %s, action %s, property %s" %(match_type,match_action,match_property))
# some process do nothing, so no information 
        if "information" not in process_info.keys():
            return [0,0]
	list_fake=[]
	list_fake.append(log)
	for   i    in  list_fake:


		if  i["type"] ==match_type and i["action"] == match_action :
			logger.debug( "we found  a type/action line %s,will calculate the rule,the match_type is %s, match_action is %s , match_property is %s , content is %s " %(i,match_type,match_action,match_property,content))
			if content_array_flags  == 0:
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
						logger.debug(content)
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
			if content_array_flags  == 1:
			#file stuff
				if match_type == "file":
					for  j  in  content_list:
						if condition == "regex":
							if j[0] in i["object1"]:
								matched=1
								code=j[1]
					if condition == "is":
						for j  in content_list:
							if j[0] == i["object1"]:
								matched=1
								code=j[1]
					if condition == "regex_i":
						for j  in content_list:
							print content_list
							if j[0].upper() in i["object1"].upper():
								matched=1
								code=j[1]
	
			
	
			#reg stuff
				if match_type == "registry":
					if condition == "regex":
						for j in content_list:
							if j[0] in i["object1"]:
								matched=1
								code=j[1]
	
					if condition == "is":
						for j[0] in content_list:
							if j == i["object1"]:
								code=j[1]
								matched=1
					if condition == "regex_i":
						for j in content_list:
							if j[0].upper() in i["object1"].upper():
								matched=1
								code=j[1]
			#process stuff
				if match_type == "process":
					logger.debug("bobo match the process rule,the process path is %s"%(i["object2"]))
					if condition == "regex":
						for j  in content_list:
							if j[0] in i["object1"]:
								matched=1
								code=j[1]
	
					if condition == "is":
						for j[0]  in content_list:
							if j == i["object1"]:
								code=j[1]
								matched=1
					if condition == "regex_i":
						for j  in content_list:
							if j[0].upper() in i["object1"].upper():
								matched=1
								code=j[1]
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



#############
	return [matched, 0]

#it calculate  the times of each matched item 
def cal_indicatoritem_times(ioc_info,process_info):
	ret=0
#if the ioc_info is dict, it means that there is only one item 
	if type(ioc_info["IndicatorItem"]) is dict:
		logger.debug( "ther is only  one item", ioc_info["IndicatorItem"])
		ret=match_rule_times(ioc_info["IndicatorItem"], process_info,ioc_info["@code"])
		logger.debug("the indicator item match times value is %d" %(ret))
		return   ret
# multi items 
	print "there are items ", ioc_info["IndicatorItem"],  type(ioc_info["IndicatorItem"]), len(ioc_info["IndicatorItem"])
	for i in ioc_info["IndicatorItem"]:
		ret+=match_rule_times(i,process_info,ioc_info["@code"])
	logger.debug( "the indicator item match list match value sum is %d" %(ret))
	return ret
# it calculate  process_info logs match how much times from item 
def match_rule_times(item, process_info, parent_code):
	matched =0
	condition=item["@condition"]
	type=item["Context"]["@document"]
	act=item["Context"]["@search"]
#support array 
	content_array_flags=0
	content=None
	content_list=[]
# code store this item rating, code_item is for arary item . content_list is [[ a1, code] ,[a2,code]...]
	code=None
	container_list=[]
	code_item=None
#code_return is whether normal item rating ,or a rating from array 
	code_return=None
	if item["Content"]["@type"] == "string":
		content=item["Content"]["#text"]
		content=format_content(content)
		if "@code" not in item["Content"]:
			code=parent_code
		else:
			code=item["Content"]["@code"]
		print condition, type, act, content
#only one element, it is a dict instead of list 
	elif item["Content"]["@type"] == "array" and item["Content"]["Item"].__class__ is dict:
		content=item["Content"]["Item"]["#text"]
		if "@code" not in item["Content"]["Item"]:
			code=parent_code
		else:
			code=item["Content"]["Item"]["@code"]


	elif item["Content"]["@type"] == "array" and item["Content"]["Item"].__class__ is list :
		content_array_flags=1
		print item["Content"]
		for i  in item["Content"]["Item"]:
			if "@code" not in item["Content"]["Item"]:
				code_item=parent_code
			else:
				code_item=item["Content"]["Item"]["@code"]

			if "#text"  in  i.keys():
				org=i["#text"]
				org=format_content(org)
				content_list.append([org,code_item])

#support container, content_list now contains all matched log from container if has. 
#content_list is still [[match1,code] , [match2,code]... ] list 
	elif item["Content"]["@type"] == "container": 

		logger.debug("it is a container  type")
		content_array_flags = 1 
		container_list=cal_container_value(item["Content"]["#text"],process_info)
		logger.debug("the container value is %s" %(container))

#deal code to form content_list in container
		if "@code" not in item["Content"].keys():
			code_item=parent_code
		else:
			code_item=item["Content"]["@code"]
		for g in  container_list:
			content_list.append([g,code_item])
	#print condition, type, act, content
	match_type=type_map[type]
	match_action=action_map[act.split('\\')[1]]
	match_property= act.split('\\')[2]
	logger.debug("in match_rule function, the match information is type %s, action %s, property %s, content_array_flags %d" %(match_type,match_action,match_property,content_array_flags))
	if content_array_flags == 1:
		logger.debug("the content_array value is %s" %(content_list))

# some process do nothing, so no information 
        if "information" not in process_info.keys():
            return [0,0]
	for   i    in  process_info["information"]:
		if  i["type"] ==match_type and i["action"] == match_action :
			logger.debug( "we found  a type/action line %s,will calculate the rule,the match_type is %s, match_action is %s , match_property is %s , content is %s " %(i,match_type,match_action,match_property,content))
			if content_array_flags  == 0:
			#file stuff
				if match_type == "file":
					if condition == "regex":
						if content in i["object1"]:
							matched+=1
	
					if condition == "is":
						if content == i["object1"]:
							matched+=1
			
					if condition == "regex_i":
						if content.upper() in i["object1"].upper():
							matched+=1
	
			
	
			#reg stuff
				if match_type == "registry":
					if condition == "regex":
						if content in i["object1"]:
							matched+=1
	
					if condition == "is":
						if content == i["object1"]:
							matched+=1
					if condition == "regex_i":
						if content.upper() in i["object1"].upper():
							matched+=1
			#process stuff
				if match_type == "process":
					logger.debug("bobo match the process rule,the process path is %s"%(i["object2"]))
					if condition == "regex":
						if content in i["object1"]:
							matched+=1
	
					if condition == "is":
						if content == i["object1"]:
							matched+=1
					if condition == "regex_i":
						logger.debug(content)
						if content.upper() in i["object1"].upper():
							matched+=1
					# it means the content value is other rule value instead of literal text
					if condition ==  "cross_match":
						#find matched items to get the value 
						(type, action, property)=break_search_text(content)				
						value_list=get_value_list(process_info,type, action)
						for   foo  in value_list:
							if i["object2"] == foo:
								matched+=1 
					if condition ==  "cross_match_i":
						(type, action, property)=break_search_text(content)				
						#find matched items to get the value 
						value_list=get_value_list(process_info,type, action)
						for   foo in value_list:
							if i["object2"].upper() == foo.upper():
								matched+=1 
			if content_array_flags  == 1:
			#file stuff
				if match_type == "file":
					for  j  in  content_list:
						if condition == "regex":
							if j[0] in i["object1"]:
								matched+=1
								code=j[1]
					if condition == "is":
						for j  in content_list:
							if j[0] == i["object1"]:
								matched+=1
								code=j[1]
					if condition == "regex_i":
						for j  in content_list:
							print content_list
							if j[0].upper() in i["object1"].upper():
								matched+=1
								code=j[1]
	
			
	
			#reg stuff
				if match_type == "registry":
					if condition == "regex":
						for j in content_list:
							if j[0] in i["object1"]:
								matched+=1
								code=j[1]
	
					if condition == "is":
						for j[0] in content_list:
							if j == i["object1"]:
								code=j[1]
								matched+=1
					if condition == "regex_i":
						for j in content_list:
							if j[0].upper() in i["object1"].upper():
								matched+=1
								code=j[1]
			#process stuff
				if match_type == "process":
					logger.debug("bobo match the process rule,the process path is %s"%(i["object2"]))
					if condition == "regex":
						for j  in content_list:
							if j[0] in i["object1"]:
								matched+=1
								code=j[1]
	
					if condition == "is":
						for j[0]  in content_list:
							if j == i["object1"]:
								code=j[1]
								matched+=1
					if condition == "regex_i":
						for j  in content_list:
							if j[0].upper() in i["object1"].upper():
								matched+=1
								code=j[1]
					# it means the content value is other rule value instead of literal text
					if condition ==  "cross_match":
						#find matched items to get the value 
						(type, action, property)=break_search_text(content)				
						value_list=get_value_list(process_info,type, action)
						for   foo  in value_list:
							if i["object2"] == foo:
								matched+=1 
					if condition ==  "cross_match_i":
						(type, action, property)=break_search_text(content)				
						#find matched items to get the value 
						value_list=get_value_list(process_info,type, action)
						for   foo in value_list:
							if i["object2"].upper() == foo.upper():
								matched+=1 

	if matched == 1:
		logger.debug(" line %s Matchs a rule !!!!!" %(i))
	logger.debug("the match number for this item is %d"%(matched))
	return matched


#replace the window marco 
def  format_content(content):
	logger.debug("the orgin content is %s"%content)
	for i  in  pwd_process.keys():
		content=content.replace(i, pwd_process[i])
	logger.debug("after the replace is %s"%(content))
	return content

#item is a dict of rule, return [ 0|1 , code ] matches this rule
#if code is defined, use it or  inherit from parent 
def match_rule(item, process_info, parent_code):
	matched =0
	condition=item["@condition"]
	type=item["Context"]["@document"]
	act=item["Context"]["@search"]
#support array 
	content_array_flags=0
	content=None
	content_list=[]
# code store this item rating, code_item is for arary item . content_list is [[ a1, code] ,[a2,code]...]
	code=None
	container_list=[]
	code_item=None
#code_return is whether normal item rating ,or a rating from array 
	code_return=None
	if item["Content"]["@type"] == "string":
		content=item["Content"]["#text"]
		content=format_content(content)
		if "@code" not in item["Content"]:
			code=parent_code
		else:
			code=item["Content"]["@code"]
		print condition, type, act, content
#only one element, it is a dict instead of list 
	elif item["Content"]["@type"] == "array" and item["Content"]["Item"].__class__ is dict:
		content=item["Content"]["Item"]["#text"]
		if "@code" not in item["Content"]["Item"]:
			code=parent_code
		else:
			code=item["Content"]["Item"]["@code"]


	elif item["Content"]["@type"] == "array" and item["Content"]["Item"].__class__ is list :
		content_array_flags=1
		print item["Content"]
		for i  in item["Content"]["Item"]:
			if "@code" not in item["Content"]["Item"]:
				code_item=parent_code
			else:
				code_item=item["Content"]["Item"]["@code"]

			if "#text"  in  i.keys():
				org=i["#text"]
				org=format_content(org)
				content_list.append([org,code_item])

#support container, content_list now contains all matched log from container if has. 
#content_list is still [[match1,code] , [match2,code]... ] list 
	elif item["Content"]["@type"] == "container": 

		logger.debug("it is a container  type")
		content_array_flags = 1 
		container_list=cal_container_value(item["Content"]["#text"],process_info)
		logger.debug("the container value is %s" %(container))

#deal code to form content_list in container
		if "@code" not in item["Content"].keys():
			code_item=parent_code
		else:
			code_item=item["Content"]["@code"]
		for g in  container_list:
			content_list.append([g,code_item])
	#print condition, type, act, content
	match_type=type_map[type]
	match_action=action_map[act.split('\\')[1]]
	match_property= act.split('\\')[2]
	logger.debug("in match_rule function, the match information is type %s, action %s, property %s, content_array_flags %d" %(match_type,match_action,match_property,content_array_flags))
	if content_array_flags == 1:
		logger.debug("the content_array value is %s" %(content_list))

# some process do nothing, so no information 
        if "information" not in process_info.keys():
            return [0,0]
	for   i    in  process_info["information"]:
		if  i["type"] ==match_type and i["action"] == match_action :
			logger.debug( "we found  a type/action line %s,will calculate the rule,the match_type is %s, match_action is %s , match_property is %s , content is %s " %(i,match_type,match_action,match_property,content))
			if content_array_flags  == 0:
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
						logger.debug(content)
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
			if content_array_flags  == 1:
			#file stuff
				if match_type == "file":
					for  j  in  content_list:
						if condition == "regex":
							if j[0] in i["object1"]:
								matched=1
								code=j[1]
					if condition == "is":
						for j  in content_list:
							if j[0] == i["object1"]:
								matched=1
								code=j[1]
					if condition == "regex_i":
						for j  in content_list:
							print content_list
							if j[0].upper() in i["object1"].upper():
								matched=1
								code=j[1]
	
			
	
			#reg stuff
				if match_type == "registry":
					if condition == "regex":
						for j in content_list:
							if j[0] in i["object1"]:
								matched=1
								code=j[1]
	
					if condition == "is":
						for j[0] in content_list:
							if j == i["object1"]:
								code=j[1]
								matched=1
					if condition == "regex_i":
						for j in content_list:
							if j[0].upper() in i["object1"].upper():
								matched=1
								code=j[1]
			#process stuff
				if match_type == "process":
					logger.debug("bobo match the process rule,the process path is %s"%(i["object2"]))
					if condition == "regex":
						for j  in content_list:
							if j[0] in i["object1"]:
								matched=1
								code=j[1]
	
					if condition == "is":
						for j[0]  in content_list:
							if j == i["object1"]:
								code=j[1]
								matched=1
					if condition == "regex_i":
						for j  in content_list:
							if j[0].upper() in i["object1"].upper():
								matched=1
								code=j[1]
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

	if matched == 1:
		logger.debug(" line %s Matchs a rule !!!!!" %(i))
	return [matched, code]
#return a list [ 1, 0, 1 , 1... ] each is a return value of ruleN

def cal_indicatoritem(ioc_info,process_info):
	ret=[]
#if the ioc_info is dict, it means that there is only one item 
	if type(ioc_info["IndicatorItem"]) is dict:
		print "ther is only  one item", ioc_info["IndicatorItem"] 
		ret.append(match_rule(ioc_info["IndicatorItem"], process_info,ioc_info["@code"]))
		print "the indicator item list value is %s" %(ret)
		return   ret
# multi items 
	print "there are items ", ioc_info["IndicatorItem"],  type(ioc_info["IndicatorItem"]), len(ioc_info["IndicatorItem"])
	for i in ioc_info["IndicatorItem"]:
		ret.append(match_rule(i,process_info,ioc_info["@code"]))
	print "the indicator item list value is %s" %(ret)
	return ret


def get_process_behavior_list(process_info,behavior_path):
	ret =[]
	name=[]
	code=[]
	logger.debug("reading behavior_path %s to calculate process %s" %(behavior_path,process_info))
	for i  in  get_files_for_dir(behavior_path):
		f=open(i, 'r')
		logger.debug("reading the behavior file: %s"%i)
		j=json.load(f)
		name.append(j["Behaviour"]["@description"])
		logger.debug("dealing with behavior %s " %(j["Behaviour"]["@description"]))
		foo=cal_indicator(j["Behaviour"]["Indicator"], process_info,1)
		if j["Behaviour"]["Indicator"]["@operator"] == "++":
			#ret.append(foo)
			print  "the behavior is %d " %foo[0]
		else:
			print  "the behavior is %d " %foo[0]
	#code value maybe list or value 
			if type(foo[1]) is list:
				print  "the behavior code is %s" %foo[1]
			else:
				print  "the behavior code is %d" %int(foo[1])
		logger.debug("dealing with behavior %s is done"%( j["Behaviour"]["@description"]))
		logger.debug("dealing with behavior %s result: binary value:%s  code value:%s"%( j["Behaviour"]["@description"], foo[0], foo[1]))
		code.append(foo[1])
		ret.append(foo[0])
	print  "behavior name list  %s" %(name)
	print  "behavior result %s"%(ret)
	print  "behavior code  %s"%(code)
	return [name,ret,code]
