/*
 * 
 *
 * Created: 20/03/2021 22:03:24
 * Author : Ezequiel
 */ 

//INCLUSÃO DAS BIBLIOTECAS
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nokia5110.h"
#include "usart.h"

//DECLARAÇÃO DE VARIÁVEIS GLOBAIS
uint8_t flag_200ms =0, flag_resp_por16=0, flag_150ms =0;
uint8_t sel = 0;
uint16_t freq_card = 0;
uint8_t respPorMinuto = 5;
uint32_t tempo_ms=0;  
uint8_t  temperatura=0;
uint16_t  oxigenacao=0;
uint16_t oxigenio=2000;
uint16_t porcentOxi=0;
uint16_t vol=0;
//PROTÓPICOS DOS MÉTODOS E FUNÇÕES
void RespiraLeds(uint8_t *flag_disparo);
void LeituraOT(uint8_t *flag_disparo);
void MostrarNokia(uint8_t *flag_disparo,uint8_t freq_resp,uint16_t freqcard,float temp_paciente, uint16_t ox_paciente, uint16_t oxi_paciente,uint16_t vol_resp);


//ALTERAÇÃO NAS RESPIRAÇÕES POR MINUTOS QUANDO APERTAR OS BOTÕES
ISR(INT0_vect){
	
	if(respPorMinuto<30 && oxigenio<4000 && vol<8){
		switch(sel)
		{
			case 1:
				respPorMinuto++;
				break;
			case 2:
				oxigenio = oxigenio+200;
				porcentOxi += 10;
				OCR1B = oxigenio;
				break;
			case 3:
				vol++;	
				break;
		}
	}
}
ISR(INT1_vect){
	
	if(respPorMinuto>5 && oxigenio>2000 && vol>=0 ){
		switch(sel)
		{
			case 1:
				respPorMinuto--;
				break;
			case  2:
				oxigenio = oxigenio-200;
				porcentOxi -= 10;
				OCR1B = oxigenio;
				break;
			case 3:
				vol--;
				break;
		}	
	}
}
ISR(PCINT0_vect){
	static uint8_t flagSobe = 0;
	if(flagSobe)
	{
		if(sel<3)
			sel++;
		else
			sel = 0;
	}
	flagSobe = !flagSobe; 
	
}
ISR(TIMER0_COMPA_vect){
	
	tempo_ms++;
	PORTD ^= 0b00000001;
	if((tempo_ms%200)==0)
		flag_200ms=1;
	if((tempo_ms%(3750/respPorMinuto))==0)
		flag_resp_por16=1;
	if((tempo_ms%150)==0){
		flag_150ms=1;
	}
		
}
ISR(PCINT2_vect){
	static uint32_t tempo_ms_freqcard_anterior=0;
	freq_card=6000/((tempo_ms -tempo_ms_freqcard_anterior)*2);
	tempo_ms_freqcard_anterior=tempo_ms;

}

int main(void)
{
	//ALTERANDO OS PINOS GPIO
	 DDRB  = 0b10111111; // COMO SAÍDAS
	 PORTB = 0b01000000;
	 DDRC =  0b11111100;
	 DDRD =  0b10000000; // COMO ENTRADAS menos o D0
	 PORTD = 0b11111110; //HABILITANDO todos o pull-ups menos o D0
	 
	 //Configuração das Interrupções
	 EICRA=  0b00001010;//INTERRUPÇÕES SENSÍVEIS A BORDA DE DESCIDA
	 EIMSK=  0b00000011;//HABILITA AS INTERRUPÇÕES EXTERNAS (INT0, INT1)
	 PCICR = 0b00000101;  //habilita o pino de interrupção 2 PCINT2 e PCINT0
	 PCMSK2= 0b00100000; // habilita interrupt pnchange17, ou seja PD6
	 PCMSK0= 0b01000000; // habila interrupt PB6
	 
	 //Configuração do Timer 0
	 TCCR0A = 0b00000010;
	 TCCR0B = 0b00000011;
	 OCR0A = 249;
	 TIMSK0 = 0b00000010;
	 
	 //Configuaração do Timer 1
	 ICR1 = 39999;
	 TCCR1A = 0b10100010;
	 TCCR1B = 0b00011010;
	 OCR1A = 2000;
	 OCR1B = 2000;
	 
	 //Configuração do ADC
	 ADMUX = 0b01000000;
	 ADCSRA= 0b11100111;
	 ADCSRB= 0b00000000;
	 DIDR0 = 0b00000011;
	 
	 sei(); //HABILITA INTERRUPÇÕES GLOBAIS
	
	 
	 nokia_lcd_init();
	 
	
    /* Replace with your application code */
	USART_Inicio(MYUBRR);
    while (1) 
    {
		LeituraOT(&flag_150ms);
		RespiraLeds(&flag_resp_por16);
		MostrarNokia(&flag_200ms,respPorMinuto,freq_card, temperatura,oxigenacao,porcentOxi,vol);
		
		
    }
}

void RespiraLeds (uint8_t *flag_disparo){
	static uint32_t led_aceso=2000,desce=1;
	OCR1A = led_aceso;
	
	if(*flag_disparo){
		if (desce)
		{
			if(led_aceso==2000)
			{
				desce=0;
				led_aceso=2000;
			}
			else
			{
				led_aceso= led_aceso-250;
			}
		}
		else
		{
			if (led_aceso==((vol*250)+2000))
			{
				desce=1;
				led_aceso=((vol*250)+2000);
			}
			else
			{
				led_aceso = led_aceso+250;
			}
		}	
		*flag_disparo=0;
	}
}

