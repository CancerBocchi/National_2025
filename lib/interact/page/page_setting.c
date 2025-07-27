#include "page_setting.h"

#define PARA_NUM 2

#define ITEM_START_X 3
#define ITEM_START_Y 3

#define ITEM_WIDTH 120
#define ITEM_HEIGHT 50

extern comkey_t matrix_key[16];

float V_Ref = 0;
float PF = 0;

void page_setting_FirstLongTriggerCallback(int key_num);
void page_setting_LongHoldCallback(int key_num);
void page_setting_HoldTriggerCallback(int key_num);
void page_setting_MultipleClickCallback(int key_num);
void page_setting_KeyPressCallback(int key_num);
void page_setting_KeyReleaseCallback(int key_num);

void page_setting_enter();
void page_setting_update_ui();
void page_setting_exit();
void page_setting_page();

UI_Para_t page_setting_Para[PARA_NUM];

char Input_Key[11] = {
    '7','4','1','.','8','5','2','0','9','6','3'
};


page_setting_t page_setting = {

    .page.FirstLongTriggerCallback = page_setting_FirstLongTriggerCallback,
    .page.LongHoldCallback = page_setting_LongHoldCallback,
    .page.HoldTriggerCallback = page_setting_HoldTriggerCallback,
    .page.MultipleClickCallback = page_setting_MultipleClickCallback,
    .page.KeyPressCallback = page_setting_KeyPressCallback,
    .page.KeyReleaseCallback = page_setting_KeyReleaseCallback,

    .page.page.enter = page_setting_enter,
    .page.page.update_ui = page_setting_update_ui,
    .page.page.exit = page_setting_exit,
    .page.page.page = page_setting_page,

    .para = page_setting_Para,

};

void page_setting_FirstLongTriggerCallback(int key_num){


}

void page_setting_LongHoldCallback(int key_num){


}

void page_setting_HoldTriggerCallback(int key_num){


}

void page_setting_MultipleClickCallback(int key_num){


}

#define VALUE_RANGE_ERROR       0
#define NOT_NUM_ERROR           1
#define INPUT_OK                2
uint8_t setting_page_CheckInput(page_setting_t *page_setting){
    int para_value_int;
    float para_value_float;

    //检查第一个点
    if(page_setting->input_value[0] == '.')
            return NOT_NUM_ERROR;
    
    //检查小数点数量
    uint8_t dot_num;
    for(int i = 1 ;i<page_setting->input_index;i++){
        if(page_setting->input_value[i] == '.'){
            dot_num++;
        }
    }
    if(dot_num > 1)
        return NOT_NUM_ERROR;

    //检查数据范围
    switch(page_setting->para[page_setting->para_index].type){
        case PARA_TYPE_INT:
            para_value_int = atoi(page_setting->input_value);


            //check input value
            if(para_value_int >= (int)page_setting->para[page_setting->para_index].max || 
                para_value_int <=  (int)page_setting->para[page_setting->para_index].min){
                return VALUE_RANGE_ERROR;
            }
            else{
                *(int*)(page_setting->para[page_setting->para_index].para) = para_value_int;
            }

        break;

        case PARA_TYPE_FLOAT:
            para_value_float = atof(page_setting->input_value);
            //check input value
            if(para_value_float >= (float)page_setting->para[page_setting->para_index].max || 
                para_value_float <=  (float)page_setting->para[page_setting->para_index].min){
                return VALUE_RANGE_ERROR;
            }
            else{
                para_value_float = atof(page_setting->input_value);
            *(float*)(page_setting->para[page_setting->para_index].para) = para_value_float;
            }
        break;

    }

    return INPUT_OK;
}

void clear_input_value(page_setting_t *page_setting){
    for(int i = 0;i<page_setting->input_index;i++){
        page_setting->input_value[i] = '\0';
    }
}

