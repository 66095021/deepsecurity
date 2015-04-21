#!/usr/bin/env python

logfile='/var/ATI/virus-sophos.log'

def log_virus(filename):
    f=open(logfile,'a+')
    f.write('file %s is virus\n' %filename)
    f.close()


def log_sig_ok(filename):
    f=open(log_file,'a+')
    f.write('file %s signature is ok \n' %filename)
    f.close()
    
if __name__=='__main__':
    log_virus(logfile)
    log_sig_ok(logfile)


