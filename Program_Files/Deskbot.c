/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : deskbot.c
  * @brief          : Final OS - Background Clock Registers & Gyro
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

/* --- OS GLOBAL STATE & REGISTERS --- */
typedef enum { MODE_EYES = 0, MODE_MENU, MODE_TIMER, MODE_CLOCK, MODE_CALC, MODE_GYRO } OS_Mode;
OS_Mode current_mode = MODE_EYES;
uint8_t mode_init_needed = 1;
int menu_idx = 0;

/* --- INDEPENDENT CLOCK REGISTERS --- */
uint8_t sys_hours = 0;
uint8_t sys_minutes = 0;
uint8_t sys_seconds = 0;

/* --- COLORS --- */
uint16_t cyan=0x07FF, black=0x0000, white=0xFFFF, dark_grey=0x2104, green=0x07E0, red=0xF800, amber=0xFD20;

/* --- FONTS --- */
const char* app_names[5] = {"NORMAL FACE", "TIMER", "CLOCK", "CALCULATOR", "GYRO PUSH"};
const uint8_t Font5x7[64][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x5f, 0x00, 0x00}, {0x00, 0x07, 0x00, 0x07, 0x00}, {0x14, 0x7f, 0x14, 0x7f, 0x14},
  {0x24, 0x2a, 0x7f, 0x2a, 0x12}, {0x23, 0x13, 0x08, 0x64, 0x62}, {0x36, 0x49, 0x55, 0x22, 0x50}, {0x00, 0x05, 0x03, 0x00, 0x00},
  {0x00, 0x1c, 0x22, 0x41, 0x00}, {0x00, 0x41, 0x22, 0x1c, 0x00}, {0x14, 0x08, 0x3E, 0x08, 0x14}, {0x08, 0x08, 0x3E, 0x08, 0x08},
  {0x00, 0x00, 0x50, 0x30, 0x00}, {0x08, 0x08, 0x08, 0x08, 0x08}, {0x00, 0x60, 0x60, 0x00, 0x00}, {0x20, 0x10, 0x08, 0x04, 0x02},
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, {0x00, 0x42, 0x7F, 0x40, 0x00}, {0x42, 0x61, 0x51, 0x49, 0x46}, {0x21, 0x41, 0x45, 0x4B, 0x31},
  {0x18, 0x14, 0x12, 0x7F, 0x10}, {0x27, 0x45, 0x45, 0x45, 0x39}, {0x3C, 0x4A, 0x49, 0x49, 0x30}, {0x01, 0x71, 0x09, 0x05, 0x03},
  {0x36, 0x49, 0x49, 0x49, 0x36}, {0x06, 0x49, 0x49, 0x29, 0x1E}, {0x00, 0x36, 0x36, 0x00, 0x00}, {0x00, 0x56, 0x36, 0x00, 0x00},
  {0x08, 0x14, 0x22, 0x41, 0x00}, {0x14, 0x14, 0x14, 0x14, 0x14}, {0x00, 0x41, 0x22, 0x14, 0x08}, {0x02, 0x01, 0x51, 0x09, 0x06},
  {0x32, 0x49, 0x59, 0x51, 0x3E}, {0x7E, 0x11, 0x11, 0x11, 0x7E}, {0x7F, 0x49, 0x49, 0x49, 0x36}, {0x3E, 0x41, 0x41, 0x41, 0x22},
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, {0x7F, 0x49, 0x49, 0x49, 0x41}, {0x7F, 0x09, 0x09, 0x09, 0x01}, {0x3E, 0x41, 0x49, 0x49, 0x7A},
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, {0x00, 0x41, 0x7F, 0x41, 0x00}, {0x20, 0x40, 0x41, 0x3F, 0x01}, {0x7F, 0x08, 0x14, 0x22, 0x41},
  {0x7F, 0x40, 0x40, 0x40, 0x40}, {0x7F, 0x02, 0x0C, 0x02, 0x7F}, {0x7F, 0x04, 0x08, 0x10, 0x7F}, {0x3E, 0x41, 0x41, 0x41, 0x3E},
  {0x7F, 0x09, 0x09, 0x09, 0x06}, {0x3E, 0x41, 0x51, 0x21, 0x5E}, {0x7F, 0x09, 0x19, 0x29, 0x46}, {0x46, 0x49, 0x49, 0x49, 0x31},
  {0x01, 0x01, 0x7F, 0x01, 0x01}, {0x3F, 0x40, 0x40, 0x40, 0x3F}, {0x1F, 0x20, 0x40, 0x20, 0x1F}, {0x3F, 0x40, 0x38, 0x40, 0x3F},
  {0x63, 0x14, 0x08, 0x14, 0x63}, {0x07, 0x08, 0x70, 0x08, 0x07}, {0x61, 0x51, 0x49, 0x45, 0x43}, {0x00, 0x7F, 0x41, 0x41, 0x00},
  {0x02, 0x04, 0x08, 0x10, 0x20}, {0x00, 0x41, 0x41, 0x7F, 0x00}, {0x04, 0x02, 0x01, 0x02, 0x04}, {0x40, 0x40, 0x40, 0x40, 0x40}
};
const uint8_t font_modern_straight[15][8] = {
    {0x3F, 0x33, 0x33, 0x00, 0x33, 0x33, 0x3F, 0x00}, {0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x00},
    {0x3F, 0x03, 0x03, 0x3F, 0x30, 0x30, 0x3F, 0x00}, {0x3F, 0x03, 0x03, 0x3F, 0x03, 0x03, 0x3F, 0x00},
    {0x33, 0x33, 0x33, 0x3F, 0x03, 0x03, 0x03, 0x00}, {0x3F, 0x30, 0x30, 0x3F, 0x03, 0x03, 0x3F, 0x00},
    {0x3F, 0x30, 0x30, 0x3F, 0x33, 0x33, 0x3F, 0x00}, {0x3F, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x00},
    {0x3F, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x3F, 0x00}, {0x3F, 0x33, 0x33, 0x3F, 0x03, 0x03, 0x3F, 0x00},
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},
    {0x24, 0x24, 0x18, 0x18, 0x24, 0x24, 0x00, 0x00}, {0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00},
    {0x1C, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/* Drivers & Prototypes */
void TFT_Command(uint8_t cmd) { HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 0); HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); HAL_SPI_Transmit(&hspi1, &cmd, 1, 10); HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1); }
void TFT_Data(uint8_t data) { HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 1); HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); HAL_SPI_Transmit(&hspi1, &data, 1, 10); HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1); }
void TFT_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (w <= 0 || h <= 0) return;
    TFT_Command(0x2A); TFT_Data(x >> 8); TFT_Data(x & 0xFF); TFT_Data((x+w-1) >> 8); TFT_Data((x+w-1) & 0xFF);
    TFT_Command(0x2B); TFT_Data(y >> 8); TFT_Data(y & 0xFF); TFT_Data((y+h-1) >> 8); TFT_Data((y+h-1) & 0xFF);
    TFT_Command(0x2C);
    uint8_t lb[640]; uint16_t aw = (w > 320) ? 320 : w;
    for(uint32_t i=0; i<aw; i++){ lb[i*2]=color>>8; lb[i*2+1]=color&0xFF; }
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 1); HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
    for(uint32_t i=0; i<h; i++){ HAL_SPI_Transmit(&hspi1, lb, aw*2, 10); }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
}
void Draw_Smooth_Pill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if(h <= 8) { TFT_FillRect(x+4, y, w-8, h, color); return; } // Anti-crash guard
    TFT_FillRect(x+4, y, w-8, h, color); TFT_FillRect(x+2, y+2, w-4, h-4, color); TFT_FillRect(x, y+4, w, h-8, color);
}
void TFT_Print(uint16_t x, uint16_t y, const char* str, uint16_t col, uint16_t bg, uint8_t scale) {
    while (*str) {
        char c = *str++; int idx = (c < 32 || c > 95) ? 0 : c - 32;
        TFT_Command(0x2A); TFT_Data(x >> 8); TFT_Data(x & 0xFF); TFT_Data((x+(5*scale)-1) >> 8); TFT_Data((x+(5*scale)-1) & 0xFF);
        TFT_Command(0x2B); TFT_Data(y >> 8); TFT_Data(y & 0xFF); TFT_Data((y+(8*scale)-1) >> 8); TFT_Data((y+(8*scale)-1) & 0xFF);
        TFT_Command(0x2C); uint8_t lb[160]; HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 1); HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
        for (int r=0; r<8; r++) { int bi=0; for (int cl=0; cl<5; cl++) { uint16_t p = (r<7 && (Font5x7[idx][cl] & (1 << r))) ? col : bg; for(int sx=0; sx<scale; sx++) { lb[bi++]=p>>8; lb[bi++]=p&0xFF; } } for (int sy=0; sy<scale; sy++) { HAL_SPI_Transmit(&hspi1, lb, 5*scale*2, 10); } }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1); x += (6*scale);
    }
}
void TFT_PrintStraight(uint16_t x, uint16_t y, char* str, uint16_t color, uint16_t bg, uint8_t size) {
    while (*str) {
        char c = *str++; uint8_t idx = (c==':')?10:(c=='-')?10:(c=='+')?11:(c=='*')?12:(c=='/')?13:(c=='^')?14:(c>='0'&&c<='9')?c-'0':0;
        for (int j=0; j<8; j++) { uint8_t row=font_modern_straight[idx][j]; for (int i=0; i<6; i++) { if (row & (1<<(5-i))) TFT_FillRect(x+(i*size), y+(j*size), size, size, color); else TFT_FillRect(x+(i*size), y+(j*size), size, size, bg); } }
        x += 7*size;
    }
}
void TFT_Init_Sequence(uint8_t rot) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 0); HAL_Delay(100); HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 1); HAL_Delay(100);
    TFT_Command(0x01); HAL_Delay(150); TFT_Command(0x11); HAL_Delay(150); TFT_Command(0x36); TFT_Data(rot); TFT_Command(0x3A); TFT_Data(0x55); TFT_Command(0x29); HAL_Delay(100);
}

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

