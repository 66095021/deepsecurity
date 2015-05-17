#@load base/protocols/http/utils


redef record HTTP::Info += {
    location: string &optional;
};


global redirectionList: table[time] of vector of string &create_expire=3 min;

event http_header(c: connection, is_orig: bool, name: string, value: string){
    if (is_orig){
        return;
    }

    #debug("++http_header " + c$uid + " " + name);
    if (/[Ll][oO][cC][aA][tT][iI][oO][nN]/ in name){
        debug("location is " + value + " " + c$uid);
        c$http$location = value;
    }

}


event http_message_done(c: connection , is_orig: bool , stat: http_message_stat ) &priority = -5
{
    if (is_orig){
        return;
    }

    if (!c?$http){
        return;
    }

    #debug("++http_message_done " + c$uid);
    local url = HTTP::build_url_http(c$http);
    local hitFlg = F;
    for (index in redirectionList){
        local v = redirectionList[index];
        debug(fmt("redirectionList index %d, len of v %d, last=%s,url=%s", index, |v|, v[|v|-1], url));
        if (url == v[|v|-1]){
            hitFlg = T;
            debug("url hit the last one");
            if (!c$http?$location){
                debug("redirection loop end. ");
                for (i in v){
                    debug("  " + v[i]);
                }
                delete redirectionList[index];
            }else{
                v[|v|] = c$http$location;
                for (i in v){
                    debug("  " + v[i]);
                }
            }
        }
    }
    if (!hitFlg && c$http?$location){
        local ve : vector of string;
        ve[0] = url;
        ve[1] = c$http$location;

        local t = current_time();
        debug("add new entry to table");
        redirectionList[t] = ve;
        debug(redirectionList[t][0]);
        debug(redirectionList[t][1]);
    }
} 
