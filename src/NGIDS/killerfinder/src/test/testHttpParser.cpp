#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/http_parser.h"


#define MAX_HEADERS 13
#define MAX_ELEMENT_SIZE 2048

static http_parser *parser;
void printParser(void);
int
message_begin_cb (http_parser *p)
{
  printf("message_begin_cb\n");
  printParser();
  return 0;
}

int
headers_complete_cb (http_parser *p)
{
  printf("headers_complete_cb\n");
  printParser();
  return 0;
}

int
message_complete_cb (http_parser *p)
{
  printf("message_complete_cb\n");
  printParser();
  return 0;
}

int
response_status_cb (http_parser *p, const char *buf, size_t len)
{
  printf("response_status_cb\n");
  printParser();
  char* b = (char*)malloc(len+1);
  strncpy(b, buf, len);
  b[len] = 0;
  printf("%s\n",b);
  return 0;
}

int
request_url_cb (http_parser *p, const char *buf, size_t len)
{
  printf("request_url_cb\n");
  printParser();
  char* b = (char*)malloc(len+1);
  strncpy(b, buf, len);
  b[len] = 0;
  printf("%s\n",b);
  return 0;
}

int
header_field_cb (http_parser *p, const char *buf, size_t len)
{
 
  printf("header_field_cb\n");
  printParser();
  char* b = (char*)malloc(len+1);
  strncpy(b, buf, len);
  b[len] = 0;
  printf("%s\n",b);
  return 0;
}

int
header_value_cb (http_parser *p, const char *buf, size_t len)
{
 
  printf("header_value_cb\n");
  printParser();
  char* b = (char*)malloc(len+1);
  strncpy(b, buf, len);
  b[len] = 0;
  printf("%s\n",b);

  return 0;
}



int
body_cb (http_parser *p, const char *buf, size_t len)
{
  printf("body_cb\n");
  printParser();
  char* b = (char*)malloc(len+1);
  strncpy(b, buf, len);
  b[len] = 0;
  printf("%s\n",b);

  return 0;
}


/*
struct http_parser_settings {
  http_cb      on_message_begin;
  http_data_cb on_url;
  http_data_cb on_status;
  http_data_cb on_header_field;
  http_data_cb on_header_value;
  http_cb      on_headers_complete;
  http_data_cb on_body;
  http_cb      on_message_complete;
};
*/

static http_parser_settings settings =
  {
  message_begin_cb,
  request_url_cb,
  response_status_cb,
  header_field_cb,
  header_value_cb,
  headers_complete_cb,
  body_cb,
  message_complete_cb
  };


void testParser(const char* buf, enum http_parser_type type){
    parser = (struct http_parser*)malloc(sizeof(http_parser));
    http_parser_init(parser, type);

    enum http_errno err;
    size_t nparsed = http_parser_execute(parser, &settings, buf, strlen(buf));
    err = HTTP_PARSER_ERRNO(parser);
    printf("nparsed %d, total %d, err %d\n", nparsed, strlen(buf), err);
    printf("*** %s ***\n\n\n\n", http_errno_description(err));
 
    free(parser);
    parser = NULL;
}


void printParser(){
    printf("type=%d,flags=%d,state=%d,header_state=%d,index=%d\n", parser->type, parser->flags,parser->state,
           parser->header_state,parser->index);
    printf("nread=%d,content_length=%lld\n",parser->nread,parser->content_length);

    printf("http_major=%d,http_minor=%d,status_code=%d,method=%d,http_errno=%d\n",
           parser->http_major,parser->http_minor,parser->status_code,parser->method,parser->http_errno);
    printf("upgrade=%d\n",parser->upgrade);
    printf("data=%s\n\n",parser->data==0?"empty":((char*)parser->data));
}

void testParser1(const char* buf[], int len, enum http_parser_type type){
    parser = (struct http_parser*)malloc(sizeof(http_parser));
    http_parser_init(parser, type);
    
    enum http_errno err;
    for (int i = 0; i < len; i++){
        size_t nparsed = http_parser_execute(parser, &settings, buf[i], strlen(buf[i]));
        err = HTTP_PARSER_ERRNO(parser);
        printf("i %d;total %d, nparsed %d, total %d, err %d\n", i,len, nparsed, strlen(buf[i]), err);
        printf("*** %s ***\n\n", http_errno_description(err));
    }
    printf("\n\n");
    free(parser);
    parser = NULL;
}



int main(){
/*
    const char* buf = 0;
    buf = "GET http://www.baidu.com HTTP/1.1\r\n"
          "aaa: bbbb\r\n"
          "ccc: dddd\r\n"
          "\r\n";
    testParser(buf, HTTP_REQUEST);

    buf = "GET www.baidu.com HTTP/1.1\r\n" //errno is invalid URL
          "aaa: bbbb\r\n"
          "ccc: dddd\r\n"
          "\r\n";
    testParser(buf, HTTP_REQUEST);
    
    buf = "GET http://www.baidu.com HTTP/1.1\n"
          "aaa: bbbb\n"
          "ccc: dddd\n"
          "\n";
    testParser(buf, HTTP_REQUEST);


    buf = "GET / HTTP/1.1\r\n"
          "content-length: 5\r\n"
          "connection: keep-alive\r\n"
          "\r\n";
    testParser(buf, HTTP_REQUEST);
    */

    const char* buflst[]={"GET htt","p://www.bai","du.com/index.html"," ","HTTP/1.1\r\n",
                          "Ho","st",":"," ","www.ba","idu.com\r\n",
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n",
                          "5\r\n12345\r\n3\r\n678\r\n000\r\n\r\nGET ftp://www.baidu.com/ HTTP/1.1\r\n",
                          "Content-Length:5\r\n",
                          "\r\n",
                          "abcde"};

    testParser1(buflst, sizeof(buflst)/sizeof(char*), HTTP_REQUEST);

    return 0;
}


