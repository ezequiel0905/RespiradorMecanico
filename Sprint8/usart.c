
/*
 * usart.c
 *
 * Created: 03/05/2021 08:18:06
 *  Author: ezequ
 */ 
#include "usart.h"
#include <stdlib.h>

unsigned char pressao_arterial[8]= "HHHxLLL";
unsigned char altura[4] = "A,AA", peso[4]= "PP,P";
int16_t alturaI, alturaD;
int16_t pressao_H, pressao_L;

ISR(USART_RX_vect){
	static uint8_t flag =0, index = 0, indexA = 0, flagA = 0, flagP=0, indexP=0;
	char recebido = UDR0;
	
	
	switch(recebido)
	{
	case ';':
		flag = 1;
		index = 0;
		break;
	case '{':
		flagA= 1;
		indexA = 0;
		break;
	case '[':
		flagP=1;
		indexP=0;
		break;
	case ':':
		flag=0;
		pressao_arterial[index]='\0';
		if(Check_Faixa_HHHxLLL(pressao_arterial,&pressao_H, &pressao_L)==0)
		strcpy(pressao_arterial,"ERRO!");
		break;
	case '}':
		flagA =0;
		altura[indexA]='\0';
		break;
	case ']':
		flagP =0;
		peso[indexP]='\0';
		break;
	default:
		if (flagP)
		{
			if (indexP<4)
			{
				
				peso[indexP++]=recebido;
			}
			else
			{
				
				flagP=0;
				strcpy(peso,"ERRO!");

			}
		}
		if (flag)
		{
			if (index<7)
			{
				pressao_arterial[index++]=recebido;
				
			}
			else
			{
				flag=0;
				
				strcpy(pressao_arterial,"ERRO!");
				

			}
		}
		if (flagA)
		{
			if (indexA<4)
			{
				
				altura[indexA++]=recebido;
				
			}
			else
			{
				
				flagA = 0;
				
				strcpy(altura,"ERRO!");
				

			}
		}
	}
}

void USART_Inicio(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
	
	DDRC |= 0b00000010;
	DDRC &= 0b11111110;
	
}
void USART_Emitir(unsigned char data)
{
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0 = data;		
}
unsigned char USART_Recebe(void)
{
	while(!(UCSR0A&(1<<RXC0)));
	return UDR0;
}

uint8_t Check_Faixa_HHHxLLL(char *check, int16_t *HHH, int16_t *LLL)
{
	unsigned char Pressao_H[8], Pressao_L[8];
	char *split, *aux;
	
	aux = strupr(check);
	split=strsep(&aux, "x");
	strcpy(Pressao_H,split);
	split = strsep(&aux,"x");
	strcpy(Pressao_L,split);
	
	*HHH = atoi(Pressao_H);
	*LLL = atoi(Pressao_L);
	
	if (*HHH>=0 & *HHH<=999 & *LLL>=0 & *LLL<=999){
		return 1;
	}else{
		return 0;
	}
}
void Check_Altura(char *check, int16_t *inteira, int16_t *decimal){
	unsigned char alturaI[1], alturaD[2];
	char *split, *aux;
	aux = strupr(check);
	split=strsep(&aux, ",");
	strcpy(alturaI,split);
	split = strsep(&aux,",");
	strcpy(alturaD,split);
	
	*inteira = atoi(alturaI);
	*decimal = atoi(alturaD);
	
	
}