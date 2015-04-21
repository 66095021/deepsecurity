@load protocols/conn/weirds.bro


export {
  redef record_all_packets = T;
  redef ignore_checksums = T;


  redef interfaces += "lo";
  redef LogAscii::use_json = T;
 #redef FileExtract::prefix = "/tmp/brofiles/";
 #redef Intel::read_files += { 
                               #"/usr/local/bro/db/2_0_dest.db",
                               #"/usr/local/bro/db/2_1_dest.db",
                               #"/usr/local/bro/db/2_4_dest.db",  
                               #"/usr/local/bro/db/2_4_url_hn.db",
                               #"/usr/local/bro/db/2_4_url.db",
                               #"/usr/local/bro/db/example.db"
  #                            };  
 
  global httpip4db="/usr/local/bro/db/httpip4.db";
  global httpdomaindb="/usr/local/bro/db/httpdomain.db";
  global httphostname1db="/usr/local/bro/db/httphostname1.db";
  global httphostname2db="/usr/local/bro/db/httphostname2.db";
  global sqlitdb="/usr/local/bro/db/brodb";
  
  global sendrst = "/usr/local/bro/bin/rst";
  global nofileslog = T;
  global debug_enabled = T;
  global line_sep = "\r\n";
  
  global file_caredprotocols : set[string] = set(
                                                "HTTP",
                                                "SMTP",
                                                "FTP"
                                                );
  global file_caredtypes : set[string] = set(
                          "application", 
                         #"audio",
                          "example", 
                         #"image", 
                          "message", 
                          "model", 
                          "multipart", 
                         #"text", 
                         #"video",
                          "end"
                         ); 

  global file_unwanted_src_ips : set[addr] = set(
                                          10.228.34.168
                                          ); 
  
}



@load killerfinder/debug
@load killerfinder/logfile
#@load killerfinder/blockkiller
@load killerfinder/incident_log


event bro_init()
{
    #Log::disable_stream(Conn::LOG);
    #Log::disable_stream(Files::LOG);
    #Log::disable_stream(HTTP::LOG);
    #Log::disable_stream(Weird::LOG);
    #Log::disable_stream(FTP::LOG);
    #Log::disable_stream(SSL::LOG);
    #Log::disable_stream(X509::LOG);
    #Log::disable_stream(DNS::LOG);
    #Log::disable_stream(Communication::LOG);
}
