/*
 * svControl.c
 * Version: 1.0
 * Created: 7/27/2020 11:46:47 PM
 * Author : Cláudio Silva
 */ 

//-------------------Cabeçalho do código-------------------//
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

//-------------------------Configurações Macros ------------------------------//
#define set_Bit(Reg, bit_x) (Reg|=(1<<bit_x)) //seta bit x do registrador Reg
#define clr_Bit(Reg, bit_x) (Reg&=~(1<<bit_x)) //reseta o bit
#define test_Bit(Reg, bit_x) (Reg&(1<<bit_x))  //testa o estado e retorna 1ou0
#define clp_Bit(Reg, bit_x) (Reg^=(1<<bit_x))  //troca o estado logico do bit

//-------------------------Diretivas de Compilação---------------------------//

//------------------Entrtadas----------------------//
#define WHIGHT_B1 PC0	//Botão de seleção de peso 1
#define WHIGHT_B2 PC1	//Botão de seleção de peso 2
#define WHIGHT_B3 PC2	//Botão de seleção de peso 3
#define WHIGHT_B4 PC3	//Botão de seleção de peso 4
#define VACUOSTAT PC4	//Vacuostato
#define AND_COURSE PC5	//Chave d fim de curso do pedal

//--------------------Saídas-------------------------//
#define LED_WB1 PB0			//Led de indicação Peso 1
#define LED_WB2 PB1			//Led de indicação Peso 2
#define LED_WB3 PB2			//Led de indicação Peso 3
#define LED_WB4 PB3			//Led de indicação Peso 4
#define LED_INIT PB4		//Led de indicação INICIAL
#define PUMP_VACUUM PB5		//Contactora da bomba de vácuo
#define MAGN_KEY_SEL PB6	//Chave magnética de SELAGEM
#define MAGN_KEY_VAC PB7	//Chave magnética de VÁCUO
#define RESISTENCE PD1		//Resistência


void init_machine(void)
{
	for (int i=0; i<3; i++)
	{
		set_Bit(PORTB, LED_INIT);
		set_Bit(PORTB, LED_WB1);
		set_Bit(PORTB, LED_WB2);
		set_Bit(PORTB, LED_WB3);
		set_Bit(PORTB, LED_WB4);
		_delay_ms(500);
		clp_Bit(PORTB, LED_INIT);
		clp_Bit(PORTB, LED_WB1);
		clp_Bit(PORTB, LED_WB2);
		clp_Bit(PORTB, LED_WB3);
		clp_Bit(PORTB, LED_WB4);
		_delay_ms(500);
	}
	
}

void exc_vacuo()
{
	if (test_Bit(PINC, VACUOSTAT)) // testa se tem vacuo
	{
		set_Bit(PORTB, PUMP_VACUUM); // se não seta bomba de vácuo
		if (~test_Bit(PINC, VACUOSTAT)) // testa dentro do if se tem vácuo
		{
			clr_Bit(PORTB, PUMP_VACUUM); // se sim desliga bomba
		}
	}
	else
	{
		return 0;
	}
}


int main(void)
{
	DDRB = 0xFF; // Definindo Todos os pinos do portB como saída
	DDRC = 0x00; // Definindo Todos os pinos do portC como entrada
	DDRD = 0x0F; // Definindo o primeiro nyble do portD como saída
	
	init_machine();
	
	
	
    while (1) 
    {
		// Em cada IF verifica qual botão foi pressionado
		if (~test_Bit(PINC, WHIGHT_B1))
		{
			while(~test_Bit(PINC, WHIGHT_B1)); // aguarda o botão ser solto
			_delay_ms(10);
			if (~test_Bit(PINC, AND_COURSE)) //testa se o pedal foi acionado
			{
				_delay_ms(10);
				set_Bit(PORTD, RESISTENCE); // se sim liga a resistência
				_delay_ms(20);
				exc_vacuo();	// inicia o processo de vácuo
				set_Bit(PORTB, MAGN_KEY_VAC);
				_delay_ms(20);
				set_Bit(PORTB, MAGN_KEY_SEL);
				
			}
			_delay_ms(10000); // se não aguarda 10 segundos e reinicia o processo
			else
			{
				return 0;
			}
			
		}
		else if (~test_Bit(PINC, WHIGHT_B2))
		{
			while(~test_Bit(PINC, WHIGHT_B2));
			_delay_ms(10);
		} 
		else if (~test_Bit(PINC, WHIGHT_B3))
		{
			while(~test_Bit(PINC, WHIGHT_B3));
			_delay_ms(10);
		} 
		else if (~test_Bit(PINC, WHIGHT_B4))
		{
			while(~test_Bit(PINC, WHIGHT_B4));
			_delay_ms(10);
		} 
    }
}

