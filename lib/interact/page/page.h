#ifndef PAGE_H
#define PAGE_H

#include "main.h"

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define screen_height 64

typedef struct
{
    void (*page)(void);//进入页面才执行一次该函数，配置UI和指针
    void (*update_ui)(void);//更新UI
    void (*enter)(void);//进入页面执行的函数，配置外设
    void (*exit)(void);//退出页面执行的函数，配置外设
    void (*tick)(void);

}page_base;

extern page_base *current_page;
void route_to(page_base *new_page);

#endif
