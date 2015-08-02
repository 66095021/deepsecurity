@load killerfinder/incident_log
export{

    global extract_name_from_uri: function (uri : string) : string; 

}

function extract_name_from_uri(uri : string) : string
{
    local name = "noname";

    # http://www.baidu.com
    local i = strstr(uri, "://") ;
    if (i != 0){
      uri = sub_bytes(uri, i+3, |uri|-i-2);
    }


    if (strstr(uri, "/") == 0){
        return name;
    }

    local paths = split(uri, /\//);
    local tmp = paths[|paths|];
    if (tmp == ""){
       return name;
    }else{
      local r = split1(tmp, /[\?#]/)[1];
      if (r == ""){
        return name;
      }else{
        return r;
      }
    }
}


######################################
#
# add current_header to http::info, but
# not log to http.log
#
######################################

type httphead: record {
    cnid   : conn_id;
   #uri    : string;
    req_h  : string;
    res_h  : string;
}; 

redef record HTTP::Info += {
    current_header : httphead  &optional;
};

event http_request(c: connection , method: string , original_URI: string , unescaped_URI: string , version: string )
{
    debug("+http_request " + c$uid);
    
    if ( !c$http?$current_header ){
        c$http$current_header = httphead($cnid=c$id,
                                        #$uri=original_URI,
                                         $req_h="",
                                         $res_h="");
    }else{
       #c$http$current_header$uri = original_URI;
        c$http$current_header$req_h="";
        c$http$current_header$res_h="";
    }
    local content = fmt("%s %s HTTP/%s", method, original_URI, version) + line_sep;
    c$http$current_header$req_h += content;
}

event http_reply(c: connection , version: string , code: count , reason: string )
{
    debug("+http_reply " + c$uid);
    if ( c$http?$current_header ){
        c$http$current_header$res_h= fmt("HTTP/%s %d %s",version,code,reason)+line_sep;
    }

}


event http_all_headers(c: connection , is_orig: bool , hlist: mime_header_list )
{
    debug("+http_all_headers " + c$uid);
    if (c$http?$current_header){
        if(is_orig){
            for (index in hlist){
                c$http$current_header$req_h += 
                   fmt("%s: %s",hlist[index]$name, hlist[index]$value) + line_sep;
            }
            c$http$current_header$req_h += line_sep;
        }else{
            for (index in hlist){
                c$http$current_header$res_h +=
                    fmt("%s: %s",hlist[index]$name, hlist[index]$value) + line_sep;
            }
            c$http$current_header$res_h += line_sep;
        }
    }
}


######################################
#
# add desc and details to files.log
#
######################################


redef record Files::Info += {
    desc    : string &log &optional;
    details : vector of string &log &optional;
};


#global mime_to_ext: table[string] of string = {
#	["application/x-dosexec"] = "exe",
#	["text/plain"] = "txt",
#	["image/jpeg"] = "jpg",
#	["image/png"] = "png",
#	["text/html"] = "html",
#};


function cared_type(f:fa_file):bool
{
    if (f$info$malicious_state != MAL_NONE && f$info$malicious_state != HOSTNAME2_HIT){
        return T;
    }
 
    if ( ! f?$mime_type ){
        return T;
    }
    
    for(t in file_caredtypes){
        if ( 0 != strstr(f$mime_type, t)){
            return T;
        }
    }
    return F;

}
event intel_data(f: fa_file, data: string)
{
    #debug("+intel_data " + f$id);
}

event file_new(f:fa_file)
{
    debug("+file_new "+f$id);
    
    if ( ! cared_type(f) ){
        return;
    }
	
    # check protocol
    if ( f?$source && f$source ! in file_caredprotocols){
        debug(f$id + " not in cared protocols: " + f$source);
        return;
    }	
	
    # check src ip
    for (id in f$conns){
        if (id$orig_h in file_unwanted_src_ips){
            debug(f$id + " in unwanted ip list");
            return;
        }
    }
    

    local fname = f$id;
    
    if ( f?$mime_type){
        local i = strstr(f$mime_type, "/");
        if (i != 0 && i != 1){
            fname += ("."+ sub_bytes(f$mime_type, 0, i-1));
        }else{
            fname += ".unknown";
        }
      }else{
        fname += ".unknown";
    }

    debug("create a file " + fname);
    Files::add_analyzer(f, Files::ANALYZER_EXTRACT, [$extract_filename=fname]);    
    #Files::add_analyzer(f, Files::ANALYZER_DATA_EVENT, [$stream_event=intel_data]);
}



event file_extraction_limit(f: fa_file, args: any, limit: count, offset: count, len: count)
{
    debug("+file_extraction_limit " + f$id);
    FileExtract::set_limit(f, args, limit+len);
}

event file_state_remove(f: fa_file)
{
    debug("+file_state_remove "+f$id + " " + Files::describe(f));
    if (!cared_type(f)){
        return;
    }
    f$info$desc = Files::describe(f);

    if ( f$source == "HTTP" && ! f$info?$filename ){
        f$info$filename = extract_name_from_uri(f$info$desc);
    }
}

event file_over_new_connection(f: fa_file , c: connection , is_orig: bool )
{
    debug("+file_over_new_connection "+f$id);

    if (!cared_type(f)){
        return;
    }

    
    if (c?$http && c$http?$current_header){
      local content = (fmt("%s", c$http$current_header$cnid) +
                                    c$http$current_header$req_h + c$http$current_header$res_h);
      if (!f$info?$details){
        f$info$details = vector(content);
      } else {
        f$info$details[|f$info$details|] = content; 
      }
    }
}

event file_timeout(f: fa_file )
{
    debug("+file_timeout "+f$id);
}

event file_gap(f: fa_file , offset: count , len: count )
{
    debug(fmt("+file_gap %s off %d len %d", f$id, offset, len));
}


event tcp_packet(c: connection , is_orig: bool , flags: string , seq: count , ack: count , len: count , payload: string )
{
    #if (!is_orig){
        #debug(fmt("+tcp_packet %s seq %d, ack %d, len %d ",c$uid, seq, ack, len));
    #}
}


######################################
#
# log filter. rewrite files.log
#
######################################

function is_file_cared(rec: Files::Info):bool
{

    local hs = "";
	if (rec?$tx_hosts){
        for (h in rec$tx_hosts){
            hs += fmt("%s ", h);
        }
	}
	
	local m = "";
	if (rec?$mime_type){
		m = rec$mime_type;
	}
    debug(rec$fuid + " mime " + m + " source " + rec$source + " ip " + hs);
   
    # check mime type
    if ( ! rec?$mime_type ){
        debug(rec$fuid + " no mime type, ignore it");
        return F;
    }
    
    local caredtype = F;
    for(t in file_caredtypes){
        if ( 0 != strstr(rec$mime_type, t) ){
            caredtype = T;
        }
    }
    if (!caredtype){
        debug(rec$fuid + "not in cared type");
        return F;
    }

    # check protocol
    if (rec?$source){
        debug(rec$fuid + " no source, ignore it");
        return F;
    } else if (rec$source ! in file_caredprotocols){
        debug(rec$fuid + " not in cared protocols");
        return F;
    }

    # check src ip
    if (rec?$tx_hosts){
        for (sender in rec$tx_hosts){
            if (sender in file_unwanted_src_ips){
                return F;
            }
        }
    } else {
        debug(rec$fuid + " no src ip, ignore it");
        return F;
    }

    return T;
}

function split_log(id: Log::ID, path: string, rec: Files::Info) : string
{
    return fmt("files-%s", rec$source);
}

function log_cared_file(rec: Files::Info): bool
{
    return T;
    #return is_file_cared(rec);
}

event bro_init()
{

    if (nofileslog){
        Log::remove_filter(Files::LOG, "default");
    }

    local filter: Log::Filter = [$name="fileinfo",
                                 $path="fileinfo",
                                 $path_func=split_log,
                                 $pred=log_cared_file,
                                 $include=set("ts",
                                              "fuid",
                                              "conn_uids",
                                              "source",
                                              "mime_type",
                                              "filename",
                                              "desc",
                                              "details")];
    Log::add_filter(Files::LOG, filter);
}







