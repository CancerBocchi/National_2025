
#ifndef __PAGE_SHOWPARA_H__
#define __PAGE_SHOWPARA_H__

#include "page.h"
#include "keyboard_page.h"
#include "page_setting.h"
#include "task.h"

typedef struct{
    uint8_t value;
    char string[20];
}str_table;

typedef struct {

    uint16_t x;
    uint16_t y;
    uint16_t blank; //显示值和名称之间的距离

    char name[20];
    uint8_t* para;

    str_table* str;
    uint8_t table_num;


}UI_Str_t;

typedef struct {
    keyboard_pagebase page;
    UI_Para_t *para;
    UI_Str_t *str;

}showPara_t;

extern showPara_t showPara;



#endif // !__PAGE_SHOWPARA_H__