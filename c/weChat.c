#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <json-c/json.h>

#define DEBUG 0

struct Response {
        char *buffer;
        size_t size;
};

static size_t WriteResponseCallback(void *contents, size_t size, size_t nmemb, void *userp);
char *ParseToken(char *response, size_t size);
char *ParseMsgAns(char *response,size_t size);

// compile:  gcc weChat.c -o weChat -Wall -lcurl -ljson-c
// json-c: apt install libjson-c-dev
// demo: https://gist.github.com/alan-mushi/19546a0e2c6bd4e059fd
//       https://curl.se/libcurl/c/example.html
// 通过调用微信接口， 演示使用  libcurl,  libjson-c 两个基本库的使用
 
const char CorpID[]="wx7fb===============";  // 微信公众号 ID
const char AgentID[]="0";          // 需要发送的子应用编号
const char Secret[]="dQnJrUS*****************************************";  // 密码

static size_t  WriteResponseCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
        size_t realsize = size * nmemb;
        struct Response *mem = (struct Response *)userp;

        mem->buffer = realloc(mem->buffer, mem->size + realsize + 1);
        if(mem->buffer == NULL) { /* out of memory */
                fprintf(stderr, "%s %d not enough memory (realloc returned NULL)\n",__FILE__,__LINE__);
                return 0;
        }

        memcpy(&(mem->buffer[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->buffer[mem->size] = 0;

        return realsize;
}

char *ParseToken(char *response,size_t size)
{ // parse output first, and return token
        struct json_object *parsed_json ;
        struct json_object *errcode;
        struct json_object *errmsg;
        struct json_object *access_token;
        struct json_object *expires_in;
        char *token;

        fprintf(stdout,"==== parse json output ====\n");
        parsed_json = json_tokener_parse(response);
        json_object_object_get_ex(parsed_json, "errocde",       &errcode);
        json_object_object_get_ex(parsed_json, "errmsg",        &errmsg);
        json_object_object_get_ex(parsed_json, "access_token",  &access_token);
        json_object_object_get_ex(parsed_json, "expires_in",    &expires_in);
        if ( json_object_get_int(errcode) != 0 ) {
                fprintf(stderr,"%s %d: GetToken() get error return, code=%d, msg=%s\n",__FILE__,__LINE__,
                   json_object_get_int(errcode), json_object_get_string(errmsg));
                return (NULL) ;
        }

        token=malloc( size );
        strcpy (token,json_object_get_string(access_token));

        FILE *fp;
    time_t t;
    t = time(NULL);
        fp=fopen("/tmp/weChatToken.dat","w");
        fprintf(fp,"%lu\t%d\t%s\n",t,json_object_get_int(expires_in), token);
        fclose(fp);

        json_object_put(parsed_json);  // delete json Object
        return (token);
}

char *ParseMsgAns(char *response,size_t size)
{ //  从服务器的返回信息中， 分析消息是否发送成功， 若成功， 返回消息 id, 否则返回NULL
  // {"errcode":0,"errmsg":"ok","msgid":"fcLc6UhB2absSaoEDgOVFA-5Uf_qT6z7_tzRh_HZduZNoy53p2zfUB9OVetb-nmTD9hR7PL6O46GV1FVItMMPQ"}

        struct json_object *parsed_json ;
        struct json_object *errcode;
        struct json_object *errmsg;
        struct json_object *msginfo;
        char *MsgID;

        parsed_json = json_tokener_parse(response);
        json_object_object_get_ex(parsed_json, "errocde",  &errcode);
        json_object_object_get_ex(parsed_json, "errmsg",   &errmsg);
        json_object_object_get_ex(parsed_json, "msgid",    &msginfo);
        if ( json_object_get_int(errcode) != 0 ) {
                fprintf(stderr,"%s %d: SendMessage and server return error, code=%d, msg=%s\n",__FILE__,__LINE__,
                   json_object_get_int(errcode), json_object_get_string(errmsg));
                return (NULL) ;
        }
        MsgID=malloc( size );
        strcpy (MsgID,json_object_get_string(msginfo));
        json_object_put(parsed_json);
        return(MsgID);
}

 
int main(int argc, char *argv[])
{
        CURL *curl;
        CURLcode res;
        char *ct=NULL ;
        struct Response chunk;
        struct json_object *jobj, *jobj2;
        char *token, *msgid;
        char *postData;
        char SendText[2048];


        // init message to send ...
        if ( argc<2 ) { 
                fprintf(stderr, "Usage: %s message_to_send \n",argv[0]);
                exit(1);
        }

        strcpy(SendText,argv[1]);

        // init chunk to no data
        chunk.buffer=malloc(1); // will be realloc by function WriteResponseCallback()
        chunk.size=0;

        char *request;

        request=malloc(512); 
        curl_global_init(CURL_GLOBAL_ALL);
        curl=curl_easy_init();
        if ( !curl ) {
                fprintf(stderr,"Can't initial curl!\n");
                curl_easy_cleanup(curl);
                exit(1);
        } 
        sprintf(request,"https://qyapi.weixin.qq.com/cgi-bin/gettoken?corpid=%s&corpsecret=%s",CorpID,Secret);
        curl_easy_setopt(curl,CURLOPT_URL, request);
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, WriteResponseCallback ); // 回写
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void *)&chunk);
        curl_easy_setopt(curl,CURLOPT_USERAGENT,"libcurl-agent/1.0");
        curl_easy_setopt(curl,CURLOPT_HTTPGET, 1L);

        res = curl_easy_perform(curl);
        if ( res == CURLE_OK ) {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                if (DEBUG) fprintf(stdout,"response = %lu\n",response_code);

                res  = curl_easy_getinfo(curl,  CURLINFO_CONTENT_TYPE, &ct);
                if (( res== CURLE_OK) && ct) {
                        fprintf(stdout,"response type: [%s]\n",ct);
                        fprintf(stdout,"---- payplod = %lu bytes---\n",(long)chunk.size);
                        fprintf(stdout,"%s",chunk.buffer);
                        fprintf(stdout,"\n---- end of response ----\n");
                } else {
                        fprintf(stderr,"%d\n",res);
                        exit(1);
                }
        }

        token=ParseToken(chunk.buffer,chunk.size);

        jobj = json_object_new_object();
        json_object_object_add(jobj,"touser",json_object_new_string("spender"));
        json_object_object_add(jobj,"toparty",json_object_new_int(3));
        json_object_object_add(jobj,"msgtype",json_object_new_string("text"));
        json_object_object_add(jobj,"agentid",json_object_new_string(AgentID));
        jobj2 = json_object_new_object();  // layer 2 object
        json_object_object_add(jobj2,"content",json_object_new_string(SendText));
        json_object_object_add(jobj,"text",jobj2);

        // Print Debug information
        if ( DEBUG ) {
                fprintf(stdout,"Toekn=%s\n",token);
                fprintf(stdout,"==== Post Json ===============\n%s\n",json_object_to_json_string(jobj));
        }

        curl_easy_reset(curl);
        chunk.buffer= NULL;   // reset chunk data 
        chunk.size=0;
        ct=NULL;

        struct curl_slist *headers = NULL; // header 

        postData= malloc( strlen(json_object_to_json_string(jobj))+1);
        sprintf(postData,"%s",json_object_to_json_string(jobj) );

        json_object_put(jobj);  // delete json Object

        headers = curl_slist_append(headers,"Accept: application/json");
        headers = curl_slist_append(headers,"Content-Type: application/json");
        headers = curl_slist_append(headers,"Charset: utf-8");

        sprintf(request,"https://qyapi.weixin.qq.com/cgi-bin/message/send?access_token=%s",token);

        if ( DEBUG ) fprintf(stdout,"D: request=%s\n",request);
        if ( DEBUG ) fprintf(stdout,"D: postData=%s\n",postData);
        fprintf(stdout,"Post data to qyapi.weixin.qq.com...\n");

        curl_easy_setopt(curl,CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl,CURLOPT_URL, request);
        curl_easy_setopt(curl,CURLOPT_POST, 1L); 
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS , postData);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE , strlen(postData));
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, WriteResponseCallback ); // 回写
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void *)&chunk);

        res=curl_easy_perform(curl);

        res  = curl_easy_getinfo(curl,  CURLINFO_CONTENT_TYPE, &ct);

        if (( res== CURLE_OK) && ct) {
                fprintf(stdout,"response type: [%s]\n",ct);
                fprintf(stdout,"---- payplod = %lu bytes---\n",(long)chunk.size);
                fprintf(stdout,"%s",chunk.buffer);
                fprintf(stdout,"\n---- end of response ----\n");

                msgid=ParseMsgAns( chunk.buffer, chunk.size );
                if (  msgid != NULL) fprintf(stdout, "Send message success, msgid=%s\n", msgid );
        } else {
                fprintf(stdout,"curl_easy_getinfo() failed: %s\n",curl_easy_strerror(res) );
        }

        free(token);
        free(msgid);
        free(postData);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl); 
        free(chunk.buffer);
        curl_global_cleanup();
        return(0);
}
