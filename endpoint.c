/*************************************************************************
	> File Name: endpoint.c
	> Author: echo
	> Mail:moho12@163.com 
	> Created Time: 2018年11月26日 21时42分51秒 CST
 ************************************************************************/
#include"endpoint.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
//指定端口号的长度
#define INET_PORTSTRLEN 5
//元祖的长度
#define TUPLE_LEN (INET_ADDRSTRLEN + INET_PORTSTRLEN + 1)

/***
 *将网络地址转化成字符串
 * */
 char *ep_tostring(endpoint_t ep)
 {
    static char tuple[TUPLE_LEN];
     //格式化字符串
    snprintf(tuple,TUPLE_LEN,"%s:%d",inet_ntoa(ep.sin_addr),ntohs(ep.sin_port));
    return tuple;
 }

/**
 *从客服端节点中获取网络地址
 *
 * */

endpoint_t ep_frompair(const char *host,short port)
{
    endpoint_t ep;
    memset(&ep,0,sizeof ep);
    ep.sin_family = AF_INET;
    ep.sin_addr.s_addr = inet_addr(host);
    ep.sin_port = htons(port); 
    return ep;
}

/***
 *从字符串中分割字符串
 * */
 endpoint_t ep_fromstring(const char * tuple)
 {
     static char _tuple[TUPLE_LEN];
     char * host = NULL;
     char * port = NULL;
     //格式化字符串
     sprintf(_tuple,"%s",tuple);
     host = strtok(_tuple,":");
     port = strtok(NULL,":");
     //如果传入的地址和端口都是NULL
     if(host == NULL || port == NULL)
     {
         host = "255.255.255.255";  //设置地址为广播地址
         port = "0";
     }
     return ep_frompair(host,atoi(port));
 }

/**
 *比较两个节点是否相等
 * */
 int ep_equal(endpoint_t lp,endpoint_t rp)
 {
     return ((lp.sin_family == rp.sin_family) && 
            (lp.sin_addr.s_addr == rp.sin_addr.s_addr) &&
            (lp.sin_port == rp.sin_port));
 }

