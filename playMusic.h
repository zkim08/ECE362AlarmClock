#ifndef PLAYMUSIC_H_
#define PLAYMUSIC_H_

void init_wavetable(void);
void set_tim6();
void set_gpioA();
void set_dac();
void note_control();
void noteChange();
void checkPoint(int answer);
void stopAlarm();
void playMusic(void);
void TIM6_DAC_IRQHandler();


#endif /* PLAYMUSIC_H_ */
