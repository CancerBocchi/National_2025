#include "page_setting.h"

extern comkey_t matrix_key[16];

int test1 = 0;
int test2 = 100;

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
Input_Key_t Input_Key[11];

page_setting_t page_setting = {

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

    for(int i = 0;i<PARA_NUM;i++){

        DrawString(Para[i].x,Para[i].y,Para[i].name,1,0);

        char str[20];
        (Para[i].type == PARA_TYPE_INT)?sprintf(str,"%d",*((int*)Para[i].para)):sprintf(str,"%.3f",*((float*)Para[i].para));
        DrawString(Para[i].x+Para[i].blank,Para[i].y,str,1,1);
    }

    DrawMYflaotRect(10,10,50,25);
    DrawRect1(10,10,10+50,10+25);
    // DrawRoundRect();
}

void page_setting_exit(){
    
}

void page_setting_page(){

}


UI_Para_t Para[PARA_NUM] = {

    {
        .name = "test1",
        .para = &test1,
        .type = PARA_TYPE_INT,
        .blank = 40,
        .x = 1,
        .y = 1
    },
    {
        .name = "test2",
        .para = &test2,
        .type = PARA_TYPE_INT,
        .blank = 40,
        .x = 1,
        .y = 15
    },

};

Input_Key_t Input_Key[11] = {
    {.key_num = 1,.key_char = '7'},{.key_num = 2,.key_char = '4'},
    {.key_num = 3,.key_char = '1'},{.key_num = 4,.key_char = '.'},
    {.key_num = 5,.key_char = '8'},{.key_num = 6,.key_char = '5'},
    {.key_num = 7,.key_char = '2'},{.key_num = 8,.key_char = '0'},
    {.key_num = 9,.key_char = '9'},{.key_num = 10,.key_char = '6'},
    {.key_num = 11,.key_char = '3'}
};
