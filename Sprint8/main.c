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
#include "usart.h"
#include "menu.h"

//PROTÓPICOS DOS MÉTODOS E FUNÇÕES
void RespiraLeds(uint8_t *flag_disparo);
void LeituraOT(uint8_t *flag_disparo);

//DEFINIÇÃO DE STRUCT
typedef struct sinaisVitais
{
    uint16_t freq_card;
    uint8_t temperatura;
    uint8_t spO2;
    uint8_t respPorMinuto;
    uint8_t misturaO2;
} sinaisVitais;
//DECLARAÇÃO DE VARIÁVEIS GLOBAIS
uint8_t flag_200ms = 0, flag_resp_por16 = 0, flag_150ms = 0, flag_hora = 0;
uint8_t sel = 0;
uint32_t tempo_ms = 0;
uint16_t porcentOxi = 0;
uint16_t vol = 0;
sinaisVitais sv = {
    .freq_card = 0,
    .misturaO2 = 2000,
    .respPorMinuto = 5,
    .spO2 = 0,
    .temperatura=0
};
//ALTERAÇÃO NAS RESPIRAÇÕES POR MINUTOS QUANDO APERTAR OS BOTÕES
ISR(INT0_vect)
{

    if (sv.respPorMinuto < 30 && sv.misturaO2 < 4000 && vol < 8)
    {
        switch (sel)
        {
        case 1:
            sv.respPorMinuto++;
            break;
        case 2:
            sv.misturaO2 = sv.misturaO2 + 200;
            porcentOxi += 10;
            OCR1B = sv.misturaO2;
            break;
        case 3:
            vol++;
            break;
        }
    }
}
ISR(INT1_vect)
{

    if (sv.respPorMinuto > 5 && sv.misturaO2 > 2000 && vol >= 0)
    {
        switch (sel)
        {
        case 1:
            sv.respPorMinuto--;
            break;
        case 2:
            sv.misturaO2 = sv.misturaO2 - 200;
            porcentOxi -= 10;
            OCR1B = sv.misturaO2;
            break;
        case 3:
            vol--;
            break;
        }
    }
}
ISR(PCINT0_vect)
{
    static uint8_t flagSobe = 0;
    if (flagSobe)
    {
        if (sel < 3)
            sel++;
        else
            sel = 0;
    }
    flagSobe = !flagSobe;
}
ISR(TIMER0_COMPA_vect)
{

    tempo_ms++;
    PORTD ^= 0b00000001;
    if ((tempo_ms % 200) == 0)
        flag_200ms = 1;
    if ((tempo_ms % (3750 / sv.respPorMinuto)) == 0)
        flag_resp_por16 = 1;
    if ((tempo_ms % 150) == 0)
    {
        flag_150ms = 1;
    }
    if (tempo_ms % 5000)
        flag_hora = 1;
}
ISR(PCINT2_vect)
{
    static uint32_t tempo_ms_freqcard_anterior = 0;
    sv.freq_card = 6000 / ((tempo_ms - tempo_ms_freqcard_anterior) * 2);
    tempo_ms_freqcard_anterior = tempo_ms;
}

int main(void)
{
    //ALTERANDO OS PINOS GPIO
    DDRB = 0b10111111; // COMO SAÍDAS
    PORTB = 0b01000000;
    DDRC = 0b11111100;
    DDRD = 0b10000000;  // COMO ENTRADAS menos o D0
    PORTD = 0b11111110; //HABILITANDO todos o pull-ups menos o D0

    //Configuração das Interrupções
    EICRA = 0b00001010;  //INTERRUPÇÕES SENSÍVEIS A BORDA DE move
    EIMSK = 0b00000011;  //HABILITA AS INTERRUPÇÕES EXTERNAS (INT0, INT1)
    PCICR = 0b00000101;  //habilita o pino de interrupção 2 PCINT2 e PCINT0
    PCMSK2 = 0b00100000; // habilita interrupt pnchange17, ou seja PD6
    PCMSK0 = 0b01000000; // habila interrupt PB6

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
    ADCSRA = 0b11100111;
    ADCSRB = 0b00000000;
    DIDR0 = 0b00000011;

    sei(); //HABILITA INTERRUPÇÕES GLOBAIS

    nokia_lcd_init();

    /* Replace with your application code */
    USART_Inicio(MYUBRR);
    while (1)
    {

        LeituraOT(&flag_150ms);
        RespiraLeds(&flag_resp_por16);
        MostrarNokia(&flag_200ms, sv.respPorMinuto, sv.freq_card, sv.temperatura, sv.spO2, porcentOxi, vol, sel);
    }
}

void RespiraLeds(uint8_t *flag_disparo)
{
    static uint32_t move = 2000, subida = 1;
    OCR1A = move;

    if (*flag_disparo)
    {
        if (subida)
        {
            if (move == 2000)
            {
                subida = 0;
                move = 2000;
            }
            else
            {
                move = move - 250;
            }
        }
        else
        {
            if (move == ((vol * 250) + 2000))
            {
                subida = 1;
                move = ((vol * 250) + 2000);
            }
            else
            {
                move = move + 250;
            }
        }
        *flag_disparo = 0;
    }
}

void LeituraOT(uint8_t *flag_disparo)
{
    static uint8_t cont_canal = 0;

    if (*flag_disparo)
    {
        switch (cont_canal)
        {
        case 0:
            sv.temperatura = ((float)ADC / 20.48) + 10;
            ADMUX = 0b01000001;
            break;

        case 1:
            sv.spO2 = (float)ADC / 8.192;
            ADMUX = 0b01000000;
            break;

        default:
            sv.temperatura = 0;
            sv.spO2 = 0xFF;
        }

        if (cont_canal < 1)
        {
            cont_canal++;
        }
        else
        {
            cont_canal = 0;
        }

        if ((sv.temperatura < 34) || (sv.temperatura > 41) || sv.spO2 < 60)
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