#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "playMusic.h"
#include <stdlib.h>
#include <stdio.h>

void init_lcd(void);
void display3(const char *);
void display4(const char *);
void press(char);

void init_pwm(void);
void update_freq(int f);
void update_rgb(int r, int g, int b);

void init_keypad(void);
void scan_keypad(void);

void display1(const char *);
void display2(const char *);
void init_lcd2(void);

void math_time();
void alarmON(void);

char timeArr[16]; // used to store time displayed
char input[3]; // used to store user input for answer
char ans[3]; // used to store answer of generated math problem
char input_time[16];

int setting = 2;
int timeofday = 9;
int cond = 0;
int alarmSound = 0; // 1 if alarm sound is active (use to trigger sound)
int alarmSet = 0;
char ansArr[9]; //
static int row = 0;
int entry = ' ';
int seed;
int onlyOnce = 0;
int noCopy = 0;
int wrong = -1;
int newE = 0;
int numTry = 0;


void alarmON(void)
{
    if(onlyOnce != 1) // check onlyOnce value
    {
        math_time(ans, ansArr);
        onlyOnce++;
    }
    else{
        if(noCopy == 0){
            if(timeArr[3] == input_time[0] && timeArr[4] == input_time[1] && timeArr[6] == input_time[2] && timeArr[7] == input_time[3] && timeofday == setting)
            {
                alarmSound = 1;
                char opt[20] = {'Q',':', '(', ansArr[0], ' ', ansArr[3] , ' ', ansArr[1], ')', ' ', ansArr[4], ' ', ansArr[2], ' ', '=', ' '};
                display3(opt);
                display4("                 ");
                playMusic();
                newE = 4;
                noCopy++;
            }
        }
        else
        {

        }
    }
}


void scan_keypad(void) {
    for(;;){
        for(row=1; row<5; row++){
            GPIOA->BSRR = 1 << (row+4);
            nano_wait(10000000);
            GPIOA->BRR |= 0x1E0;
            nano_wait(1000000);
        }
    }
}

void TIM2_IRQHandler() {
    if ((TIM2->SR & TIM_SR_UIF) != 0) {
        TIM2->SR &= ~TIM_SR_UIF;
        return;
    }

    if (TIM2->SR & TIM_SR_CC2IF) {
        switch(row) {
        case 1: press('1'); break;
        case 2: press('4'); break;
        case 3: press('7'); break;
        case 4: press('*'); break;
        }
    }

    if (TIM2->SR & TIM_SR_CC3IF) {
        switch(row) {
        case 1: press('2'); break;
        case 2: press('5'); break;
        case 3: press('8'); break;
        case 4: press('0'); break;
        }
    }

    if (TIM2->SR & TIM_SR_CC4IF) {
        switch(row) {
        case 1: press('3'); break;
        case 2: press('6'); break;
        case 3: press('9'); break;
        case 4: press('#'); break;
        }
    }

    nano_wait(10 * 1000 * 1000);
    while((GPIOA->IDR & 0xf) != 0);
    nano_wait(10 * 1000 * 1000);

    int __attribute((unused)) useless;
    useless = TIM2->CCR2;
    useless = TIM2->CCR3;
    useless = TIM2->CCR4;

    return;
}

void init_keypad(void) {
        RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
        GPIOA->MODER |= 0x15400;
        GPIOA->BRR |= 0x1E0;
        GPIOA->PUPDR &= ~(3<<2);
        GPIOA -> PUPDR &= ~(GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1 | GPIO_PUPDR_PUPDR2 | GPIO_PUPDR_PUPDR3);
        GPIOA -> PUPDR |= (GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1);
        GPIOA->PUPDR |= 2<<2;
        GPIOA->PUPDR |= 2<<4;
        GPIOA->PUPDR |= 2<<6;
        GPIOA->MODER |= 0xA8;
        GPIOA->AFR[0] |= 0x2220;

        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        TIM2->PSC = 0;
        TIM2->ARR = 0xffffffff;

        //channel 2
        TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
        TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
        TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
        TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;
        TIM2->CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP);
        TIM2->CCER |= TIM_CCER_CC2E;
        TIM2->DIER |= TIM_DIER_CC2IE;
        TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;
        TIM2->CCMR1 |= TIM_CCMR1_IC2F_3 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_0;

        //channel 3
        TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
        TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;
        TIM2->CCER &= ~(TIM_CCER_CC3P|TIM_CCER_CC3NP);
        TIM2->CCER |= TIM_CCER_CC3E;
        TIM2->DIER |= TIM_DIER_CC3IE;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
        TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;


        //channel 4
        TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
        TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;
        TIM2->CCER &= ~(TIM_CCER_CC4P|TIM_CCER_CC4NP); //unsure
        TIM2->CCER |= TIM_CCER_CC4E;
        TIM2->DIER |= TIM_DIER_CC4IE;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
        TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 | TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_0;

        TIM2->CR1 |= TIM_CR1_CEN;
        NVIC->ISER[0] = 1<<TIM2_IRQn;
        NVIC_SetPriority(TIM2_IRQn,2);
}

