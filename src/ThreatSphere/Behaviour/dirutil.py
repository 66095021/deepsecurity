#!/usr/bin/env python
import fnmatch
import os
import sys


def get_files_for_dir(dir):
 
    rootPath = '/tmp'
    rootPath = dir
    pattern = '*'
    filelist=[]
    for root, dirs, files in os.walk(rootPath):
        for filename in fnmatch.filter(files, pattern):
            #print(     os.path.join(root, filename))
            filelist.append(os.path.join(root, filename))
    return filelist
        #print(type (os.path.join(root, filename)))


def get_file_size(filename):
    try:
        size=os.path.getsize(filename)
        return size
    except :
	return -1
 
if __name__ =='__main__':
    foo=get_files_for_dir(sys.argv[1])
    print foo
