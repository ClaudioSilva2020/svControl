/*
 * svControl.c
 * Version: 1.0
 * Created: 7/27/2020 11:46:47 PM
 * Author : Cl�udio Silva
 */ 

//-------------------Cabe�alho do c�digo-------------------//
#define F_CPU 4000000UL
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
#define WHIGHT_B1 PD0  //Bot�o de sele��o de peso 1
#define WHIGHT_B2 PD1 //Bot�o de sele��o de peso 2
#define WHIGHT_B3 PD2 //Bot�o de sele��o de peso 3
#define WHIGHT_B4 PD3 //Bot�o de sele��o de peso 4
#define VACUOSTAT PD4 //Vacuostato
#define AND_COURSE PD5  //Chave d fim de curso do pedal

//--------------------Sa�das-------------------------//
#define LED_WB1 PB0     //Led de indica��o Peso 1
#define LED_WB2 PB1     //Led de indica��o Peso 2
#define LED_WB3 PB2     //Led de indica��o Peso 3
#define LED_WB4 PB3     //Led de indica��o Peso 4
#define LED_INIT PB4    //Led de indica��o INICIAL
#define PUMP_VACUUM PB5   //Contactora da bomba de v�cuo
#define MAGN_KEY_SEL PC2  //Chave magn�tica de SELAGEM
#define MAGN_KEY_VAC PC1  //Chave magn�tica de V�CUO
#define RESISTENCE PC0    //Resist�ncia


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
    set_Bit(PORTB, PUMP_VACUUM); // se n�o, seta bomba de v�cuo
    while(test_Bit(PIND, VACUOSTAT)); // testa dentro do if se tem v�cuo
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
  set_Bit(PORTC, MAGN_KEY_VAC); // Libera dulto de v�cuo
  _delay_ms(500);
  set_Bit(PORTC, MAGN_KEY_SEL); // Avan�a para selar
  my_delay_ms(time_sel*1000);
  clr_Bit(PORTC, RESISTENCE);
  my_delay_ms(time_sleep*1000);
}


int main(void)
{
    int option;
    option = 0;
    
    DDRB = 0xFF; // Definindo Todos os pinos do portB como sa�da
    DDRC = 0xFF; // Definindo Todos os pinos do portC como Sa�da
    DDRD = 0x00; // Definindo o primeiro nyble do portD como Entrada
    PORTD = 0xFF; //PULL- UP  
    
    init_machine();
   
  
  
  
  
    while (1) 
    {
     set_Bit(PORTB, LED_INIT);
    // Em cada IF verifica qual bot�o foi pressionado
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
        PORTC = 0x00;
        set_Bit(PORTB, LED_WB1);
        if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
        {
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o bot�o ser solto remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resist�ncia
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de v�cuo
          proc_selar(20, 10 );
        
          // pisca o lede indicando fim do processo.
          for (int i=0; i<3; i++)
          {
            set_Bit(PORTB, LED_WB1);
            _delay_ms(200);
            clr_Bit(PORTB, LED_WB1);
            _delay_ms(200);
          }
          PORTB |= 0x10; //desliga todos menos o PB4
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
          /*else
          {
            _delay_ms(10000); // se n�o aguarda 10 segundos e reinicia o processo
            asm("JMP 0"); // Reinicia
          }*/
            
      break;
    
      case 2:
        PORTB |= 0x10; //desliga todos menos o PB4
        PORTC = 0x00;
        set_Bit(PORTB, LED_WB2);
        if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
        {
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o bot�o ser solto remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resist�ncia
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de v�cuo
          proc_selar(5, 5 );
        
          // pisca o lede indicando fim do processo.
          for (int i=0; i<3; i++)
          {
            set_Bit(PORTB, LED_WB2);
            _delay_ms(200);
            clr_Bit(PORTB, LED_WB2);
            _delay_ms(200);
          }
          PORTB |= 0x10; //desliga todos menos o PB4
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
          /*else
          {
            _delay_ms(10000); // se n�o aguarda 10 segundos e reinicia o processo
            asm("JMP 0"); // Reinicia
          }
          //*/
        
      break;
    
      case 3:
        PORTB |= 0x10; //desliga todos menos o PB4
        PORTC = 0x00;
        set_Bit(PORTB, LED_WB3);
        if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
        {
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o bot�o ser solto remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resist�ncia
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de v�cuo
          proc_selar(3, 3 );
        
          // pisca o lede indicando fim do processo.
          for (int i=0; i<3; i++)
          {
            set_Bit(PORTB, LED_WB3);
            _delay_ms(200);
            clr_Bit(PORTB, LED_WB3);
            _delay_ms(200);
          }
          PORTB |= 0x10; //desliga todos menos o PB4
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
         /* else
          {
            _delay_ms(10000); // se n�o aguarda 10 segundos e reinicia o processo
            asm("JMP 0"); // Reinicia
          }*/
        //
      
      break;
    
      case 4:
        PORTB |= 0x10; //desliga todos menos o PB4
        PORTC = 0x00;
        set_Bit(PORTB, LED_WB4);
        if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
        {
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o bot�o ser solto  remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resist�ncia
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de v�cuo
          proc_selar(2, 2 );
        
          // pisca o lede indicando fim do processo.
          for (int i=0; i<3; i++)
          {
            set_Bit(PORTB, LED_WB4);
            _delay_ms(200);
            clr_Bit(PORTB, LED_WB4);
            _delay_ms(200);
          }
          PORTB |= 0x10; //desliga todos menos o PB4
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
         /* else
          {
            _delay_ms(10000); // se n�o aguarda 10 segundos e reinicia o processo
            asm("JMP 0"); // Reinicia
          }*/
        //
      
      break;
    
      //default:
        //asm("JMP 0");
    
    }
  
    
    }
}

