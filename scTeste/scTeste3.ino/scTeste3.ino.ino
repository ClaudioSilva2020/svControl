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
#define WHIGHT_B1 PD0  //Botão de seleção de peso 1
#define WHIGHT_B2 PD1 //Botão de seleção de peso 2
#define WHIGHT_B3 PD2 //Botão de seleção de peso 3
#define WHIGHT_B4 PD3 //Botão de seleção de peso 4
#define VACUOSTAT PD7 //Vacuostato
#define AND_COURSE PD5  //Chave d fim de curso do pedal

//--------------------Saídas-------------------------//
#define LED_WB1 PB0     //Led de indicação Peso 1
#define LED_WB2 PB1     //Led de indicação Peso 2
#define LED_WB3 PB2     //Led de indicação Peso 3
#define LED_WB4 PB3     //Led de indicação Peso 4
#define LED_INIT PB4    //Led de indicação INICIAL
#define PUMP_VACUUM PB5   //Contactora da bomba de vácuo
#define MAGN_KEY_SEL PC2  //Chave magnética de SELAGEM
#define MAGN_KEY_VAC PC1  //Chave magnética de VÁCUO
#define RESISTENCE PC0    //Resistência

void selar_key();
void proc_selar(int time_sel, int time_sleep);
void exc_vacuo();
void my_delay_ms(int ms);
void init_machine(void);


int main(void)
{
    int option;
    option = 0;
    
    DDRB = 0xFF; // Definindo Todos os pinos do portB como saída
    DDRC = 0xFF; // Definindo Todos os pinos do portC como Saída
    PORTC = 0x06;
    PORTB = 0x00;
    DDRD = 0x00; // Definindo o primeiro nyble do portD como Entrada
    PORTD = 0xFF; //PULL- UP  

    selar_key();
    set_Bit(PORTC, MAGN_KEY_VAC);
    init_machine();
   
    while (1) 
    {
        PORTC = 0x06;
        PORTB = 0x00;
      
        set_Bit(PORTB, LED_INIT);
        // Em cada IF verifica qual botão foi pressionado
        if (!test_Bit(PIND, WHIGHT_B1))
        {
            while(!test_Bit(PIND, WHIGHT_B1));
            _delay_ms(10);
            option = 1;
        }
        else if (!test_Bit(PIND, WHIGHT_B2))
        {
            while(!test_Bit(PIND, WHIGHT_B2));
            _delay_ms(10);
            option = 2;
        }
        else if (!test_Bit(PIND, WHIGHT_B3))
        {
            while(!test_Bit(PIND, WHIGHT_B3));
            _delay_ms(10);
            option = 3;
        }
        else if (!test_Bit(PIND, WHIGHT_B4))
        {
            while(!test_Bit(PIND, WHIGHT_B4));
            _delay_ms(10);
            option = 4;
        }
    
        switch (option)
        {
            case 1:
                //
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                set_Bit(PORTB, LED_WB1);
                if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
                {
                while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto remover quando for valendo
                //_delay_ms(10);
                set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
                _delay_ms(20);
                exc_vacuo();  // inicia o processo de vácuo
                proc_selar(20, 10 );
                
                // pisca o lede indicando fim do processo.
                for (int i=0; i<3; i++)
                {
                    set_Bit(PORTB, LED_WB1);
                    set_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                    clr_Bit(PORTB, LED_WB1);
                    clr_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                }
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                _delay_ms(2);
                option = 0;
                _delay_ms(500);
                selar_key();
                set_Bit(PORTC, MAGN_KEY_VAC);
                }
            break;
            
            case 2:
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                set_Bit(PORTB, LED_WB2);
                if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
                {
                while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto remover quando for valenDO
                set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
                _delay_ms(20);
                exc_vacuo();  // inicia o processo de vácuo
                proc_selar(5, 5 );
                
                // pisca o lede indicando fim do processo.
                for (int i=0; i<3; i++)
                {
                    set_Bit(PORTB, LED_WB2);
                    set_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                    clr_Bit(PORTB, LED_WB2);
                    clr_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                }
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                _delay_ms(2);
                
                option = 0;
                _delay_ms(500);
                selar_key();
                set_Bit(PORTC, MAGN_KEY_VAC);
                }
            break;
            
            case 3:
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                set_Bit(PORTB, LED_WB3);
                if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
                {
                while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto remover quando for valendo
                //_delay_ms(10);
                set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
                _delay_ms(20);
                exc_vacuo();  // inicia o processo de vácuo
                proc_selar(3, 3 );
                
                // pisca o lede indicando fim do processo.
                for (int i=0; i<3; i++)
                {
                    set_Bit(PORTB, LED_WB3);
                    set_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                    clr_Bit(PORTB, LED_WB3);
                    clr_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                }
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                _delay_ms(2);

                option = 0;
                _delay_ms(500);
                selar_key();
                set_Bit(PORTC, MAGN_KEY_VAC);
                }
            break;
            
            case 4:
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                set_Bit(PORTB, LED_WB4);
                if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
                {
                while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto  remover quando for valendo
                //_delay_ms(10);
                set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
                _delay_ms(20);
                exc_vacuo();  // inicia o processo de vácuo
                proc_selar(2, 2 );
                
                // pisca o lede indicando fim do processo.
                for (int i=0; i<3; i++)
                {
                    set_Bit(PORTB, LED_WB4);
                    set_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                    clr_Bit(PORTB, LED_WB4);
                    clr_Bit(PORTB, LED_INIT);
                    _delay_ms(200);
                }
                PORTB |= 0x10; //desliga todos menos o PB4
                PORTC = 0x06;
                _delay_ms(2);
                
                option = 0;
                _delay_ms(500);
                selar_key();
                set_Bit(PORTC, MAGN_KEY_VAC);
                }
            break;    
        }
    }
}

