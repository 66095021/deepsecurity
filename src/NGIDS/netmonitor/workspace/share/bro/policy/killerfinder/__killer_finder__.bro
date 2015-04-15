export {

  redef interfaces += "lo";
  redef LogAscii::use_json = T;
 #redef FileExtract::prefix = "/tmp/brofiles/";
  redef Intel::read_files += { "/usr/local/bro/db/example.db",
                              #"/usr/local/bro/db/2_0_dest.db",
                              #"/usr/local/bro/db/2_1_dest.db",
                              #"/usr/local/bro/db/2_4_dest.db",  
                              #"/usr/local/bro/db/2_4_url.db",
                               "/no"};  
 

  global sendrst = "/usr/local/bro/bin/rst";
  global nofileslog = T;
  global debug_enabled = T;
  global line_sep = "\r\n";
  global caredtypes : set[string] = set(
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
  
  
}



@load killerfinder/debug
@load killerfinder/logfile
@load killerfinder/blockkiller


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
