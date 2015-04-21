

redef enum Notice::Type += {
    IP4_MAL,
    IP6_MAL,
    DOMAIN_MAL,
    HOSTNAME_MAL,
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

redef record HTTP::Info += {
# 0: init   1: hit  2: need url search
    malicous_state : count  &default = 0;
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
    c$http$malicous_state = 0;
    debug(fmt("ip search begin: %s",current_time()));
    if (c$id$resp_h in http_ip4_list){
        debug(fmt("ip search end1: %s",current_time()));
        c$http$malicous_state = 1;
        NOTICE([$note=IP4_MAL,
                $msg=fmt("malicious ipv4 %s found", c$id$resp_h),
                $conn=c,
                $identifier=cat(c$id)]);
    }
    debug(fmt("ip search end2: %s",current_time()));

}


event http_header(c: connection, is_orig: bool, name: string, value: string)
	{
	if ( is_orig )
		{
		switch ( name ) 
			{
			case "HOST":
			#if ( is_valid_ip(value) )
			#else
			{
                          local parts = split(value, /\./);
                          if (|parts| > 2){
                              local domain = parts[|parts|-1] + "." + parts[|parts|];
                              if (c$http$malicous_state == 0){
                                if (domain in http_domain_list){
                                  c$http$malicous_state = 1;
                                  NOTICE([$note=DOMAIN_MAL,
                                          $msg=fmt("malicious domain %s found", domain),
                                          $conn=c,
                                          $identifier=cat(c$id)]);                                   
                                }
                              }
                          }
                        }
                        if (c$http$malicous_state == 0){
                            debug(fmt("hostname1 search begin: %s",current_time()));
                            if (value in http_hostname1_list){
                                debug(fmt("hostname1 search end1: %s",current_time()));
                                c$http$malicous_state = 1;
                                NOTICE([$note=HOSTNAME_MAL,
                                          $msg=fmt("malicious hostname %s found", value),
                                          $conn=c,
                                          $identifier=cat(c$id)]);
                            }
                            debug(fmt("hostname1 search end2: %s",current_time()));
                        }
                        if (c$http$malicous_state == 0){
                            debug(fmt("hostname2 search begin: %s",current_time()));
                            if (value in http_hostname2_list){
                                debug(fmt("hostname2 search end1: %s",current_time()));
                                c$http$malicous_state = 2;
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
    NOTICE([$note=URL_MAL,
            $msg=fmt("malicious url %s", r$url)]);
}

event http_message_done(c: connection, is_orig: bool, stat: http_message_stat)
{
    if ( is_orig && c?$http ){
        if (c$http$malicous_state == 2){
            local url = HTTP::build_url(c$http);
            debug(fmt("select * from http_url where url='%s';", url));
            Input::add_event(
                    [
                    $source=sqlitdb,
                    $name=sqlitdb,
                    $fields=SqliteVal,
                    $ev=line_url,
                    $want_record=T,
                    $config=table(
                        ["query"] = fmt("select * from http_url where url='%s';", url)
                     ),
                    $reader=Input::READER_SQLITE
                    ]);
        }
    }
}
