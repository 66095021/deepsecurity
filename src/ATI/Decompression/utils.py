#!/usr/bin/env python

logfile='/tmp/monitor.log'

def log_virus(filename):
    f=open(filename,'a+')
    f.write('file %s is virus\n' %filename)
    f.close()


def log_sig_ok(filename):
    f=open(filename,'a+')
    f.write('file %s signature is ok \n' %filename)
    f.close()
    
if __name__=='__main__':
    log_virus(logfile)
    log_sig_ok(logfile)


