/*
 * menu.c
 *
 * Created: 21/05/2021 23:06:19
 *  Author: ezequ
 */ 
#include <stdlib.h>
#include <stdio.h>
#include "nokia5110.h"
#include "menu.h"

void MostrarNokia(uint8_t *flag_disparo,uint8_t freq_resp,uint16_t freqcard, float temp_paciente, uint8_t ox_paciente, uint8_t oxi_paciente,uint8_t vol_resp, uint8_t sel){

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