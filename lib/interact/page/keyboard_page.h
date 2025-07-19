#ifndef __KEYBOARD_PAGE_H__
#define __KEYBOARD_PAGE_H__

#include "page.h"
#include "key_matrix.h"
#include "comKey.h"

typedef struct keyboard_pagebase{
    page_base page;
    
    void (*FirstLongTriggerCallback)(int key_num);

    void (*LongHoldCallback)(int key_num);

    void (*HoldTriggerCallback)(int key_num);

    void (*MultipleClickCallback)(int key_num);

    void (*KeyPressCallback)(int key_num);

    void (*KeyReleaseCallback)(int key_num);

}keyboard_pagebase;


#endif