void MostrarNokia(uint8_t *flag_disparo,uint8_t freq_resp,uint16_t freqcard, float temp_paciente, uint16_t ox_paciente, uint16_t oxi_paciente,uint16_t vol_resp){

	unsigned char oxi_string[3];
	unsigned char vol_string[3];
	unsigned char ox_string[3];
	unsigned char temp_string[5];
	unsigned char freqResp_string[3];
	unsigned char freqCard_string[4];
	
	if(*flag_disparo){

		nokia_lcd_clear();
		nokia_lcd_set_cursor(0,0);
		switch(sel){
			case 0:
			//Transformando os inteiros em caracteres
			sprintf(freqCard_string,"%u",freqcard);
			sprintf(temp_string,"%u",(uint16_t)(temp_paciente*10));
			temp_string[4]=temp_string[3];temp_string[3]=temp_string[2];temp_string[2]='.';
			sprintf(ox_string,"%u",ox_paciente);
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("SINAIS VITAIS",1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string(freqCard_string,1);
			nokia_lcd_set_cursor(43,10);
			nokia_lcd_write_string("bpm",1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_string(ox_string,1);
			nokia_lcd_set_cursor(43,20);
			nokia_lcd_write_string("%SpO2",1);
			nokia_lcd_set_cursor(0,30);
			nokia_lcd_write_string(temp_string,1);
			nokia_lcd_set_cursor(43,30);
			nokia_lcd_write_string("C",1);
			nokia_lcd_set_cursor(0,40);
			nokia_lcd_write_string(pressao_arterial,1);
			nokia_lcd_set_cursor(43,40);
			nokia_lcd_write_string("mmHg",1);
			break;
			
			case 1:
			nokia_lcd_clear();
			sprintf(freqResp_string,"%u",freq_resp);
			sprintf(oxi_string,"%u", oxi_paciente);
			sprintf(vol_string,"%u", vol_resp);
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("PARAMETROS",1);
			nokia_lcd_set_cursor(0,15);
			nokia_lcd_write_string(freqResp_string,1);
			nokia_lcd_set_cursor(25,15);
			nokia_lcd_write_string("* Resp/min",1);
			nokia_lcd_set_cursor(0,25);
			nokia_lcd_write_string(oxi_string,1);
			nokia_lcd_set_cursor(25,25);
			nokia_lcd_write_string(" %O2",1);
			nokia_lcd_set_cursor(0,35);
			nokia_lcd_write_string(vol_string, 1);
			nokia_lcd_set_cursor(25,35);
			nokia_lcd_write_string(" volume",1);
			break;
			
			case 2:
			nokia_lcd_clear();
			sprintf(freqResp_string,"%u",freq_resp);
			sprintf(oxi_string,"%u", oxi_paciente);
			sprintf(vol_string,"%u", vol_resp);
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("PARAMETROS",1);
			nokia_lcd_set_cursor(0,15);
			nokia_lcd_write_string(freqResp_string,1);
			nokia_lcd_set_cursor(25,15);
			nokia_lcd_write_string(" Resp/min",1);
			nokia_lcd_set_cursor(0,25);
			nokia_lcd_write_string(oxi_string,1);
			nokia_lcd_set_cursor(25,25);
			nokia_lcd_write_string("* %O2",1);
			nokia_lcd_set_cursor(0,35);
			nokia_lcd_write_string(vol_string, 1);
			nokia_lcd_set_cursor(25,35);
			nokia_lcd_write_string(" volume",1);
			break;
			case 3:
			nokia_lcd_clear();
			sprintf(freqResp_string,"%u",freq_resp);
			sprintf(oxi_string,"%u", oxi_paciente);
			sprintf(vol_string,"%u", vol_resp);
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("PARAMETROS",1);
			nokia_lcd_set_cursor(0,15);
			nokia_lcd_write_string(freqResp_string,1);
			nokia_lcd_set_cursor(25,15);
			nokia_lcd_write_string(" Resp/min",1);
			nokia_lcd_set_cursor(0,25);
			nokia_lcd_write_string(oxi_string,1);
			nokia_lcd_set_cursor(25,25);
			nokia_lcd_write_string(" %O2",1);
			nokia_lcd_set_cursor(0,35);
			nokia_lcd_write_string(vol_string, 1);
			nokia_lcd_set_cursor(25,35);
			nokia_lcd_write_string("* volume",1);
			break;
		}
	nokia_lcd_render();
	
	*flag_disparo=0;
	}
}
void LeituraOT(uint8_t *flag_disparo)
{
	static uint8_t cont_canal = 0;
	
	if (*flag_disparo)
	{
		switch(cont_canal)
		{
			case 0:
			temperatura = ((float)ADC/20.48)+10;
			ADMUX = 0b01000001;
			break;
			
			case 1:
			oxigenacao = (float)ADC/8.192;
			ADMUX =0b01000000;
			break;
			
			default:
			temperatura = 0;
			oxigenacao = 0xFF;
		}
		
		if (cont_canal<1)
		{
			cont_canal++;
		} 
		else
		{
			cont_canal = 0;
		}
		
		if ((temperatura<34)||(temperatura>41)||oxigenacao<60)
		{
			PORTD |= 0b10000000; 
		} 
		else
		{
			PORTD &= 0b01111111;
		}
		*flag_disparo = 0;
	}
}