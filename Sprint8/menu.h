/*
 * menu.h
 *
 * Created: 22/05/2021 00:36:14
 *  Author: ezequ
 */ 
//INCLUSÃO DAS BIBLIOTECAS
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h> 
#include "usart.h"

void MostrarNokia(uint8_t *flag_disparo,uint8_t freq_resp,uint16_t freqcard, float temp_paciente, uint8_t ox_paciente, uint8_t oxi_paciente,uint8_t vol_resp, uint8_t sel);