/*
 * Mini_Project_2.c
 *
 *  Created on: Sep 12, 2023
 *      Author: omars
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char counter[6]={0};  /*{SEC1,SEC2,MIN1,MIN2,HR1,HR2}*/
unsigned char loops_counter;   /*USED FOR ANY LOOPS*/
unsigned char flag=0;		   /*Flag indicates that an interrupt has happened*/
/*------------------- Functions and ISR for timer -------------------*/

void Timer1_CTC(void){
	TCCR1A=(1<<FOC1A);  /*Non-PWM mode*/
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11);/*F(timer)=F(cpu)/64*/
	TCNT1=0;   /*Initial Value*/
	OCR1A=15625; /*To count 1 Second*/
	TIMSK|=(1<<OCIE1A); /*Enable interrupt for timer 1*/
}
void time_counter(void) {
	counter[0]++;
	if (counter[0] == 10) {
		counter[0] = 0;
		counter[1]++;
		if (counter[1] == 6) {
			counter[1] = 0;
			counter[2]++;
			if (counter[2] == 10) {
				counter[2] = 0;
				counter[3]++;
				if (counter[3] == 6) {
					counter[3] = 0;
					counter[4]++;
					if (counter[4] == 10) {
						counter[4] = 0;
						counter[5]++;

					}
				}
			}
		}
	}
}
ISR(TIMER1_COMPA_vect){
	flag=1;
}
/*------------------- Functions and ISR for Interrupt 0 -------------------*/

void INT_0(void){
	DDRD&=~(1<<PD2);
	PORTD|=(1<<PD2);
	MCUCR=(1<<ISC01); /*Falling edge*/
	GICR|=(1<<INT0);
}

void reset(void){
	for(loops_counter=0;loops_counter<6;loops_counter++){
			counter[loops_counter]=0;
		}
	_delay_ms(3);
}

ISR(INT0_vect){
	flag=2;
}
/*------------------- Functions and ISR for Interrupt 1 -------------------*/

void INT_1(void){
	DDRD&=~(1<<PD3);
	MCUCR=(1<<ISC10)|(1<<ISC11); /*Raising Edge*/
	GICR|=(1<<INT1);
}
ISR(INT1_vect){
	TCCR1B=0; /*Disconnect the clock*/
}
/*------------------- Functions and ISR for Interrupt 2 -------------------*/

void INT_2(void){
	DDRB&=~(1<<PB3);
	PORTB|=(1<<PB3);
	MCUCSR&=~(1<<ISC2); /*Falling edge*/
	GICR|=(1<<INT2);
}
ISR(INT2_vect){
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11);  /*Connect the clock*/
}
/*------------------- Main Program -------------------*/
int main(void){
	Timer1_CTC();
	INT_0();
	INT_1();
	INT_2();
	DDRC|=0x0F;
	PORTC&=0xF0;
	DDRA|=0x3F;
	PORTA&=0xC0;
	SREG|=(1<<7); /*Enable I-bit*/
	while(1){
		PORTC=(PORTC&0xF0)|(counter[0]&0x0F);
		PORTA|=(1<<PA0);
		_delay_ms(3);
		PORTA&=~(1<<PA0);
		PORTC=(PORTC&0xF0)|(counter[1]&0x0F);
		PORTA|=(1<<PA1);
		_delay_ms(3);
		PORTA&=~(1<<PA1);
		PORTC=(PORTC&0xF0)|(counter[2]&0x0F);
		PORTA|=(1<<PA2);
		_delay_ms(3);
		PORTA&=~(1<<PA2);
		PORTC=(PORTC&0xF0)|(counter[3]&0x0F);
		PORTA|=(1<<PA3);
		_delay_ms(3);
		PORTA&=~(1<<PA3);
		PORTC=(PORTC&0xF0)|(counter[4]&0x0F);
		PORTA|=(1<<PA4);
		_delay_ms(3);
		PORTA&=~(1<<PA4);
		PORTC=(PORTC&0xF0)|(counter[5]&0x0F);
		PORTA|=(1<<PA5);
		_delay_ms(3);
		PORTA&=~(1<<PA5);

		if(flag==1){
			flag=0;
			time_counter();
		}
		if(flag==2){
			flag=0;
			reset();
		}

	}
}

