/*
 * svControl.c
 * Version: 1.0
 * Created: 7/27/2020 11:46:47 PM
 * Author : Cl�udio Silva
 */ 

//-------------------Cabe�alho do c�digo-------------------//
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

//-------------------------Configura��es Macros ------------------------------//
#define set_Bit(Reg, bit_x) (Reg|=(1<<bit_x)) //seta bit x do registrador Reg
#define clr_Bit(Reg, bit_x) (Reg&=~(1<<bit_x)) //reseta o bit
#define test_Bit(Reg, bit_x) (Reg&(1<<bit_x))  //testa o estado e retorna 1ou0
#define clp_Bit(Reg, bit_x) (Reg^=(1<<bit_x))  //troca o estado logico do bit

//-------------------------Diretivas de Compila��o---------------------------//

//------------------Entrtadas----------------------//
#define WHIGHT_B1 PC0	//Bot�o de sele��o de peso 1
#define WHIGHT_B2 PC1	//Bot�o de sele��o de peso 2
#define WHIGHT_B3 PC2	//Bot�o de sele��o de peso 3
#define WHIGHT_B4 PC3	//Bot�o de sele��o de peso 4
#define VACUOSTAT PC4	//Vacuostato
#define AND_COURSE PC5	//Chave d fim de curso do pedal

//--------------------Sa�das-------------------------//
#define LED_WB1 PB0			//Led de indica��o Peso 1
#define LED_WB2 PB1			//Led de indica��o Peso 2
#define LED_WB3 PB2			//Led de indica��o Peso 3
#define LED_WB4 PB3			//Led de indica��o Peso 4
#define LED_INIT PB4		//Led de indica��o INICIAL
#define PUMP_VACUUM PB5		//Contactora da bomba de v�cuo
#define MAGN_KEY_SEL PB6	//Chave magn�tica de SELAGEM
#define MAGN_KEY_VAC PB7	//Chave magn�tica de V�CUO
#define RESISTENCE PD1		//Resist�ncia


void init_machine()
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


int main(void)
{
	DDRB = 0xFF; // Definindo Todos os pinos do portB como sa�da
	DDRC = 0x00; // Definindo Todos os pinos do portC como entrada
	DDRD = 0x0F; // Definindo o primeiro nyble do portD como sa�da
	
	init_machine();
	
	
	
    while (1) 
    {
		if (test_Bit(PORTC, AND_COURSE))
		{
			set_Bit(PORTD, RESISTENCE);
			if (test_Bit(PORTC, WHIGHT_B1))
			{
				_delay_ms(50);
				if(test_Bit(PORTC, WHIGHT_B1))
					
			} 
			else
			{
			}
		}
		
		
    }
}

