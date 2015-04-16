#!/usr/bin/env python
import magic
import sys

#can not use WCG_EXE because WCG_EXE contains the linux exe also
def is_pe(filemagic):
    if filemagic.find("PE32") != -1:
        return 1
    else: 
        return 0
def is_pdf(filemagic):
    if filemagic.find("WCG_PDF") != -1:
        return 1
    else:
        return 0 
def is_doc(filemagic):
    if filemagic.find("WCG_DOC") != -1:
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
    ms=magic.open(magic.MAGIC_NONE)
    ms.load("/usr/share/misc/all.mgc")
    return ms.file(filename)

if __name__== '__main__':
    
    ret=getfiletype(sys.argv[1])
    print ret
    pe=is_pe(ret)
    compression=is_compression(ret)
    pdf=is_pdf(ret)
    doc=is_doc(ret)
    print "PE" ,pe , "compression",compression, "PDF",pdf, "doc",doc

