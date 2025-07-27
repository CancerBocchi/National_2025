#ifndef __PAGE_SETTING_H__
#define __PAGE_SETTING_H__

#include "keyboard_page.h"
#include "page_showPara.h"

#define PARA_TYPE_INT 0
#define PARA_TYPE_FLOAT 1

#define SETTING_PAGE_INPUT_CHAR_MAX 8


typedef struct{

    keyboard_pagebase page;

    char input_value[SETTING_PAGE_INPUT_CHAR_MAX+1];
    uint8_t input_index;
    
    UI_Para_t *para;
    uint8_t para_index;

    enum{
        STATE_PARA_SEL,
        STATE_INPUT,
    }state;

    uint8_t check_flag;


}page_setting_t;

extern page_setting_t page_setting;

#endif // !__PAGE_SETTING_H__