/* --- APP LOGIC --- */

void Run_App_Eyes(void) {
    if(mode_init_needed){ TFT_Init_Sequence(0x28); TFT_FillRect(0,0,320,240,black); mode_init_needed=0; }

    static uint8_t state = 0; // STATE_IDLE
    static uint32_t state_start_time = 0;
    static uint32_t idle_wait_time = 1000;
    static float last_progress = -1.0f;

    int fixed_w = 40, max_h = 80, left_x = 80, right_x = 200, y_center = 120;
    uint32_t now = HAL_GetTick();
    uint32_t elapsed = now - state_start_time;
    float progress = 1.0f;

    if (state == 0) { // IDLE
        if (elapsed >= idle_wait_time) { state = 1; state_start_time = now; }
    } else if (state == 1) { // CLOSING
        if (elapsed >= 150) { state = 2; state_start_time = now; progress = 0.0f; }
        else progress = 1.0f - ((float)elapsed / 150.0f);
    } else if (state == 2) { // OPENING
        if (elapsed >= 150) { state = 0; state_start_time = now; idle_wait_time = (rand() % 2501) + 500; progress = 1.0f; }
        else progress = ((float)elapsed / 150.0f);
    }

    if (state == 0 && progress == last_progress) return;
    last_progress = progress;

    int current_h = (int)(max_h * progress);
    if (current_h < 8) current_h = 8; // Anti-Crash Guard
    int current_y = y_center - (current_h / 2);

    int top_erase_h = current_y - (y_center - max_h/2);
    int bottom_erase_y = current_y + current_h;
    int bottom_erase_h = (y_center + max_h/2) - bottom_erase_y;

    if (top_erase_h > 0) {
        TFT_FillRect(left_x, y_center - max_h/2, fixed_w, top_erase_h, black);
        TFT_FillRect(right_x, y_center - max_h/2, fixed_w, top_erase_h, black);
    }
    Draw_Smooth_Pill(left_x, current_y, fixed_w, current_h, cyan);
    Draw_Smooth_Pill(right_x, current_y, fixed_w, current_h, cyan);
    if (bottom_erase_h > 0) {
        TFT_FillRect(left_x, bottom_erase_y, fixed_w, bottom_erase_h, black);
        TFT_FillRect(right_x, bottom_erase_y, fixed_w, bottom_erase_h, black);
    }
}

