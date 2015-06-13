#!/usr/bin/env python



#logging level 
#logging.NOTSET 0
#logging.DEBUG 10
#logging.INFO 20
#logging.WARNING 30 
#logging.ERROR 40 
#logging.CRITICAL 50


import logging  
import logging.handlers  
import json 
import os 
LOG_FILE = '/var/log/sbx.log'  
LOG_FILE_ATI = '/opt/ATI/logs/ATI.log'
LOG_FILE_ATI_DIR='/opt/ATI/logs'
if not os.path.exists(LOG_FILE_ATI_DIR):
    os.system("mkdir %s" %(LOG_FILE_ATI_DIR))
handler = logging.handlers.RotatingFileHandler(LOG_FILE, maxBytes = 1024*1024, backupCount = 5) # ...handler    
handler_ATI = logging.handlers.RotatingFileHandler(LOG_FILE_ATI, maxBytes = 1024*1024, backupCount = 5) # ...handler    
fmt = '%(asctime)s - %(filename)s:%(lineno)s - %(name)s - %(message)s'  
fmt1 = '%(asctime)s---%(message)s'    
formatter = logging.Formatter(fmt)   # ...formatter   
formatter1= logging.Formatter(fmt1)
handler.setFormatter(formatter)      # .handler..formatter   
handler_ATI.setFormatter(formatter1)      

logger = logging.getLogger('sbx')    # ....tst.logger   
logger.addHandler(handler)           # .logger..handler   
logger.setLevel(logging.DEBUG)  


logger1 = logging.getLogger('sbx1')    # ....tst.logger   
logger1.addHandler(handler_ATI)           # .logger..handler   
logger1.setLevel(logging.DEBUG)  

ch = logging.StreamHandler()
logger.addHandler(ch)
ch.setFormatter(formatter)
logger.info('first info message')  
logger.debug('first debug message') 


#test write python info 

d={"a":1 ,"d":2}
x=json.dumps(d)
logger1.addHandler(ch)
#logger1.info('first info message')  
#logger1.debug('first debug message') 
#logger1.debug(x)