void init_RTC(){
    RCC -> APB1ENR |= RCC_APB1ENR_PWREN;
    PWR -> CR |= PWR_CR_DBP;
    RCC -> CR |= RCC_CR_HSEON; //RCC_CR_CSSON | RCC_CR_HSEON | RCC_CR_HSEBYP;
    while(!(RCC -> CR & RCC_CR_HSERDY)){}
    RCC -> CFGR =((RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_0);
    RCC -> BDCR |= RCC_BDCR_RTCSEL;
    RCC -> BDCR |= RCC_BDCR_RTCEN;

    RTC -> WPR = 0xCA;
    RTC -> WPR = 0x53;
    RTC -> CR |= RTC_CR_FMT;

    RTC -> ISR |= RTC_ISR_INIT;
    while((RTC -> ISR & RTC_ISR_INITF) != RTC_ISR_INITF){}
    RTC -> PRER &= ~(RTC_PRER_PREDIV_A | RTC_PRER_PREDIV_S);
    RTC -> PRER |= 10000-1;
    RTC -> PRER |= (25-1) << 16;
    RTC -> TR &= ~(RTC_TR_PM | RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU | RTC_TR_ST | RTC_TR_SU);
    RTC -> TR |= (RTC_TR_PM
            | RTC_TR_HT_0 | RTC_TR_HU_0                                     //11
            | RTC_TR_MNT_2 | RTC_TR_MNT_0 | RTC_TR_MNU_3 | RTC_TR_MNU_0     //59
            | RTC_TR_ST_2 | RTC_TR_SU_3 | RTC_TR_SU_0 );                    //49
    RTC -> ISR &= ~RTC_ISR_INIT;
    //RTC -> CR |= RTC_CR_BYPSHAD;
    RTC -> WPR = 0xFE;
    RTC -> WPR = 0x64;
}

void rtc_GetTime(char* timeStr){
    uint32_t hour =0, min =0, sec =0;
    RTC -> WPR = 0xCA;
    RTC -> WPR = 0x53;
    if((RTC->ISR & RTC_ISR_RSF)== RTC_ISR_RSF){
        hour = ((RTC->TR & RTC_TR_HT) >> 20) *10;
        hour += (RTC->TR & RTC_TR_HU) >> 16;
        min = ((RTC->TR & RTC_TR_MNT) >> 12) *10;
        min += (RTC->TR & RTC_TR_MNU) >> 8;
        sec = ((RTC->TR & RTC_TR_ST) >> 4) *10;
        sec += (RTC->TR & RTC_TR_SU) >> 0;
    }
    if (!(RTC->TR & RTC_TR_PM))
    {
        timeofday = 1;
    }
    else
    {
        timeofday = 0;
    }
    RTC -> WPR = 0xFE;
    RTC -> WPR = 0x64;
    sprintf(timeStr, "   %02d:%02d:%02d %s", hour, min, sec, (timeofday == 1 ? "AM" : "PM"));
}

void TIM3_IRQHandler(void)
{
    TIM3->SR &= ~TIM_SR_UIF;
    if(alarmSet == 1)
    {
        alarmON();
    }
    rtc_GetTime(timeArr);
    display1(timeArr);
}

void init_tim3(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 6- 1;
    TIM3->ARR = 6000000 - 1;
    TIM3->CR1 &= ~TIM_CR1_DIR;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn); /* (1) */
    NVIC_SetPriority(TIM3_IRQn,0); /* (2) */
}

