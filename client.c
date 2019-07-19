/*************************************************************************
	> File Name: client.c
	> Author: echo
	> Mail:moho12@163.com 
	> Created Time: 2018年11月27日 16时09分14秒 CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include"endpoint.h"
#include"endpoint_list.h"

static int quiting = 0;
#define RECV_BUFSIZE 1024
#define SEND_BUFSIZE 1024

//存储网络地址
static endpoint_t g_server;
//客服端连接节点
static eplist_t * g_peers;
//socket套接字
static int g_clientfd;

/**
* 打印帮助命令
***/
void print_help()
{
    const static char *help_message = ""
        "Usage:"
        "\n\n login"
        "\n  Login to server"
        "\n\n punch punch:port"
        "\n  punch a hole through UDP to [host:port]"
        "\n  Example:"
        "\n  >>> punch 8.8.8.8:44";
    printf("%s\n",help_message);
}

//发送数据函数
void * receive_loop()
{
    char * operate = NULL;//记录操作
    char Send_Buf[1024] = {0};

    endpoint_t peer;
    socklen_t addrlen;
    char buf[RECV_BUFSIZE];
    int nfds;
    //fd_set是一种数据结构 实际上是一组long类型的数组,每一个数组元素都能打开一个文件句柄建立连接
    fd_set readfds;
    struct timeval timeout;
    nfds = g_clientfd + 1;
    while(!quiting)
    {
        FD_ZERO(&readfds);    //将集合清零
        FD_SET(g_clientfd,&readfds); //将要检测的句柄 加入集合中
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        int ret = select(nfds,&readfds,NULL,NULL,&timeout);
        //printf("%d\n",ret);
        if(ret == 0)
        {
            /**超时**/
            continue;
        }
        else if(ret == -1)
        {
            perror("select");
            continue;
        }
        FD_ISSET(g_clientfd,&readfds);
        //获取peer长度
        addrlen = sizeof(peer);
        memset(&peer,0,addrlen); //分配空间
        memset(buf,0,RECV_BUFSIZE);
        int rd_size = recvfrom(g_clientfd,buf,RECV_BUFSIZE,0,(struct sockaddr *)&peer,&addrlen); 
        printf("recvfrom:%s\n",buf);
        if(rd_size == -1)
        {
            perror("recvfrom");
            continue;
        }
        else if(rd_size == 0)
        {
            continue;
        }
        //解析出具体的操作
        operate = strtok(buf,"@");
        
        //如果消息来自服务器
        if(ep_equal(g_server,peer))
        {
            switch(*operate)
            {
                case '1':
                    {
                        printf("\n\n%s\n\n",ep_tostring(peer));
                        //服务器的回复消息 输出即可
                        printf("%s\n",strtok(NULL,"@"));
                    }
                    break;
                case '2':
                    {
                        char *host_port = strtok(NULL,"@");
                        endpoint_t other = ep_fromstring(host_port);  //定义网络地址结构 
                        //printf("%s",host_port);
                        //如果消息来自服务器是服务器打洞的消息 则此客服端需要想打洞请求方 发送消息 目的是为了让路由器的NAT认为打洞请求方的地址合法
                        strcpy(Send_Buf,"i want get you,brother!");
                        sendto(g_clientfd,Send_Buf,strlen(Send_Buf),0,(struct sockaddr *)&other,sizeof(other));
                        
                    }
                    break;
            }
        }else{
            /*
            strcpy(Send_Buf,"I get you!");
            //如果接受到的是端对端的链接 直接返回信息给对方端口
            sendto(g_clientfd,Send_Buf,strlen(Send_Buf),0,(struct sockaddr *)&peer,sizeof(peer));
            printf("==============%s===================\n",ep_tostring(peer));*/
        }//如果消息来自可客户端
        //打印接收到的消息
    }
    return NULL;
}

/*
 *客服端与服务器进行交互
 * */
 void *console_loop()
 {  
     char Send_Buf[1024]={0};
     char *line = NULL;
     size_t len;
     ssize_t read;
     while(fprintf(stdout,">>>")&&(read = getline(&line,&len,stdin)) != -1)
     {
         if(read == 1)
         {
            continue;
         }
         //解析出命令
         char *cmd = strtok(line," ");
         //printf("%s",cmd);
         if(strncmp(cmd,"login",5) == 0)
         {
             //封装命令数据
             strcpy(Send_Buf,"1@");
             sendto(g_clientfd,Send_Buf,SEND_BUFSIZE,0,(struct sockaddr *)&g_server,sizeof(g_server));
         }else if(strncmp(cmd,"punch",5) == 0){
             //打洞请求 首先获取希望打洞的peer地址
             char *host_port = strtok(NULL,"\n");
            //命令格式错误
             if(host_port == NULL)
             {
                printf("Usage :punch host:port\n");
                continue;
             }
             
             endpoint_t other = ep_fromstring(host_port);    
             //向需要打洞的节点发送消息 发送的消息是空的 仅仅是为了通信让NAT认为对方的网络地址是合法的
             sendto(g_clientfd,Send_Buf,strlen(Send_Buf),0,(struct sockaddr *)&other,sizeof(other));
             //向服务器发送消息 消息的格式是内容+@消息类型
             strcpy(Send_Buf,"2@");
             strcat(Send_Buf,host_port);
             sendto(g_clientfd,Send_Buf,SEND_BUFSIZE,0,(struct sockaddr *)&g_server,sizeof(g_server));
             
         }else if(strncmp(cmd,"list",4) == 0)
         {
             //获取注册在服务器中节点信息
             //封装命令信息
             strcpy(Send_Buf,"3@");
             sendto(g_clientfd,Send_Buf,strlen(Send_Buf),0,(struct sockaddr *)&g_server,sizeof(g_server));
         }else if(strncmp(cmd,"send",4) == 0)
         {
             //获取网络地址
             char *host_port = strtok(NULL,"\n");
             endpoint_t other = ep_fromstring(host_port);
             //点对点发送信息函数
             strcpy(Send_Buf,"I want get you");
             sendto(g_clientfd,Send_Buf,strlen(Send_Buf),0,(struct sockaddr *)&other,sizeof(other));
         }
         else{
             printf("Unknown command%s\n",line);
             print_help();  
         }
     }
     printf("after while");
     free(line);
     return NULL;
 }

int main(int argc,char **argv)
{
    //判断启动参数
    if(argc != 2)
    {
        printf("Usage:%s server:port\n",argv[0]);
        return 1;
    }
    
    int ret;
    //创建线程
    pthread_t receive_pid,console_pid;
    //对参数进行初始化
    g_server = ep_fromstring(argv[1]);
    //创建客服端链
    g_peers = eplist_create();
    
    g_clientfd = socket(AF_INET,SOCK_DGRAM,0);
    //套接字失败 
    if(g_clientfd == -1)
    {
        perror("socket");
        goto clean;
    }
    //创建工作线程
    ret = pthread_create(&receive_pid,NULL,&receive_loop,NULL);
    if(ret != 0)
    {
        perror("receive");
        goto clean;
    }
    ret = pthread_create(&console_pid,NULL,&console_loop,NULL);
    if(ret != 0)
    {
        perror("console");
        goto clean;
    }
    pthread_join(console_pid,NULL);
    printf("pthread_join");
    pthread_join(receive_pid,NULL);
clean:
    close(g_clientfd); //关闭套接字
    eplist_destroy(g_peers);
    return 0;
}
