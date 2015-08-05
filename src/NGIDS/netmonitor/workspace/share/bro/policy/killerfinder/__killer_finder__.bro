@load protocols/conn/weirds.bro


export {
  redef record_all_packets = T;
  redef ignore_checksums = T;


  redef interfaces += "lo";
  redef interfaces += "wlan0";
  redef PacketFilter::default_capture_filter = "tcp or udp";

  redef record HTTP::Info += {
    disposition: string &log &optional;
  };

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
 
  global incident_log_server = "127.0.0.1";
  global httpip4db="/usr/local/bro/db/httpip4.db";
  global httpdomaindb="/usr/local/bro/db/httpdomain.db";
  global httphostname1db="/usr/local/bro/db/httphostname1.db";
  global httphostname2db="/usr/local/bro/db/httphostname2.db";
  global httpurldb="/usr/local/bro/db/httpurl.db";
  global sqlitdb="/usr/local/bro/db/brodb";

  global sendsuspiciousurl = "/usr/local/bro/bin/uploadurl.py";  
  global sendincidentlog = "/usr/local/bro/bin/postIncidentLog.py";
  global sendrst = "/usr/local/bro/bin/rst";
  global nofileslog = T;



  global debug_enabled = T;
  global line_sep = "\\u000a\\u000d";

  
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

  global trustHostList: set[string] = {
     "www.baidu.com",
     "www.sina.com",
     "www.163.com"
  };

  global mailRefererRex = /mail.sina.com.cn/ |
                          /mail.163.com/;



  
}



@load killerfinder/debug
@load killerfinder/logfile
#@load killerfinder/blockkiller
@load killerfinder/incident_log
@load killerfinder/redirect       
@load killerfinder/no_referer
@load killerfinder/vulware
@load killerfinder/mal_smtp_orig
@load killerfinder/detect_mal_file

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
