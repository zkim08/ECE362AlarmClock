#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "playMusic.h"
#include <math.h>

#define N 1000
#define RATE 100000

int offset = 0;
int cnt = 0;
float noteFreq = 293.66; //D
int step = 293.66 * N / RATE * (1 << 16);
short int wavetable[N];
int limit = 100000;
int rest = 0;
int musicCase = 0;
int octaveUp = 0;


void init_wavetable(void)
{
  int x;
  for(x = 0; x < N; x++)
    wavetable[x] = 32767 * sin(2 * M_PI * x / N);
}

void set_tim6(){
    RCC -> APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6 -> CR1 &= ~TIM_CR1_CEN;
    TIM6 -> CR1 &= ~TIM_CR1_DIR;
    TIM6 -> PSC = 48 -1;
    TIM6 -> ARR = 10 -1;
    TIM6 -> DIER |= TIM_DIER_UIE;
    TIM6 -> CR1 |= TIM_CR1_CEN;
    NVIC -> ISER[0] |= 1 << TIM6_DAC_IRQn;
}

void set_DAC() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA -> MODER |= GPIO_MODER_MODER4;

    RCC -> APB1ENR |= RCC_APB1ENR_DACEN;
    DAC -> CR &= ~DAC_CR_EN1;
    DAC -> CR &= ~DAC_CR_BOFF1;
    DAC -> CR |= DAC_CR_TEN1;
    DAC -> CR |= DAC_CR_TSEL1;
    DAC -> CR |= DAC_CR_EN1;
}




/*
void note_control(){
    if(cnt < limit){
        cnt++;
    }
    else{
        if (noteFreq < 300 && noteFreq > 290){      //D -> D#
            noteFreq = 311.13;
            step = noteFreq * N / RATE * (1 << 16);
        }
        else if (noteFreq < 315 && noteFreq > 310){ //D# ->
            //if()
            noteFreq = 293.66;
            step = noteFreq * N / RATE * (1 << 16);
        }
        offset = 0;
        limit = limit/1.1;
        cnt = 0;
    }
}
*/