void Run_App_Menu(void) {
    if(mode_init_needed){ TFT_Init_Sequence(0x68); TFT_FillRect(0,0,320,240,black);
    for(int i=0; i<5; i++){ char n[4]; sprintf(n,"%d.",i+1); TFT_Print(15,i*48+16,n,white,black,2); TFT_Print(55,i*48+16,app_names[i],cyan,black,2); if(i<4)TFT_FillRect(10,i*48+47,300,1,dark_grey); }
    mode_init_needed=0; }
    static int oi=-1; if(menu_idx!=oi){ if(oi!=-1){ int yo=(oi*48)+24; for(int i=0;i<12;i++)TFT_FillRect(280+i,yo-i,1,(i*2)+1,black); }
    int yn=(menu_idx*48)+24; for(int i=0;i<12;i++)TFT_FillRect(280+i,yn-i,1,(i*2)+1,green); oi=menu_idx; }
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1){ menu_idx=(menu_idx<=0)?4:menu_idx-1; HAL_Delay(250); }
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==1){ menu_idx=(menu_idx>=4)?0:menu_idx+1; HAL_Delay(250); }
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1){ current_mode=(OS_Mode)(menu_idx+1); mode_init_needed=1; HAL_Delay(300); }
}

void Run_App_Timer(void) {
    static uint32_t s=60, lt=0, pv=999; static uint8_t run=0, ps=0, st=0; char buf[16];
    if(mode_init_needed){ TFT_Init_Sequence(0x68); TFT_FillRect(0,0,320,240,black); s=60; run=0; st=0; pv=999; mode_init_needed=0; }
    if(run && (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1 || HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==1)){ run=0; s=60; pv=999; mode_init_needed=1; HAL_Delay(400); }
    if(!run){
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1){ if(s<3600)s+=60; HAL_Delay(200); }
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==1){ if(s>60)s-=60; HAL_Delay(200); }
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1){ run=1; ps=0; lt=HAL_GetTick(); TFT_FillRect(0,0,320,240,black); HAL_Delay(300); }
        if(pv!=(s/60)){ TFT_FillRect(80,70,160,100,black); sprintf(buf,"%02ld",s/60); TFT_PrintStraight(95,80,buf,white,black,12); pv=s/60; }
    } else {
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1){ ps=!ps; HAL_Delay(300); }
        if(!ps && HAL_GetTick()-lt>=1000){ if(s>0)s--; else run=0; lt=HAL_GetTick(); }
        if(s>=60){ if(st!=1){TFT_FillRect(0,0,320,240,black);st=1;} sprintf(buf,"%02ld:%02ld",s/60,s%60); TFT_PrintStraight(45,85,buf,white,black,8); }
        else if(s>0){ if(st!=2){TFT_FillRect(0,0,320,240,black);st=2;} sprintf(buf,"%02ld",s); TFT_PrintStraight(110,75,buf, (s<=10?red:white), black, 12); }
        else { TFT_FillRect(0,0,320,240,black); TFT_PrintStraight(60,85,"DONE",red,black,10); run=0; HAL_Delay(2000); mode_init_needed=1; }
    }
}

