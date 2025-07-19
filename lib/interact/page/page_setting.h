#ifndef __PAGE_SETTING_H__
#define __PAGE_SETTING_H__

#include "keyboard_page.h"

#define PARA_NUM 2

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
    int key_num;
    char key_char;
}Input_Key_t;

typedef struct{

    keyboard_pagebase page;
    char input_value[10];
    UI_Para_t *para;

    enum{
        STATE_PARA_SEL,
        STATE_INPUT,
    }state;


}page_setting_t;

extern page_setting_t page_setting;

#endif // !__PAGE_SETTING_H__