void init_machine(void)
{
  for (int i=0; i<3; i++)
  {
    set_Bit(PORTB, LED_INIT);
    set_Bit(PORTB, LED_WB1);
    set_Bit(PORTB, LED_WB2);
    set_Bit(PORTB, LED_WB3);
    set_Bit(PORTB, LED_WB4);
    _delay_ms(250);
    clp_Bit(PORTB, LED_INIT);
    clp_Bit(PORTB, LED_WB1);
    clp_Bit(PORTB, LED_WB2);
    clp_Bit(PORTB, LED_WB3);
    clp_Bit(PORTB, LED_WB4);
    _delay_ms(250);
  }
  
}
void my_delay_ms(int ms)
{
  while (0 < ms)
  {
    _delay_ms(1);
    --ms;
  }
}

void exc_vacuo()
{
  if (test_Bit(PIND, VACUOSTAT)) // testa se tem vacuo
  {
    set_Bit(PORTB, PUMP_VACUUM); // se não, seta bomba de vácuo
    while(test_Bit(PIND, VACUOSTAT)); // testa dentro do if se tem vácuo
    {
      clr_Bit(PORTB, PUMP_VACUUM); // se sim desliga bomba
    }
  }
  else
  {
    asm("JMP 0");// REINICIA
  }
}

void proc_selar(int time_sel, int time_sleep)
{
  clr_Bit(PORTC, MAGN_KEY_VAC); // Libera dulto de vácuo
  _delay_ms(150);
  clr_Bit(PORTC, MAGN_KEY_SEL); // Avança para selar
  my_delay_ms(time_sel*1000);
  clr_Bit(PORTC, RESISTENCE);
  my_delay_ms(time_sleep*1000);
}
void selar_key()
{
  for(int i=0;i<3;i++)
  {
    set_Bit(PORTC, MAGN_KEY_SEL);
    _delay_ms(250);
    clr_Bit(PORTC, MAGN_KEY_SEL); // Libera dulto de vácuo
    _delay_ms(150);
  }
  set_Bit(PORTC, MAGN_KEY_SEL);
}
