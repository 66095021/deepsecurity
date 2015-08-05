#@load killerfinder/incident_log
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


function cared_type(f:fa_file):bool
{
 
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
}

event file_over_new_connection(f: fa_file , c: connection , is_orig: bool )
{
    debug("+file_over_new_connection "+f$id);

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



