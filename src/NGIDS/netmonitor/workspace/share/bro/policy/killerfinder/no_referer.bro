redef enum Notice::Type += {
    NO_REFERER,
    URL_FROM_EMAIL,
};

global trustHostList: set[string] = {
     "www.baidu.com",
     "www.sina.com",
     "www.163.com"
};

global mailRefererRex = /mail.sina.com.cn/ |
                        /mail.163.com/;


event http_message_done(c: connection , is_orig: bool , stat: http_message_stat )
{
    if (!is_orig){
        return;
    }

    #debug("+http_message_done "+ c$uid);
    if (c?$http){
        local url = HTTP::build_url_http(c$http); 
        #local cmd = fmt("%s \"%s\" \"%s\" ", sendsuspiciousurl, incident_log_server, url);

        if (!c$http?$referrer || c$http$referrer == ""){
            if (c$http?$host && c$http$host in trustHostList){
                debug("trust host " + c$uid);
            }else{
                #debug("no referrer " + c$uid + " " + cmd);
                #system(cmd);
                NOTICE([$note=NO_REFERER,$msg=url, $sub="http request without referer", $conn=c, $identifier=cat(c$id)]);
            }
            return;
        }

        if (mailRefererRex in c$http$referrer){
            #debug("referrer is " + c$http$referrer + " " + c$uid + " " + cmd);
            #system(cmd); 
            NOTICE([$note=URL_FROM_EMAIL,$msg=url, $sub="http request from email", $conn=c, $identifier=cat(c$id)]);
        }

    }
}



event Notice::log_notice(rec: Notice::Info){

    if (rec$note ==  NO_REFERER || rec$note == URL_FROM_EMAIL){
        local cmd = fmt("%s \"%s\" \"%s\" ", sendsuspiciousurl, incident_log_server, rec$msg);
        debug(cmd);
        system(cmd);
    }
}
