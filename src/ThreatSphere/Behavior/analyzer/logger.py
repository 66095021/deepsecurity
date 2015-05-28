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
  
LOG_FILE = '/var/log/behavior.log'  
  
handler = logging.handlers.RotatingFileHandler(LOG_FILE, maxBytes = 1024*1024, backupCount = 5) # ...handler    
fmt = '%(asctime)s - %(filename)s:%(lineno)s - %(name)s - %(message)s'  
  
formatter = logging.Formatter(fmt)   # ...formatter   
handler.setFormatter(formatter)      # .handler..formatter   
  
logger = logging.getLogger('behavior')    # ....tst.logger   
logger.addHandler(handler)           # .logger..handler   
logger.setLevel(logging.DEBUG)  



ch = logging.StreamHandler()
logger.addHandler(ch)
ch.setFormatter(formatter)
logger.info('first info message')  
logger.debug('first debug message') 
