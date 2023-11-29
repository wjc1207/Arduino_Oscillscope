/*
 * ADC_test.c
 *
 * Created: 2023/11/15 16:31:30
 * Author : 16425
 */ 
#define F_CPU 16000000UL
#define BAUD 38400
#define UBRR_VAL ((F_CPU/(16UL*BAUD))-1)
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>



void adc_init(uint8_t channel)
{
	ADMUX = 1 << REFS0 | channel;
	ADCSRA |= 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0; //prescaler clk/128
	ADCSRA |= 1 << ADEN; //ADC enable //no ADC interrupt enable
}

uint16_t adc_read()
{
	uint16_t ADCValue;
	ADCSRA |= 1 << ADSC; //start conversion
	while (ADCSRA & (1 << ADSC));//wait conversion complete
	uint8_t lowADCValue = ADCL;
	uint8_t highADCValue = ADCH;
	ADCValue = lowADCValue | highADCValue << 8; //read adc value
	return ADCValue;
}

void uart_init()
{
	UBRR0L = (uint8_t)UBRR_VAL;
	UBRR0H = (uint8_t)UBRR_VAL >> 8;
	UCSR0B |= 1 << TXEN0 | 1 << RXEN0 | 1 << RXCIE0; //transmit enable, receive enable
	UCSR0C = (3 << UCSZ00);
}

void uart_send(char data)
{
	UDR0 = data;
	while (!(UCSR0A & (1 << TXC0)));
	UCSR0A |= 1 << TXC0; //write one to clear it
}

void delay(uint16_t delay_time) //f = 16MHz/256, T = delay_time * 64 * T0
{
	TCCR0B |= 0 << WGM02;
	TCCR0A |= 1 << WGM01 | 0 << WGM00;//set clock as Compare Match, f = fc/(N)
	TCCR0B |= 0 << CS02 | 0 << CS01 | 0 << CS00; //stop the timer
	TCNT0 = 0; //clear the timer
	OCR0A = 64; //T = 1.024ms
 	for (uint16_t i = 0; i < delay_time; i++)
	{
		TCCR0B |= 1 << CS02 | 0 << CS01 | 0 << CS00; //start the timer, N = 256
		while(!(TIFR0 & 1 << OCF0A)) {}
		TIFR0 |= 1 << OCF0A; // clear flag
		TCNT0 = 0; //clear the timer
	}

}


int main(void)
{
	uart_init();
	adc_init(0);
	delay(100);
	sei();
    
    while (1) 
    {
		uint16_t adc_value;
		adc_value = adc_read();
		char buffer[20];  // Adjust the buffer size as needed
		// Using sprintf to convert uint16_t to char*
		sprintf(buffer, "%u", adc_value);
		int i = 0;
		while (buffer[i] != '\0')
		{
			uart_send(buffer[i]);
			i++;
		}
		uart_send('\n');
		delay(5); //delay 5ms, Fs = 200Hz
			
    }
	
}

ISR(USART_RX_vect)
{
	if (UCSR0A & (1 << RXC0)) // Check if receive complete flag is set
	{
		char receive_data = UDR0;
		adc_init((uint8_t)receive_data - (uint8_t)48);
	}
}
//restart bug - ADC interrupt is enabled but should not be enabled
//the Receive Complete Interrupt isn't triggered normally - the mistake cli();