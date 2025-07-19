#include "page_setting.h"

extern comkey_t matrix_key[16];

int test = 0;

void FirstLongTriggerCallback(int key_num);
void LongHoldCallback(int key_num);
void HoldTriggerCallback(int key_num);
void MultipleClickCallback(int key_num);
void KeyPressCallback(int key_num);
void KeyReleaseCallback(int key_num);

void page_setting_enter();
void page_setting_update_ui();
void page_setting_exit();
void page_setting_page();

UI_Para_t Para[PARA_NUM];


static page_setting_t page_setting = {

    .page.FirstLongTriggerCallback = FirstLongTriggerCallback,
    .page.LongHoldCallback = LongHoldCallback,
    .page.HoldTriggerCallback = HoldTriggerCallback,
    .page.MultipleClickCallback = MultipleClickCallback,
    .page.KeyPressCallback = KeyPressCallback,
    .page.KeyReleaseCallback = KeyReleaseCallback,

    .page.page.enter = page_setting_enter,
    .page.page.update_ui = page_setting_update_ui,
    .page.page.exit = page_setting_exit,
    .page.page.page = page_setting_page,

};

void FirstLongTriggerCallback(int key_num){


}

void LongHoldCallback(int key_num){


}

void HoldTriggerCallback(int key_num){


}

void MultipleClickCallback(int key_num){


}

void KeyPressCallback(int key_num){


}

void KeyReleaseCallback(int key_num){


}

void page_setting_enter(){

}

void page_setting_update_ui(){

}

void page_setting_exit(){
    
}

void page_setting_page(){

}


UI_Para_t Para[PARA_NUM] = {

    {
        .name = "test1",
        .para = &test,
        .type = PARA_TYPE_INT,
    },
    {},
    {},
    {},
    {},

}

