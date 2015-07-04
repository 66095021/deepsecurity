* Each pid information with type dict will be stayed in list.
* the input for SVM is pid_info["svm_input"]
* need  pip install beautifulsoup4  tldextract



./extract.py  logdir jsondir


grep "current pid seq" /var/log/behavior.log





logic of reding log



i f sbx log 
   if  new process , log new process info 
   else  disacard sbx log 
else
   
    if  it is  in list, 
             appened the log to it.
    else 
             make a new node, appened the log to node, then insert list 


   if  event is create process:
        if   the new create process is already in list
  		just update info, such as parent, start time
        else 

		make a noew node , appened the log to node, insert it to list 