void Run_App_Clock(void) {
    if(mode_init_needed){
        TFT_Init_Sequence(0x68);
        TFT_FillRect(0,0,320,240,black);
        mode_init_needed=0;
    }

    // Uses the global clock registers directly! No delays to block the buttons!
    static uint8_t last_drawn_min = 99;

    if (sys_minutes != last_drawn_min) {
        char buf[12];
        sprintf(buf, "%02d:%02d", sys_hours, sys_minutes);
        TFT_FillRect(20, 85, 280, 80, black); // Erase old time area
        TFT_PrintStraight(20, 85, buf, white, black, 8);
        last_drawn_min = sys_minutes;
    }
    HAL_Delay(10); // Small delay to avoid pegging the CPU
}

void Run_App_Calc(void) {
    static int32_t vA=0, vB=0; static int8_t op=0, cs=0; static uint32_t pv=999; char disp[20];
    if(mode_init_needed){ TFT_Init_Sequence(0x68); TFT_FillRect(0,0,320,240,black); vA=0;vB=0;op=0;cs=0;pv=999;mode_init_needed=0; }
    if(cs==0){
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1){if(vA<99)vA++;HAL_Delay(150);} if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==1){if(vA>0)vA--;HAL_Delay(150);}
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1){cs=1;pv=999;TFT_FillRect(0,0,320,240,black);HAL_Delay(300);}
        if(pv!=vA){TFT_PrintStraight(100,30,"VAR A",white,black,3);sprintf(disp,"%ld",vA);TFT_FillRect(0,80,320,100,black);TFT_PrintStraight((320-(strlen(disp)*84))/2,85,disp,amber,black,12);pv=vA;}
    } else if(cs==1){
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1){if(vB<99)vB++;HAL_Delay(150);} if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==1){if(vB>0)vB--;HAL_Delay(150);}
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1){cs=2;pv=999;TFT_FillRect(0,0,320,240,black);HAL_Delay(300);}
        if(pv!=vB){TFT_PrintStraight(100,30,"VAR B",white,black,3);sprintf(disp,"%ld",vB);TFT_FillRect(0,80,320,100,black);TFT_PrintStraight((320-(strlen(disp)*84))/2,85,disp,amber,black,12);pv=vB;}
    } else if(cs==2){
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1){op=(op+1)%5;HAL_Delay(200);} if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==1){op=(op==0)?4:op-1;HAL_Delay(200);}
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1){cs=3;pv=999;TFT_FillRect(0,0,320,240,black);HAL_Delay(300);}
        if(pv!=op){TFT_PrintStraight(80,30,"OPERATION",white,black,3);char sy[2]={0};if(op==0)sy[0]='+';else if(op==1)sy[0]='-';else if(op==2)sy[0]='*';else if(op==3)sy[0]='/';else sy[0]='^';TFT_FillRect(0,80,320,100,black);TFT_PrintStraight(135,85,sy,white,black,12);pv=op;}
    } else {
        if(pv!=888){long r=0;if(op==0)r=vA+vB;else if(op==1)r=vA-vB;else if(op==2)r=vA*vB;else if(op==3)r=(vB!=0?vA/vB:0);else r=(long)pow(vA,vB);sprintf(disp,"%ld",r);TFT_PrintStraight((320-(strlen(disp)*70))/2,85,disp,white,black,10);pv=888;}
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1){cs=0;vA=0;vB=0;op=0;TFT_FillRect(0,0,320,240,black);pv=999;HAL_Delay(300);}
    }
}

