/*************************************************************************
	> File Name: endpoint_list.c
	> Author: echo
	> Mail:moho12@163.com 
	> Created Time: 2018年11月26日 22时38分43秒 CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<assert.h>
#include<arpa/inet.h>
#include"endpoint_list.h"

/**
 * 创建链表的头结点
 * */
eplist_t *eplist_create()
{
    eplist_t * head = (eplist_t *)malloc(sizeof(eplist_t));
    head->lastseen = -1;
    head->next = NULL;
    return head;
}

/***
 *向链表中添加元素
 *
 * */
 int eplist_add(eplist_t * head,endpoint_t ep)
 {
     eplist_t *current;
     //向链条尾部添加函元素 必须将元素的循环到链表的尾部
     for(current = head;current != NULL;current = current->next)
     {
        if(current != head && ep_equal(ep,current->endpoint))
         {
             //如果节点已经存在 则不在添加
             return 1;
         }else if(current->next == NULL) //如果到达链表的末尾节点
         {
             eplist_t *newep = (eplist_t *)malloc(sizeof(eplist_t));
             newep->lastseen = time(NULL); //获取当前的时间 以秒为单位
             newep->endpoint = ep;
             newep->next = NULL;  //设置尾节点
             current->next = newep;  //挂载节点
             return 0;
         }
     }
     return 1;  //一般情况下不会运行到这步
 }
 /**
  *将元素从链表中删除
  *
  * */
  void eplist_destroy(eplist_t *head)
  {
     if(head == NULL)
     {
         return;
     }
     eplist_destroy(head->next); //递归处理
     head->next =NULL; //摘除后面的节点
     free(head);  //释放节点
  }

