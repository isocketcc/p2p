/*************************************************************************
	> File Name: endpoint_list.h
	> Author: echo
	> Mail:moho12@163.com 
	> Created Time: 2018年11月26日 22时28分35秒 CST
 ************************************************************************/

#ifndef _ENDPOINT_LIST_H
#define _ENDPOINT_LIST_H
#include "endpoint.h"
typedef struct _eplist_t eplist_t; 
//定义结构 用于存储目前连上服务器的节点
struct _eplist_t {
    //网络地址
    endpoint_t endpoint;
    //最后登录世间
    time_t lastseen;
    //下一个节点
    eplist_t *next;    

};
//创建客服端链
eplist_t *eplist_create();
//向客服端链中添加元素
int eplist_add(eplist_t *head,endpoint_t ep);
//清空客服端连接链
void eplist_destroy(eplist_t * head);
#endif
