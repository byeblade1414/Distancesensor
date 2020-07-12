#include <avr/io.h>
#include <avr/interrupt.h>
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "USART.h"
#include <stdio.h>


#include <util/delay.h>
#include <avr/interrupt.h>

#define ECHO 0

volatile int overflow;
volatile unsigned char timerValue;

volatile int s = 0;


void initHc04();
void sendPulse();
void initTimer();
void initExtInt0();

float measureDistance();



int main(void)
{

	
	initHc04();
	USART_init();
	char buffer[20];
	float distance;
	
	while (1)
	{

		distance = measureDistance();
		snprintf(buffer,20, "distance = %.2f", distance);
		USART_putstring(buffer);
		USART_send('\n');
		_delay_ms(250);
	}
}

void initHc04()
{
	DDRB |= (1 << ECHO);
	initExtInt0();
	sei();	
}


void sendPulse()
{
	PORTB |= (1 << ECHO);
	_delay_us(20);
	PORTB &= ~(1 << ECHO);
}

void initTimer()
{
	TCCR0 |= (1 << CS01);
	TIMSK |= (1 << TOIE0);
}

void stopTimer()
{
	TCCR0 = 0x00;
	TCNT0 = 0x00;
	
}

void initExtInt0()
{
	MCUCR |=  (1 << ISC00);
	GICR  |=  (1 << INT0);
}

ISR(INT0_vect)
{
	if(s == 0)
	{
		s = 1;
		initTimer();
	}
	else if(s == 1)
	{
		s = 0;
		timerValue = TCNT0;
		stopTimer();
	}
}


void blink()
{
	DDRA = 0xff;
	
	PORTA = 0xff;
	_delay_ms(100);
	PORTA = 0x00;
}

ISR(TIMER0_OVF_vect)
{
	overflow++;
}


float measureDistance()
{
	sendPulse();
	_delay_ms(1);
	float distance = (float)((0.01*(timerValue + 256*overflow)));
	timerValue = 0;
	overflow = 0;
	_delay_ms(1);	
	return distance;
}
