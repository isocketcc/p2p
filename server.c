/*************************************************************************
	> File Name: server.c
	> Author: echo
	> Mail:moho12@163.com 
	> Created Time: 2018年11月26日 20时37分43秒 CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include"endpoint.h"
#include"endpoint_list.h"
#define RECV_BUFSIZE 1024
#define SEND_BUFSIZE 1024 
//定义链表存储用户
eplist_t *g_client_pool;

//信息接收函数 注意此函数有typedef修饰,主要原因是此处的声明只是仅仅作为函数的参数而已 有点函数指针的意思
/**
 * 接受客服端信息 传入的参数是创建的socket套接字
 * */
 void udp_receive_loop(int listen_sock)
 {
     //记录客服端想要操作的类型
     char *operate;
     //客服端节点信息
     endpoint_t peer;
     //获取客服端地址的长度
     socklen_t addrlen;
    
     char Recv_Buf[RECV_BUFSIZE];//接收缓存
     char Send_Buf[SEND_BUFSIZE]={0};//发送缓存
     for(;;)
     {
         addrlen = sizeof(peer);
         memset(&peer,0,addrlen);
         memset(Recv_Buf,0,RECV_BUFSIZE);
         int rd_size;
         printf("server start\n");
         //接受客服端发过来的信息 此处的peer是用来记录客服端的地址 DUP需要tcp则不需要
         rd_size = recvfrom(listen_sock,Recv_Buf,RECV_BUFSIZE,0,(struct sockaddr *)&peer,&addrlen);
         if(rd_size == -1) //接受信息出错
         {
             perror("recvfrom error");
             break;
         }else if(rd_size == 0) //没有接受到消息
         {
             //日志处理
             printf("目前没有接受到消息!");
             continue;
         
         }
        // printf("接受到的消息:%s",Recv_Buf);
         //解析出操作的类型
         operate = strtok(Recv_Buf,"@");  
         
         //此处只做简单的处理 
         switch(*operate)
         {
             case '1':
             {
                //将客服端存储到链表中
                if(0 == eplist_add(g_client_pool,peer))
                {
                    //注意c语言中这样直接赋值是不允许的,直接给char数组赋值 只有在char数组初始化的时候才可以
                    //Send_Buf = "客服端节点信息添加成功!";
                    strcpy(Send_Buf,"1@");
                    strcat(Send_Buf,"客服端节点信息登录成功!");
                    //向客服端回复消息
                    sendto(listen_sock,Send_Buf,SEND_BUFSIZE,0,(struct sockaddr *)&peer,sizeof(peer));
                    printf("回复消息发送!"); 
                }else{
                    strcpy(Send_Buf,"1@");
                    strcat(Send_Buf,"客服端节点信息登录失败!");
                    sendto(listen_sock,Send_Buf,SEND_BUFSIZE,0,(struct sockaddr *)&peer,sizeof(peer));
                }  
             }
             break;
             //客服端打洞穿刺
             case '2':
             {
                //提去peer的准备打洞的目的节点的网络地址
                 char * host_port = strtok(NULL,"@");
                 endpoint_t other = ep_fromstring(host_port);
                 //向对方对口发送消息 将消息客服端打包成字符串发给对方客服端
                 strcpy(Send_Buf,"2@");
                 strcat(Send_Buf,ep_tostring(peer));
                 sendto(listen_sock,Send_Buf,SEND_BUFSIZE,0,(struct sockaddr *)&other,sizeof(peer));
                 //向客服端回复信息
                 strcpy(Send_Buf,"1@");
                 strcat(Send_Buf,"打洞消息发出!");  
                 sendto(listen_sock,Send_Buf,strlen(Send_Buf),0,(struct sockaddr *)&peer,sizeof(peer));
                 printf("%s",host_port);
            }
             break;
             //向客户端返回注册的所有节点
            case '3':
             {
                 char text[SEND_BUFSIZE] = {0};
                 //将节点消息当做普通消息处理 加上标号1作为普通标示    
                 strcpy(text,"1@");
                 for(eplist_t *c = g_client_pool->next;c!=NULL;c=c->next)
                 {
                     if(ep_equal(c->endpoint,peer))
                     {
                        strcat(text,"(you)");
                     }
                     strcat(text,ep_tostring(c->endpoint));
                     if(c->next)
                     {
                         strcat(text,";");
                     }
                 }
                 sendto(listen_sock,text,SEND_BUFSIZE,0,(struct sockaddr *)&peer,sizeof(peer));  //将数据下发给请求的客服端
             }
         }

    }
}


int main(int argc,char **argv)
{
    //参数处理
    if(argc != 2){
        printf("Usage:%s <port>\n",argv[0]);
        //正常状态返回的是零 非正常退出返回的是1
        return 1;
    }
    //0.0.0.0表示本机的所有ip地址
    const char * host = "0.0.0.0";
    //获取端口号
    int port = atoi(argv[1]);
    int ret;
    //初始化地址
    endpoint_t server = ep_frompair(host,port);    
    //创建socket udp不连续 无连接 不可靠
    int sock = socket(AF_INET,SOCK_DGRAM,0);
    //socket创建失败
    if(sock == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    //绑定套接字
    ret = bind(sock,(const struct sockaddr *)&server,sizeof(server));
    //套接字绑定失败
    if(ret == -1)
    {
        perror("build");
        exit(EXIT_FAILURE);
    }
    //初始化客服端链表
    g_client_pool = eplist_create();
    
    //开始接受服务端的链接
    udp_receive_loop(sock);
    //当程序退出的时候 一定要清空客服端链接链
    eplist_destroy(g_client_pool);  
    return 0;
}
