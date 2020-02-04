#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

//#define SCL 1
//#define SDI 2
//#define SS 4
#define SPI_DELAY 400

void nano_wait(int t);

static void sendbit(int b)
{
    const int SCK = 1<<3;
    const int MOSI = 1<<5;
    // We do this slowly to make sure we don't exceed the
    // speed of the device.
    GPIOB->BRR = SCK;
    if (b)
        GPIOB->BSRR = MOSI;
    else
        GPIOB->BRR = MOSI;
    //GPIOA->BSRR = b ? MOSI : (MOSI << 16);
    nano_wait(SPI_DELAY);
    GPIOB->BSRR = SCK;
    nano_wait(SPI_DELAY);
}

static void sendbyte(char b)
{
    int x;
    // Send the eight bits of a byte to the SPI channel.
    // Send the MSB first (big endian bits).
    for(x=8; x>0; x--) {
        sendbit(b & 0x80);
        b <<= 1;
    }
}

static void cmd(char b)
{
//    GPIOC-> BRR = SS;
//    nano_wait(SPI_DELAY);
//    sendbit(0);
//    sendbit(0);
//    sendbyte(b);
//    nano_wait(SPI_DELAY);
//    GPIOC->BSRR = SS;
//    nano_wait(SPI_DELAY);
    const int NSS = 1<<15;
    GPIOA->BRR = NSS; // NSS low
    nano_wait(SPI_DELAY);
    sendbit(0); // RS = 0 for command.
    sendbit(0); // R/W = 0 for write.
    sendbyte(b);
    nano_wait(SPI_DELAY);
    GPIOA->BSRR = NSS; // set NSS back to high
    nano_wait(SPI_DELAY);
}

static void data(char b)
{
//    GPIOC->BRR = SS;
//    nano_wait(SPI_DELAY);
//    sendbit(1);
//    sendbit(0);
//    sendbyte(b);
//    nano_wait(SPI_DELAY);
//    GPIOC->BSRR = SS;
//    nano_wait(SPI_DELAY);
    const int NSS = 1<<15;
    GPIOA->BRR = NSS; // NSS low
    nano_wait(SPI_DELAY);
    sendbit(1); // RS = 1 for data.
    sendbit(0); // R/W = 0 for write.
    sendbyte(b);
    nano_wait(SPI_DELAY);
    GPIOA->BSRR = NSS; // set NSS back to high
    nano_wait(SPI_DELAY);
}

void generic_lcd_startup(void) {
    nano_wait(100000000); // Give it 100ms to initialize
    cmd(0x38);  // 0011 NF00 N=1, F=0: two lines
    cmd(0x0c);  // 0000 1DCB: display on, no cursor, no blink
    cmd(0x01);  // clear entire display
    nano_wait(6200000); // clear takes 6.2ms to complete
    cmd(0x02);  // put the cursor in the home position
    cmd(0x06);  // 0000 01IS: set display to increment
}

void init_lcd(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOA->BSRR = 1<<15; // set NSS high
    GPIOB->BRR = (1<<3) + (1<<5); // set SCK and MOSI low
    // Now, configure pins for output.
    GPIOA->MODER &= ~(3<<(2*15));
    GPIOA->MODER |=  (1<<(2*15)); // Setting output for PA15 (spi1NSS)
    GPIOB->MODER &= ~( (3<<(2*3)) | (3<<(2*5)) );
    GPIOB->MODER |=    (1<<(2*3)) | (1<<(2*5)); // Setting output for PB3 (spi1SCK) and PB5 (spi1MOSI)

    generic_lcd_startup();
}

void display3(const char *s)
{
//    cmd(0x02);
//    int len;
//    for(len=0; len<16; len += 1)
//    {
//        if(s[len] == '\0')
//            break;
//        data(s[len]);
//    }
    // put the cursor on the beginning of the first line (offset 0).
    cmd(0x80 + 0);
    int x;
    for(x=0; x<16; x+=1)
        if (s[x])
            data(s[x]);
        else
            break;
    for(   ; x<16; x+=1)
        data(' ');
}

void display4(const char *s)
{
//    cmd(0xc0);
//    int len;
//    for(len=0; len<16; len += 1)
//    {
//        if(s[len] == '\0')
//            break;
//        data(s[len]);
//    }
    // put the cursor on the beginning of the second line (offset 64).
    cmd(0x80 + 64);
    int x;
    for(x=0; x<16; x+=1)
        if (s[x] != '\0')
            data(s[x]);
        else
            break;
    for(   ; x<16; x+=1)
        data(' ');
}

void init_pwm(void) {
    GPIOA->MODER |= 0x2a0000;
    GPIOA->AFR[1] &= ~(0xf<<(4*0));
    GPIOA->AFR[1] |= 0x2<<(4*0);
    GPIOA->AFR[1] &= ~(0xf<<(4*1));
    GPIOA->AFR[1] |= 0x2<<(4*1);
    GPIOA->AFR[1] &= ~(0xf<<(4*2));
    GPIOA->AFR[1] |= 0x2<<(4*2);

    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->PSC = 1000;
    TIM1->ARR = 100 - 1;
    TIM1->CCR1 = 99;
    TIM1->CCR2 = 99;
    TIM1->CCR3 = 99;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
    TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
    TIM1->CCER |= TIM_CCER_CC1E;
    TIM1->CCER |= TIM_CCER_CC2E;
    TIM1->CCER |= TIM_CCER_CC3E;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
//  TIM1->EGR |= TIM_EGR_UG;
}

void update_freq(int freq) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->PSC = (48000000 / (freq * 100)) - 1.0;
}

void update_rgb(int r, int g, int b) {
    TIM1->CCR1 = r;
    TIM1->CCR2 = g;
    TIM1->CCR3 = b;
}