void page_setting_KeyPressCallback(int key_num){
    switch(page_setting.state){
        case STATE_INPUT:
            if(key_num <=10){
                page_setting.input_value[page_setting.input_index] = Input_Key[key_num];
                page_setting.input_index = (page_setting.input_index == SETTING_PAGE_INPUT_CHAR_MAX-1)?SETTING_PAGE_INPUT_CHAR_MAX-1:(page_setting.input_index+1);
            }
            //删除建
            else if(key_num == 11){
                //检查当前是否有输入
                if(page_setting.input_index>0){
                    page_setting.input_value[page_setting.input_index] = '\0';
                    page_setting.input_index = (page_setting.input_index == 0)?0:(page_setting.input_index-1);
                }
            }
            //输入建
            else if(key_num == 15){

                if(page_setting.input_index > 0){
                    page_setting.check_flag = setting_page_CheckInput(&page_setting);
                    if(page_setting.check_flag == INPUT_OK){
                        page_setting.state = STATE_PARA_SEL;
                        clear_input_value(&page_setting);
                        page_setting.input_index = 0;
                    }
                    else{
                        clear_input_value(&page_setting);
                        page_setting.input_index = 0;
                    }

                    
                }
                else{
                    clear_input_value(&page_setting);
                    page_setting.input_index = 0;
                    page_setting.state = STATE_PARA_SEL;
                }

            }
            //返回键
            else if(key_num == 12){
                clear_input_value(&page_setting);
                page_setting.input_index = 0;
                page_setting.state = STATE_PARA_SEL;
            }
               
            
        break;

        case STATE_PARA_SEL:
        //index ++
            if(key_num == 14)
                page_setting.para_index = (page_setting.para_index == PARA_NUM-1)?PARA_NUM-1:(page_setting.para_index+1);

            else if(key_num == 13)
                page_setting.para_index = (page_setting.para_index == 0)?0:(page_setting.para_index-1);

            else if(key_num == 15){
                page_setting.state = STATE_INPUT;
            }
            else if(key_num == 12){

                //更新值
                Single_PFC_ChangePF(&PFC,PF);
                Single_PFC_SetBusVolt(PFC,V_Ref);
                route_to(&showPara.page);
            }

        break;
    }

}

void page_setting_KeyReleaseCallback(int key_num){


}

void page_setting_enter(){

    //显示预先设置好的值
    PF = Single_PFC_GetPfRef(PFC);
    V_Ref = Single_PFC_GetBusVRef(PFC);

}

void page_setting_update_ui(){

    for(int i = 0;i<PARA_NUM;i++){

        DrawString(page_setting_Para[i].x,page_setting_Para[i].y,page_setting_Para[i].name,1,page_setting.para_index == i?0:1);

        char str[20];
        (page_setting_Para[i].type == PARA_TYPE_INT)?sprintf(str,"%d",*((int*)page_setting_Para[i].para)):sprintf(str,"%.3f",*((float*)page_setting_Para[i].para));
        DrawString(page_setting_Para[i].x+page_setting_Para[i].blank,page_setting_Para[i].y,str,1,1);
    }

    //输入模式变量输入框显示
    if(page_setting.state == STATE_INPUT){
        DrawMYflaotRect(ITEM_START_X,ITEM_START_Y,ITEM_WIDTH,ITEM_HEIGHT);
        DrawRect1(ITEM_START_X,ITEM_START_Y+1,ITEM_START_X + ITEM_WIDTH,ITEM_START_Y+1 + ITEM_HEIGHT);
        
        char str[20];

        DrawString(ITEM_START_X+4,ITEM_START_Y+4,page_setting.para[page_setting.para_index].name,1,1);

        (page_setting.para[page_setting.para_index].type == PARA_TYPE_INT)?
        sprintf(str,"cur:%d",*(int*)page_setting.para[page_setting.para_index].para):
        sprintf(str,"cur:%.3f",*(float*)page_setting.para[page_setting.para_index].para);

        DrawString(ITEM_START_X+44,ITEM_START_Y+4,str,1,1);
        //输入下划线
        for(int i = 0;i<SETTING_PAGE_INPUT_CHAR_MAX;i++){
            DrawLine(ITEM_START_X+14+i*12,ITEM_START_Y+36,ITEM_START_X+14+i*12+10,ITEM_START_Y+36);
        }   

        DrawString(ITEM_START_X+14,ITEM_START_Y+20,page_setting.input_value,2,1);

        if(page_setting.check_flag != INPUT_OK){
            (page_setting.check_flag == VALUE_RANGE_ERROR)?
            DrawString(ITEM_START_X+50,ITEM_START_Y+40,"range error",1,1):
            DrawString(ITEM_START_X+50,ITEM_START_Y+40,"input error",1,1);
        }
    }

    DrawString(100,50,"Para",1,0);




    // DrawRoundRect();
}

void page_setting_exit(){
    
}

void page_setting_page(){

    page_setting.state = STATE_PARA_SEL;
    page_setting.para_index = 0;
    page_setting.input_index = 0;

    clear_input_value(&page_setting);

    page_setting.check_flag = INPUT_OK;

}


UI_Para_t page_setting_Para[PARA_NUM] = {

    {
        .name = "v_dc",
        .para = &V_Ref,
        .type = PARA_TYPE_FLOAT,
        .blank = 35,
        .x = 1,
        .y = 1,
        .max = 39,
        .min = 32
    },
    {
        .name = "pf",
        .para = &PF,
        .type = PARA_TYPE_FLOAT,
        .blank = 35,
        .x = 1,
        .y = 12,
        .max = 1,
        .min = 0
    },

};

