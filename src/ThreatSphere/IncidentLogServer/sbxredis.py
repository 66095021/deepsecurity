#!/usr/bin/env python
#  job queue,  history queue and hash 
#job queue is used for other process which check the queue to send file to capture
#history queue is for recording
#hash  store all detail info , hash key is the ID stored in two queues, info are 
# status, filename, filepath, sha1 of file


#use db 1 , we can change it to any 

import redis
r = redis.Redis(host='127.0.0.1', port=6379, db=1)

def add_job_queue(jobname):
    r.lpush("job",jobname)



def add_history_queue(jobname):
    r.lpush("history",jobname)

# jobname is hash key,  info is a dict  like {"status":**,  "filename":**, "md5":**}
def add_info(jobname,info):
    r.hmset(jobname,info)


def remove_job_queue():
    return r.lpop("job")

def get_info(jobname):
    return r.hgetall(jobname)



