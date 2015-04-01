#!/usr/bin/env python
import magic
import sys


def is_pe(filemagic):
    if filemagic.find("PE32") != -1:
        return 1
    else: 
        return 0
def is_pdf(filemagic):
    if filemagic.find("PDF") != -1:
        return 1
    else:
        return 0 
def is_compression(filemagic):
    #tar.bz2
    if filemagic.find("bzip2 compressed data") != -1:
        return 1
    #tar.gz
    if filemagic.find("gzip compressed data")  != -1:
        return 1      
    # zip
    if  filemagic.find("Zip archive data")  != -1:
        return  1
    # tar 
    if  filemagic.find("POSIX tar archive")  != -1:
        return  1
    return  0
def getfiletype(filename):
    magic.Magic(magic_file='/tmp/Magdir/')
    return magic.from_file(filename)

if __name__== '__main__':
    
    ret=getfiletype(sys.argv[1])
    print ret
    v=is_pe(ret)
    v=is_compression(ret)
    v=is_pdf(ret)
    print v