void Run_App_Gyro(void) {
    static int16_t bx=0; static int pf=0, vf=0, lL=80;
    static uint32_t stable_st=0; static int16_t last_r=0;

    if(mode_init_needed){
        TFT_Init_Sequence(0x28); TFT_FillRect(0,0,320,240,black);
        uint8_t p=0; HAL_I2C_Mem_Write(&hi2c1,(0x68<<1),0x6B,1,&p,1,100);
        HAL_Delay(100);
        uint8_t d_in[2]; if(HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x3B,1,d_in,2,100)==HAL_OK) bx=(int16_t)(d_in[0]<<8|d_in[1]);
        pf=0; vf=0; lL=80; last_r=bx; stable_st=HAL_GetTick(); mode_init_needed=0;
        Draw_Smooth_Pill(80, 80, 40, 80, cyan); Draw_Smooth_Pill(200, 80, 40, 80, cyan);
    }

    uint8_t d[2]; int16_t cx=0; if(HAL_I2C_Mem_Read(&hi2c1,(0x68<<1),0x3B,1,d,2,100)==HAL_OK) cx=(int16_t)(d[0]<<8|d[1]);

    if(abs(cx - last_r) < 500) { if(HAL_GetTick() - stable_st > 1000) { bx = cx; stable_st = HAL_GetTick(); } }
    else { stable_st = HAL_GetTick(); last_r = cx; }

    int16_t f = cx - bx;
    if(abs(f)>4000) vf+=(-(f/12)*256);
    vf+=(0-pf)>>3; vf-=(vf>>2); pf+=vf;
    if(pf>20480) pf=20480; if(pf<-20480) pf=-20480;
    int off=pf>>8; int cL=80+off, cR=200+off;

    if(cL!=lL){
        int dx=cL-lL;
        if(dx>0){TFT_FillRect(lL,80,dx,80,black);TFT_FillRect(200+(lL-80),80,dx,80,black);}
        else{TFT_FillRect(cL+40,80,-dx,80,black);TFT_FillRect(cR+40,80,-dx,80,black);}
        Draw_Smooth_Pill(cL, 80, 40, 80, cyan); Draw_Smooth_Pill(cR, 80, 40, 80, cyan); lL=cL;
    }
    HAL_Delay(10);
}