void noteChange(){
    if(limit == 100000){
        if(cnt < limit){
            cnt++;
        }
        else{
            if (noteFreq < 300 && noteFreq > 290){      //D -> D#
                noteFreq = 311.13;
                step = noteFreq * N / RATE * (1 << 16);
            }
            else if (noteFreq < 315 && noteFreq > 310){ //D# -> none
                noteFreq = 0;
                step = noteFreq * N / RATE * (1 << 16);
            }
            else if (noteFreq < 10 && rest%2 == 0){
                rest++;
            }
            else if (noteFreq < 10 && rest%2 == 1){
                noteFreq = 293.66;
                step = noteFreq * N / RATE * (1 << 16);
                rest++;
                if(rest == 4){
                    limit = limit/2;
                    rest = 0;
                }
            }
            offset = 0;
            cnt = 0;
        }
    }
    else if(limit == 50000){
        if(cnt < limit){
            cnt++;
        }
        else{
            if (noteFreq < 300 && noteFreq > 290){      //D -> D#
                noteFreq = 311.13;
                step = noteFreq * N / RATE * (1 << 16);
            }
            else if (noteFreq < 315 && noteFreq > 310){ //D# -> D
                noteFreq = 293.66;;
                step = noteFreq * N / RATE * (1 << 16);
                rest++;
                if(rest == 4){
                    limit = limit/2;
                    rest = 0;
                }
            }
            offset = 0;
            cnt = 0;
        }
    }
    else if(limit == 25000 && octaveUp == 0){
        if(cnt < limit){
            cnt++;
        }
        else{
            if (noteFreq < 300 && noteFreq > 290){      //D -> D#
                noteFreq = 311.13;
                step = noteFreq * N / RATE * (1 << 16);
            }
            else if (noteFreq < 315 && noteFreq > 310){ //D# -> D
                noteFreq = 293.66;
                step = noteFreq * N / RATE * (1 << 16);
            }
            offset = 0;
            cnt = 0;
            /*
            if (rest == 8){
                octaveUp++;
                limit = limit/2;
                rest =0;
            }
            */
        }
    }
    else if(limit == 12500 && octaveUp == 1){
        if(cnt < limit){
            cnt++;
        }
        else{
            if (noteFreq < 600 && noteFreq > 580){      //D -> D#
                noteFreq = 622.25;
                step = noteFreq * N / RATE * (1 << 16);
            }
            else if (noteFreq > 610){ //D# -> D
                noteFreq = 587.33;
                step = noteFreq * N / RATE * (1 << 16);
            }
            offset = 0;
            cnt = 0;
            /*
            if (rest == 16){
                octaveUp++;
            }
            */
        }
    }
    else if(limit == 12500 && octaveUp == 2){
        if(cnt < limit){
            cnt++;
        }
        else{
            if (noteFreq < 1230){      //D -> D#
                noteFreq = 1244.51;
                step = noteFreq * N / RATE * (1 << 16);
            }
            else{ //D# -> D
                noteFreq = 1174.66;
                step = noteFreq * N / RATE * (1 << 16);
            }
            offset = 0;
            cnt = 0;
        }
    }
    else{
        if(cnt < limit){
            cnt++;
        }
        else{
            if (musicCase == 0){      //C -> D
                noteFreq = 1174.66;
                step = noteFreq * N / RATE * (1 << 16);
                musicCase = 1;
            }
            else if (musicCase == 1){      //D -> E
                noteFreq = 1318.51;
                step = noteFreq * N / RATE * (1 << 16);
                musicCase = 2;
            }
            else if (musicCase == 2){      //E -> G
                noteFreq = 1567.98;
                step = noteFreq * N / RATE * (1 << 16);
                musicCase = 3;
            }
            else if (musicCase == 3){      //G -> A
                noteFreq = 1760;
                step = noteFreq * N / RATE * (1 << 16);
                musicCase = 4;
            }
            else if (musicCase == 4){      //A -> G
                noteFreq = 1567.98;
                step = noteFreq * N / RATE * (1 << 16);
                musicCase = 5;
            }
            else if (musicCase == 5){      //G -> C
                noteFreq = 1046.5;
                step = noteFreq * N / RATE * (1 << 16);
                musicCase = 0;
            }
            cnt = 0;
        }
    }
}

void wrongAnswer(int answer){
    octaveUp = answer;
    if(answer == 1){
        limit = 12500;
        noteFreq = 587.33;
        step = noteFreq * N / RATE * (1 << 16);
    }
    else if (answer == 2){
        noteFreq = 1174.66;
        step = noteFreq * N / RATE * (1 << 16);
    }
    else if (answer == 2){
        noteFreq = 1046.5;
        step = noteFreq * N / RATE * (1 << 16);
    }
}

void stopAlarm(){
	DAC -> CR &= ~DAC_CR_EN1;
	GPIOA -> MODER &= ~GPIO_MODER_MODER4;
	RCC -> APB1ENR &= ~RCC_APB1ENR_DACEN;
}

void TIM6_DAC_IRQHandler(){
    DAC -> SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
    TIM6 -> SR &= ~TIM_SR_UIF;
    noteChange();
    offset += step;
    if((offset >> 16) >= N){
        offset -= N << 16;
    }
    int sample = wavetable[offset >> 16];
    sample = sample / 16 +2048;
    if (sample > 4095){
        sample = 4095;
    }
    else if (sample < 0){
        sample = 0;
    }
    DAC -> DHR12R1 = sample;
}


void playMusic(void)
{
    offset = 0;
    cnt = 0;
    noteFreq = 293.66; //D
    step = 293.66 * N / RATE * (1 << 16);
    limit = 100000;
    rest = 0;
    octaveUp = 0;
    musicCase = 0;
    init_wavetable();
    set_DAC();
    set_tim6();
}
