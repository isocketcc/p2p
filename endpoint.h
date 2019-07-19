/*************************************************************************
	> File Name: endpoint.h
	> Author: echo
	> Mail:moho12@163.com 
	> Created Time: 2018年11月26日 21时33分54秒 CST
 ************************************************************************/

#ifndef _ENDPOINT_H
#define _ENDPOINT_H
#include<netinet/in.h>
typedef struct sockaddr_in endpoint_t;

//将网络地址转化成字符串
char * ep_tostring(endpoint_t ep);
//初始化网络地址
endpoint_t ep_fromstring(const char * tuple);
endpoint_t ep_frompair(const char * ip,short port);
//比较两个节点是否相等 参数类型是网络地址
int ep_equal(endpoint_t lp,endpoint_t rp);
#endif