/* --- MAIN --- */
int main(void) {
  HAL_Init(); SystemClock_Config(); MX_GPIO_Init(); MX_I2C1_Init(); MX_RTC_Init(); MX_SPI1_Init(); MX_USART2_UART_Init();

  /* ONE TIME RTC INITIALIZATION AT BOOT */
  RTC_TimeTypeDef boot_time = {0};
  char compile_time[] = __TIME__;
  int h, m, ss;
  sscanf(compile_time, "%d:%d:%d", &h, &m, &ss);
  boot_time.Hours = h;
  boot_time.Minutes = m;
  boot_time.Seconds = ss;
  HAL_RTC_SetTime(&hrtc, &boot_time, RTC_FORMAT_BIN);

  TFT_Init_Sequence(0x28); TFT_FillRect(0, 0, 320, 240, black);

  uint32_t last_rtc_update = 0;

  while (1) {

      /* BACKGROUND CLOCK ENGINE */
      // Updates the registers quietly every 1000ms, regardless of what app is open!
      if (HAL_GetTick() - last_rtc_update >= 1000) {
          RTC_TimeTypeDef curr_t;
          RTC_DateTypeDef curr_d; // Must read date after time for STM32 hardware unlock
          HAL_RTC_GetTime(&hrtc, &curr_t, RTC_FORMAT_BIN);
          HAL_RTC_GetDate(&hrtc, &curr_d, RTC_FORMAT_BIN);
          sys_hours = curr_t.Hours;
          sys_minutes = curr_t.Minutes;
          sys_seconds = curr_t.Seconds;
          last_rtc_update = HAL_GetTick();
      }

      /* GLOBAL NAVIGATION */
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 1) { current_mode = MODE_MENU; mode_init_needed = 1; HAL_Delay(300); }
      if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 1) { current_mode = MODE_EYES; mode_init_needed = 1; HAL_Delay(300); }

      /* APP EXECUTION */
      switch(current_mode) {
          case MODE_EYES:Run_App_Eyes();break;
          case MODE_MENU:Run_App_Menu();break;
          case MODE_TIMER:Run_App_Timer();break;
          case MODE_CLOCK:Run_App_Clock();break;
          case MODE_CALC:Run_App_Calc();break;
          case MODE_GYRO:Run_App_Gyro();break;
      }
  }
}

/* --- HARDWARE CONFIG --- */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0}; RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON; RCC_OscInitStruct.HSI48State = RCC_HSI48_ON; RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON; RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48; RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1; PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI; PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}
static void MX_I2C1_Init(void) { hi2c1.Instance = I2C1; hi2c1.Init.Timing = 0x00201D2B; hi2c1.Init.OwnAddress1 = 0; hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; hi2c1.Init.OwnAddress2 = 0; hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK; hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; HAL_I2C_Init(&hi2c1); }
static void MX_RTC_Init(void) { hrtc.Instance = RTC; hrtc.Init.HourFormat = RTC_HOURFORMAT_24; hrtc.Init.AsynchPrediv = 127; hrtc.Init.SynchPrediv = 255; hrtc.Init.OutPut = RTC_OUTPUT_DISABLE; HAL_RTC_Init(&hrtc); }
static void MX_SPI1_Init(void) { hspi1.Instance = SPI1; hspi1.Init.Mode = SPI_MODE_MASTER; hspi1.Init.Direction = SPI_DIRECTION_2LINES; hspi1.Init.DataSize = SPI_DATASIZE_8BIT; hspi1.Init.CLKPolarity = SPI_POLARITY_LOW; hspi1.Init.CLKPhase = SPI_PHASE_1EDGE; hspi1.Init.NSS = SPI_NSS_SOFT; hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB; hspi1.Init.TIMode = SPI_TIMODE_DISABLE; hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; HAL_SPI_Init(&hspi1); }
static void MX_USART2_UART_Init(void) { huart2.Instance = USART2; huart2.Init.BaudRate = 38400; huart2.Init.WordLength = UART_WORDLENGTH_8B; huart2.Init.StopBits = UART_STOPBITS_1; huart2.Init.Parity = UART_PARITY_NONE; huart2.Init.Mode = UART_MODE_TX_RX; HAL_UART_Init(&huart2); }
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef G = {0}; __HAL_RCC_GPIOA_CLK_ENABLE(); __HAL_RCC_GPIOC_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE();
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5, 1); HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
  G.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4; G.Mode = GPIO_MODE_INPUT; G.Pull = GPIO_PULLDOWN; HAL_GPIO_Init(GPIOA, &G);
  G.Pin = GPIO_PIN_4|GPIO_PIN_5; G.Mode = GPIO_MODE_OUTPUT_PP; G.Pull = GPIO_NOPULL; G.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(GPIOC, &G);
  G.Pin = GPIO_PIN_0|GPIO_PIN_1; G.Mode = GPIO_MODE_INPUT; G.Pull = GPIO_PULLDOWN; HAL_GPIO_Init(GPIOB, &G);
  G.Pin = GPIO_PIN_12; G.Mode = GPIO_MODE_OUTPUT_PP; G.Pull = GPIO_NOPULL; G.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(GPIOB, &G);
}
void Error_Handler(void) { __disable_irq(); while (1) {} }