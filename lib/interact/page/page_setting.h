#ifndef __PAGE_SETTING_H__
#define __PAGE_SETTING_H__

#include "keyboard_page.h"

#define PARA_NUM 10

#define PARA_TYPE_INT 0
#define PARA_TYPE_FLOAT 1

typedef struct{

    uint16_t x;
    uint16_t y;
    uint16_t blank; //显示值和名称之间的距离

    char name[20];
    void* para;
    uint8_t type;

}UI_Para_t;

typedef struct{

    keyboard_pagebase page;

    char input_value[10];

    UI_Para_t *para;


}page_setting_t;

#endif // !__PAGE_SETTING_H__