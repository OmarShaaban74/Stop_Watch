#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void reset(void);
enum counters{SEC1,SEC2,MIN1,MIN2,HR1,HR2};
unsigned char tick[6]={0};  /*{SEC1,SEC2,MIN1,MIN2,HR1,HR2}*/
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
	tick[SEC1]++;
	if (tick[SEC1] == 10) {
		tick[SEC1] = 0;
		tick[SEC2]++;
		if (tick[SEC2] == 6) {
			tick[SEC2] = 0;
			tick[MIN1]++;
			if (tick[MIN1] == 10) {
				tick[MIN1] = 0;
				tick[MIN2]++;
				if (tick[MIN2] == 6) {
					tick[MIN2] = 0;
					tick[HR1]++;
					if (tick[HR1] == 10) {
						tick[HR1] = 0;
						tick[HR2]++;
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
	MCUCR|=(1<<ISC01); /*Falling edge*/
	GICR|=(1<<INT0);
}

void reset(void){
	for(loops_counter=0;loops_counter<6;loops_counter++){
		tick[loops_counter]=0;
		}
	TCNT1=0;
}

ISR(INT0_vect){
	flag=2;
}
/*------------------- Functions and ISR for Interrupt 1 -------------------*/

void INT_1(void){
	DDRD&=~(1<<PD3);
	MCUCR|=(1<<ISC10)|(1<<ISC11); /*Raising Edge*/
	GICR|=(1<<INT1);
}
ISR(INT1_vect){
	TCCR1B&=~(1<<CS10)&~(1<<CS11); /*Disconnect the clock*/
}
/*------------------- Functions and ISR for Interrupt 2 -------------------*/

void INT_2(void){
	DDRB&=~(1<<PB2);
	PORTB|=(1<<PB2);
	MCUCSR&=~(1<<ISC2); /*Falling edge*/
	GICR|=(1<<INT2);
}
ISR(INT2_vect){
	TCCR1B|=(1<<CS10)|(1<<CS11);  /*Connect the clock*/
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
		PORTC=(PORTC&0xF0)|(tick[SEC1]&0x0F);
		PORTA|=(1<<PA0);
		_delay_ms(3);
		PORTA&=~(1<<PA0);
		PORTC=(PORTC&0xF0)|(tick[SEC2]&0x0F);
		PORTA|=(1<<PA1);
		_delay_ms(3);
		PORTA&=~(1<<PA1);
		PORTC=(PORTC&0xF0)|(tick[MIN1]&0x0F);
		PORTA|=(1<<PA2);
		_delay_ms(3);
		PORTA&=~(1<<PA2);
		PORTC=(PORTC&0xF0)|(tick[MIN2]&0x0F);
		PORTA|=(1<<PA3);
		_delay_ms(3);
		PORTA&=~(1<<PA3);
		PORTC=(PORTC&0xF0)|(tick[HR1]&0x0F);
		PORTA|=(1<<PA4);
		_delay_ms(3);
		PORTA&=~(1<<PA4);
		PORTC=(PORTC&0xF0)|(tick[HR2]&0x0F);
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
		if(tick[HR2]==10){
			reset();
		}
}

