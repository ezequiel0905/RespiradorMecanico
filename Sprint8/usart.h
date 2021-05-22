
/*
 * usart.h
 *
 * Created: 03/05/2021 08:17:46
 *  Author: ezequ
 */ 
#ifndef USARTLIB_H_
#define USARTLIB_H_
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>


void USART_Inicio(unsigned int ubrr);
void USART_Emitir(unsigned char data);
unsigned char USART_Recebe(void);
uint8_t Check_Faixa_HHHxLLL(char *payload, int16_t *HHH,  int16_t *LLL);

extern unsigned char pressao_arterial[8];
extern int16_t pressao_H, pressao_L;

#endif
