global outfile: file;
export {
    global debug: function(content:string);
}

function debug(content:string)
{
    if (!debug_enabled){
        return;
    }
    if(!active_file(outfile)){  
        outfile = open("./thomas_debug.out");
        print "open debug";
    }
    local s = fmt("[%s] %d %s", strftime("%Y-%m-%d %H:%M:%S", current_time()), getpid(), content);
    print s;
    write_file(outfile, s+"\n");  
    flush_all(); 
}


event bro_init()
{
  print "+bro_init";
  if (!mkdir(FileExtract::prefix)){
    print "mkdir failed " + FileExtract::prefix;
  }else{
    print "mkdir seccessfully " + FileExtract::prefix;
    outfile = open("./thomas_debug.out");
  }
}

