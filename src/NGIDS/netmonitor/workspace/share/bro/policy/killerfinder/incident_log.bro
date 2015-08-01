

redef enum Notice::Type += {
    IP4_MAL,
    IP6_MAL,
    DOMAIN_MAL,
    HOSTNAME1_MAL,
    HOSTNAME2_MAL,
    URL_MAL,
};


type StringIndex: record{
    key: string;
};

type AddrIndex: record {
        ip4: addr;
};

type Val: record {
        val: count;
};

type http_mal_state: enum {
   MAL_NONE,
   IP4_HIT,
   DOMAIN_HIT,
   HOSTNAME1_HIT,
   HOSTNAME2_HIT,
   URL_HIT,  
} &redef;

redef record HTTP::Info += {
    malicous_state : http_mal_state  &log &default = MAL_NONE;
};

global http_ip4_list: table[addr] of Val = table();
global http_domain_list: table[string] of Val = table();
global http_hostname1_list: table[string] of Val = table();
global http_hostname2_list: table[string] of Val = table();


event bro_init()
{
   debug("add table begin");
   

   Input::add_table([$source=httpip4db, 
                      $name="httpip4list", 
                      $idx=AddrIndex, 
                      $val=Val, 
                      $destination=http_ip4_list,
                      $mode=Input::REREAD]);

    Input::add_table([$source=httpdomaindb,
                      $name="httpdomainlist",
                      $idx=StringIndex,
                      $val=Val,
                      $destination=http_domain_list,
                      $mode=Input::REREAD]);

    Input::add_table([$source=httphostname1db,
                      $name="httphostname1list",
                      $idx=StringIndex,
                      $val=Val,
                      $destination=http_hostname1_list,
                      $mode=Input::REREAD]);

    Input::add_table([$source=httphostname2db,
                      $name="httphostname2list",
                      $idx=StringIndex,
                      $val=Val,
                      $destination=http_hostname2_list,
                      $mode=Input::REREAD]);

    Input::remove("httpip4list");
    Input::remove("httpdomainlist");
    Input::remove("httphostname1list");
    Input::remove("httphostname2list");
}

event http_request(c: connection , method: string , original_URI: string , unescaped_URI: string , version: string )
{
    debug(fmt("++http_request, ip %s", c$id$resp_h));
    c$http$malicous_state = MAL_NONE;
    debug(fmt("ip search begin: %s",current_time()));
    if (c$id$resp_h in http_ip4_list){
        debug(fmt("ip search end1: %s",current_time()));
        c$http$malicous_state = IP4_HIT;

        #NOTICE([$note=IP4_MAL,
        #        $msg=fmt("malicious ipv4 %s found", c$id$resp_h),
        #        $conn=c,
        #        $identifier=cat(c$id)]);
    }
    debug(fmt("ip search end2: %s",current_time()));

}


event http_header(c: connection, is_orig: bool, name: string, value: string)
{
    if ( is_orig ){
        switch ( name ) {
        case "HOST":
        #if ( is_valid_ip(value) )
        #else
        {
          local parts = split(value, /\./);
          if (|parts| > 2){
             local domain = parts[|parts|-1] + "." + parts[|parts|];
             if (c$http$malicous_state == MAL_NONE){
                 debug(fmt("domain search begin: %s",current_time()));
                 if (domain in http_domain_list){
                     debug(fmt("domain search end1: %s",current_time()));
                     c$http$malicous_state = DOMAIN_HIT;
                     #NOTICE([$note=DOMAIN_MAL,
                     #                     $msg=fmt("malicious domain %s found", domain),
                     #                     $conn=c,
                     #                     $identifier=cat(c$id)]);                                   
                  }
                  debug(fmt("domain search end2: %s",current_time()));
              }
            }
          }
          if (c$http$malicous_state == MAL_NONE){
              debug(fmt("hostname1 search begin: %s",current_time()));
              if (value in http_hostname1_list){
                  debug(fmt("hostname1 search end1: %s",current_time()));
                  c$http$malicous_state = HOSTNAME1_HIT;
                  #NOTICE([$note=HOSTNAME_MAL,
                  #                        $msg=fmt("malicious hostname %s found", value),
                  #                        $conn=c,
                  #                        $identifier=cat(c$id)]);
              }
              debug(fmt("hostname1 search end2: %s",current_time()));
            }
            if (c$http$malicous_state == MAL_NONE){
                debug(fmt("hostname2 search begin: %s",current_time()));
                if (value in http_hostname2_list){
                    debug(fmt("hostname2 search end1: %s",current_time()));
                    c$http$malicous_state = HOSTNAME2_HIT;
                }
                debug(fmt("hostname2 search end2: %s",current_time()));
            }
            break;

          case "REFERER":
            break;

          case "X-FORWARDED-FOR":
            break;

          case "USER-AGENT":
            break;
          }
       }
}

