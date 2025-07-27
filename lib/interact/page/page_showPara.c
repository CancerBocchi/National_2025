#include "page_showPara.h"

#define PARA_NUM 4
#define STR_NUM 1

extern comkey_t matrix_key[16];

static float PF = 0;
static float V_Bus = 0;
static float Vac = 0;
static float Iac = 0;

void showPara_FirstLongTriggerCallback(int key_num);
void showPara_LongHoldCallback(int key_num);
void showPara_HoldTriggerCallback(int key_num);
void showPara_MultipleClickCallback(int key_num);
void showPara_KeyPressCallback(int key_num);
void showPara_KeyReleaseCallback(int key_num);

void showPara_enter();
void showPara_update_ui();
void showPara_exit();
void showPara_page();

UI_Para_t showPara_Para[PARA_NUM];
UI_Str_t showPara_str[STR_NUM];

showPara_t showPara = {

    .page.FirstLongTriggerCallback = showPara_FirstLongTriggerCallback,
    .page.LongHoldCallback = showPara_LongHoldCallback,
    .page.HoldTriggerCallback = showPara_HoldTriggerCallback,
    .page.MultipleClickCallback = showPara_MultipleClickCallback,
    .page.KeyPressCallback = showPara_KeyPressCallback,
    .page.KeyReleaseCallback = showPara_KeyReleaseCallback,

    .page.page.enter = showPara_enter,
    .page.page.update_ui = showPara_update_ui,
    .page.page.exit = showPara_exit,
    .page.page.page = showPara_page,

    .para = showPara_Para,

};

void showPara_FirstLongTriggerCallback(int key_num){


}

void showPara_LongHoldCallback(int key_num){


}

void showPara_HoldTriggerCallback(int key_num){


}

void showPara_MultipleClickCallback(int key_num){


}


void showPara_KeyPressCallback(int key_num){
    if(key_num == 12){
        route_to(&page_setting.page);
    }
    else if(key_num == 15){
        if(PFC.state ==  || PFC.state == SINGLE_PFC_PROTECTION)
            Single_PFC_Start(&PFC);
        else{
            Single_PFC_Stop(&PFC);
        }
    }
}

void showPara_KeyReleaseCallback(int key_num){


}

void showPara_enter(){

}

void showPara_update_ui(){

    PF = Single_PFC_GetRealPF(PFC);
    V_Bus = Single_PFC_GetBusVolt(PFC);
    Iac = Single_PFC_GetLCurrent(PFC);
    Vac = Single_PFC_GetAcVolt(PFC);
    // Vac = PFC.ac_volt*SINGLE_PFC_AC_MAX;

    for(int i = 0;i<PARA_NUM;i++){

        DrawString(showPara.para[i].x,showPara.para[i].y,showPara.para[i].name,1,1);

        char str[20];
        (showPara.para[i].type == PARA_TYPE_INT)?sprintf(str,"%d",*((int*)showPara.para[i].para)):sprintf(str,"%.3f",*((float*)showPara.para[i].para));
        DrawString(showPara.para[i].x+showPara.para[i].blank,showPara.para[i].y,str,1,1);
    }

    for(int i = 0;i<STR_NUM;i++){
        DrawString(showPara_str[i].x,showPara_str[i].y,showPara_str[i].name,1,1);

        for(int j = 0;j<showPara_str[i].table_num;j++){
            if(*showPara_str[i].para == showPara_str[i].str[j].value)
                DrawString(showPara_str[i].x+showPara_str[i].blank,showPara_str[i].y,showPara_str[i].str[j].string,1,1);
        }
        
    }

    DrawString(100,50,"show",1,0);

    PF = Single_PFC_GetRealPF(PFC);
}

void showPara_exit(){
    
}

void showPara_page(){

}

str_table state_table[5] = {
    {
        .value = SINGLE_PFC_PLL, 
        .string = "PLL"
    },
    {
        .value = SINGLE_PFC_PROTECTION, 
        .string = "PRO"
    },
    {
        .value = SINGLE_PFC_CZ, 
        .string = "CZ"
    },
    {
        .value = SINGLE_PFC_OUTPUT, 
        .string = "OUT"
    },
    {
        .value = , 
        .string = "IDLE"
    },

};



UI_Str_t showPara_str[STR_NUM] = {

    {
        .name = "state",
        .para = &PFC.state,
        .blank = 35,
        .str = state_table,
        .table_num = 5,
        .x = 60,
        .y = 1,
    },
};

UI_Para_t showPara_Para[PARA_NUM] = {

    {
        .name = "PF",
        .para = &PF,
        .type = PARA_TYPE_FLOAT,
        .blank = 35,
        .x = 1,
        .y = 1,
    },
    {
        .name = "V_Bus",
        .para = &V_Bus,
        .type = PARA_TYPE_FLOAT,
        .blank = 35,
        .x = 1,
        .y = 10,
    },
    {   
        .name = "Vac",
        .para = &Vac,
        .type = PARA_TYPE_FLOAT,
        .blank = 35,
        .x = 1,
        .y = 19,
    },
    {
        .name = "Iac",
        .para = &Iac,
        .type = PARA_TYPE_FLOAT,
        .blank = 35,
        .x = 1,
        .y = 28,
    }

};