void newReset(){
    setting = 2;
    alarmSet = 0;
    newE = 0;
    alarmSound = 0;
    cond = 0;
    onlyOnce = 0;
    noCopy = 0;
    numTry = 0;
    bzero(input_time, sizeof(input_time));
    display3("    WELCOME");
    display4("  * TO START");
}

char tep[16];
//This function is used for conditions when using the keypad for inputs
void press(char c) {
    static char settimeline[17];
    static char fline[17];
    static int offs = 0;
    if (alarmSound == 1){
        if (newE == 5){
            fline[3+offs] = c;
            if (offs < 2){
                fline[3 + offs + 1] = '_';
            }
            if (c != '*'){
                input[offs] = c;
                display4(fline);
                offs += 1;
            }
            else{
                 int j = 0;
                 while (j < 3)
                 {
                     if (input[j] == ans[j])
                     {
                         cond = 1;
                     }
                     else
                     {
                         cond = 0;
                         break;
                     }
                     j++;
                 }
                 if (cond == 1) {
                     display4("    CORRECT!");
                     stopAlarm();
                     nano_wait(5000000000);
                     newReset();
                     return;
                 }
                 else {
                     display4("                           ");
                     display4("   TRY AGAIN!");
                     offs = 0;
                     numTry++;
                     wrongAnswer(numTry);
                     bzero(input, sizeof(input));
                     newE = 4;
                     return;
                 }
            }
        }
        else if(newE == 4){
            if (c == '*'){
                strcpy(fline, "A: _         ");
                display4(fline);
                newE = 5;
            }
        }
        return;
    }
    else{
        if(newE == 0){
            if(c == '*'){
                newE = 1;
                display3("                ");
                display3("   SET ALARM?   ");
                display4(" YES: *   NO: #");
            }
            else {
                display3("    PRESS *");
                display4("   TO START");
            }
            return;
        }
        else if (newE == 1){
            if (c == '*'){
                offs = 0;
                newE = 2;
                display3("ENTER TIME:           ");
                if(alarmSet == 0){
                    display4("                      ");
                }
                else{
                    display4(input_time);
                }
            }
            else if(c == '#'){
                newReset();
            }
            return;
        }
        else if (newE == 2){
            display3("SET: * RETURN: #");
            settimeline[offs] = c;
            if (offs < 8)
                settimeline[offs+1] = '_';

            if(c == '#'){
                newReset();
            }
            else{
                if(c != '*'){
                    input_time[offs] = c;
                    display4(input_time);
                }
                else{
                    newE = 3;
                    display3("  AM: *   PM: #  ");
                    display4("                 ");
                }
            }
            offs += 1;
            return;
        }
        else if (newE == 3){
            if (c == '*') {
                setting = 1;
                offs = 0;
                alarmSet = 1;
                display3(" SET!! (DEL: 0)");
                bzero(tep, sizeof(tep));
                strcpy(tep,"   ");
                tep[3] = input_time[0];
                tep[4] = input_time[1];
                tep[5] = ':';
                tep[6] = input_time[2];
                tep[7] = input_time[3];
                tep[8] = ':';
                tep[9] = '0';
                tep[10] = '0';
                strcat(tep," AM");
                display4(tep);
            }
            else if (c == '#')
            {
                setting = 0;
                offs = 0;
                alarmSet = 1;
                display3(" SET!! (DEL: 0)");
                bzero(tep, sizeof(tep));
                strcpy(tep,"   ");
                tep[3] = input_time[0];
                tep[4] = input_time[1];
                tep[5] = ':';
                tep[6] = input_time[2];
                tep[7] = input_time[3];
                tep[8] = ':';
                tep[9] = '0';
                tep[10] = '0';
                strcat(tep," PM");
                display4(tep);
            }
            else if (c == '0'){
                newReset();
            }
            return;
        }
    }
}


void reset(char* ans)
{
    int i = 0;
    while (i < 3)
    {
        ans[i] = '\0';
        i++;
    }
}

int main(void) {

    init_lcd();
    init_lcd2();
    //init_pwm();
    init_keypad();
    display3("    WELCOME");
    display4("  * TO START");
    init_RTC();
    init_tim3();
    scan_keypad();

    for(;;) asm("wfi");
}