type SqliteVal: record {
    url: string;
    cat: int;
};

event Input::end_of_data(name: string, source:string)
{
    if ( source == sqlitdb )
        Input::remove(name);
}

event line_url(description: Input::EventDescription, tpe: Input::Event, r: SqliteVal)
{
    debug("url hit " + r$url);
    if (description$name == sqlitdb){
        local ipinfo = description$config["ip"];
        local parts = split(ipinfo, /#/);
        NOTICE([$note=URL_MAL,
                $msg=r$url,
                $sub="malicious url found",
                $src=to_addr(parts[1]),
                $dst=to_addr(parts[2])]);
    }
}

event http_message_done(c: connection, is_orig: bool, stat: http_message_stat)
{
    if ( is_orig && c?$http ){
        local url = HTTP::build_url(c$http);
        switch (c$http$malicous_state){
          case IP4_HIT:
            NOTICE([$note=IP4_MAL,$msg=url, $sub="malicious ipv4 found", $conn=c, $identifier=cat(c$id)]);  
            break;
          case DOMAIN_HIT:
            NOTICE([$note=DOMAIN_MAL, $msg=url, $sub="malicious domain found",$conn=c, $identifier=cat(c$id)]);
            break;      
          case HOSTNAME1_HIT:
            NOTICE([$note=HOSTNAME1_MAL, $msg=url, $sub="malicious hostname found",  $conn=c, $identifier=cat(c$id)]);
            break;
          case HOSTNAME2_HIT:
            debug(fmt("select * from http_url where url='%s';", url));
            Input::add_event(
                    [
                    $source=sqlitdb,
                    $name=sqlitdb,
                    $fields=SqliteVal,
                    $ev=line_url,
                    $want_record=T,
                    $config=table(
                        ["query"] = fmt("select * from http_url where url='%s';", url),
                        ["ip"] = fmt("%s#%s", c$id$orig_h, c$id$resp_h)
                     ),
                    $reader=Input::READER_SQLITE
                    ]);
            break;
        }
    }
}


event Notice::log_notice(rec: Notice::Info){

    if (rec$note == IP4_MAL ||
        rec$note == IP6_MAL ||
        rec$note == DOMAIN_MAL ||
        rec$note == HOSTNAME1_MAL ||
        rec$note == HOSTNAME2_MAL ||
        rec$note == URL_MAL ){
      local src = "unknown";
      if (rec?$src){
        src = fmt("%s",rec$src);
      }

      local dst = "unknown";
      if (rec?$dst){
        dst = fmt("%s",rec$dst);
      }
    
      local url = "unknown";
      if (rec?$msg)
        url = rec$msg;
      if (rec?$sub)
        url += "; " + rec$sub;

      # posterincidenturl.py server_ip src_ip dst_ip url type detector direction
      local cmd = fmt("%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" 0 1", sendincidentlog, incident_log_server, src, dst, url, rec$note);
      debug("cmd : " + cmd);
      system(cmd);
    }
}


