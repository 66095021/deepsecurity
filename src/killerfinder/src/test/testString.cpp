#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>
#include <fstream>
#include <string.h>
#include <vector>
#include <unordered_map>

//HELO <SP> <domain> <CRLF> 
//MAIL <SP> FROM:<reverse-path> <CRLF>
//RCPT <SP> TO:<forward-path> <CRLF>
//DATA <CRLF>
//RSET <CRLF>
//SEND <SP> FROM:<reverse-path> <CRLF>
//SOML <SP> FROM:<reverse-path> <CRLF>
//SAML <SP> FROM:<reverse-path> <CRLF>
//VRFY <SP> <string> <CRLF>
//EXPN <SP> <string> <CRLF>
//HELP [<SP> <string>] <CRLF>
//NOOP <CRLF>
//QUIT <CRLF>
//TURN <CRLF>


using namespace std;

void fun(string& out){
    string a("ss");
    out = a;
    cout<<out<<endl;
}

void split(const string& str, vector<string>& result){
   size_t len = str.size();
   char* buf = (char*)malloc(len+1);
   char* b = buf;
   memcpy(b, str.c_str(), len);
   b[len] = 0;
   char*tmp;
   while((tmp = strsep(&buf, " \t"))!=0){
     if(tmp[0] != 0){
       string s;
       s.append(tmp);
       result.push_back(s);
     }
   }
   free(b);
   return;
}

int main(){
    unordered_map<int, int> mymap;
    mymap[0]=10;
    mymap[2]=12;
    mymap[3]=13;
    mymap[4]=15;
    mymap[1]=11;
    unordered_map<int, int>::iterator it, it_tmp;
    for(it = mymap.begin();
        it!= mymap.end();
        ){
        cout<<" "<< it->first<<endl;
        if (it->second == 12||it->second == 15){
            it_tmp = it;
            ++it;
            mymap.erase(it_tmp);
        }else{
            ++it;
        }
    }

    cout<<" "<<endl;

    for(it = mymap.begin();
        it!= mymap.end();
        ++it){
        cout<<" "<< it->first<<endl;
    }

    /*
   vector<string> result;
   split(" ab cd \t ss   t  ", result);
   for(size_t i=0; i<result.size();i++){
     cout<<result[i]<<":"<<endl;
   }

   ifstream ifs;
   string line;
   ifs.open ("data.txt", ifstream::in);
   while (getline(ifs, line)) {
     cout<<line<<" thomas"<<endl;
   }
   ifs.close();

   string test;
   string base("123456");
   test.clear();
   test.append(base, 3, 2);
   cout<<test<<endl;

   string out;
   fun(out);
   cout<<out<<endl;
    
   string s;
   cout<<s.size()<<endl;
   s.resize(14);
   cout<<s.size()<<endl;

   s.replace(7,2, "789", 2);
   s.replace(0,4, "11111", 4);
   cout<<s.size()<<" "<<s<<endl;
   s.replace(3,2, "2222", 2);
   
   cout<<s.size()<<" "<<s.length()<<" "<<s<<endl;
   const char* b = s.c_str();
   for (size_t i = 0; i < s.size(); i++){
    if(b[i]==0){
        cout<<"empty";
    }else{
        cout<<b[i];
    }
   }
   cout<<endl;

    
   

   string ct = "app/oce";
   cout<<ct.substr(0, ct.find("/"))<<ct.substr(ct.find("/")+1)<<endl;
   ct = "appoce";
   cout<<ct.substr(0, ct.find("/"))<<ct.substr(ct.find("/")+1)<<endl;
    
  

  list<int> li;
  cout<<"size "<<li.size()<<" empty "<<li.empty()<<endl;
    

  string s("thisisatest");
  printf("capacity=%d,length=%d,size=%d,max_size=%x\n",
  s.capacity(),s.length(),s.size(),s.max_size());

  s.reserve(1024);
  printf("capacity=%d,length=%d,size=%d,max_size=%x\n",
  s.capacity(),s.length(),s.size(),s.max_size());

  s.reserve(1024*2);
  printf("capacity=%d,length=%d,size=%d,max_size=%x\n",
  s.capacity(),s.length(),s.size(),s.max_size());
*/
  return 0;

}
