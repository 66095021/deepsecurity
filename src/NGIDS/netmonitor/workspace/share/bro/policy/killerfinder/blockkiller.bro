

event Intel::log_intel(rec: Intel::Info)
{
    debug("+Intel::log_intel " + rec$uid);
    #print network_time();
    #print current_time();

    if (rec?$seen && rec$seen?$indicator_type){ 
      if(rec$seen$indicator_type == Intel::ADDR ||
         rec$seen$indicator_type == Intel::URL ||
         rec$seen$indicator_type == Intel::DOMAIN){
          if (rec?$id){
            local srcip = rec$id$orig_h;
            local srcport = port_to_count(rec$id$orig_p);
            local srcseq = get_orig_seq(rec$id);
        
            local dstip = rec$id$resp_h;
            local dstport = port_to_count(rec$id$resp_p);
            local dstseq = get_resp_seq(rec$id);

            local rstcmd = fmt("%s -i eth0 %s %d %d %s %d %d", sendrst, srcip, srcport, srcseq, dstip, dstport, dstseq);
            debug(rstcmd);
            system(rstcmd);
        }
      }
    }
}


event connection_reset(c: connection ){
    debug("+connection_reset " + c$uid);
    #print network_time();
    #print current_time();
}



