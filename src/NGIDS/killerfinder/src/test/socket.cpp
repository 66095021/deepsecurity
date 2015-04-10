#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>





#include <fstream>
#include <iostream>
using namespace std;

const char* sendstr[] = {
    "GET ww","w.b","aidu.com:80","80/path/in","dex.jsp?att1=val","ue1&att2=value2#frag=1 HTTP/1.1\r\n",
    "Cache-Control: no-cache\r\n",
    "H","os","t: ","www.b","aidu.com\r\n",
    "Via: wcg\r\n",
    "\r\n",
    "GET path2/index2.jsp?a1=value1&a2=value2#frag=2 HTTP/1.1\r\n",
    "Cache-Control: no-cache\r\n",
    "Hos","t:"," ","ww","w.","google.com\r\n",
    "Via: wcg\r\n",
    "Content-Length: 5\r\n\r\n",
    "12345"
};

int main(int argc, char** argv){

  int s = socket(AF_INET, SOCK_STREAM, 0);
  if(-1 == s)
  {
    cout<<"socket fail !"<<endl;
    return -1;
  }

  struct sockaddr_in s_add;
  bzero(&s_add,sizeof(struct sockaddr_in));
  s_add.sin_family=AF_INET;
  s_add.sin_addr.s_addr= inet_addr("192.168.0.104");
  s_add.sin_port=htons(8080);
  printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);

  if(-1 == connect(s,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
  {
    printf("connect fail !\r\n");
    return -1;
  }

  for (unsigned i = 0; i < sizeof(sendstr)/sizeof(char*); i++){
    printf("%s\n", sendstr[i]);
    write(s, sendstr[i], strlen(sendstr[i]));
  }
  
/*
  ifstream file;
  file.open("./socket.request");

  char c;
  while (file.peek() != EOF){
    c = file.get();
    cout<<c;
    write(s, &c, 1);
  }
  cout<<endl;

  file.close();
*/
  close(s);
  



}
