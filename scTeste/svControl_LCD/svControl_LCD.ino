/*
 * svControl.c
 * Version: 1.1
 * Created: 7/27/2020 11:46:47 PM
 * Author : Cláudio Silva
 */ 

//-------------------Cabeçalho do código-------------------//
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>




//-------------------------Configurações Macros ------------------------------//
#define set_Bit(Reg, bit_x) (Reg|=(1<<bit_x)) //seta bit x do registrador Reg
#define clr_Bit(Reg, bit_x) (Reg&=~(1<<bit_x)) //reseta o bit
#define test_Bit(Reg, bit_x) (Reg&(1<<bit_x))  //testa o estado e retorna 1ou0
#define clp_Bit(Reg, bit_x) (Reg^=(1<<bit_x))  //troca o estado logico do bit

//-------------------------Diretivas de Compilação---------------------------//

#define ADDRESS  0x38 // Endereços comuns: 0x27, 0x3F
#define COLUMN   16
#define ROW    2

//------------------Entrtadas----------------------//
#define OPTION_UP PD0  //Botão de seleção de peso 1
#define OPTION_DOWN_B2 PD1 //Botão de seleção de peso 2
#define OPTION_LEFT PD2 //Botão de seleção de peso 3
#define OPTION_RIGHT PD3 //Botão de seleção de peso 4
#define VACUOSTAT PD4 //Vacuostato
#define AND_COURSE PD5  //Chave d fim de curso do pedal

//--------------------Saídas-------------------------//
#define AUX_OUT1 PB0     //Led de indicação Peso 1
#define AUX_OUT2 PB1     //Led de indicação Peso 2
//#define LED_WB3 PB2     //Led de indicação Peso 3
//#define LED_WB4 PB3     //Led de indicação Peso 4
#define LED_INIT PB4    //Led de indicação INICIAL
#define PUMP_VACUUM PB5   //Contactora da bomba de vácuo
#define MAGN_KEY_SEL PC2  //Chave magnética de SELAGEM
#define MAGN_KEY_VAC PC1  //Chave magnética de VÁCUO
#define RESISTENCE PC0    //Resistência

#define TIME_SEL  20
#define TIME_SLEEP 10
#define TIME_SEL2  10
#define TIME_SLEEP2  5
#define TIME_SEL3  8
#define TIME_SLEEP3  4
#define TIME_SEL4  5
#define TIME_SLEEP4  2

// INSTANCIANDO OBJETOS
LiquidCrystal_I2C lcd(ADDRESS, COLUMN, ROW);

typedef struct
{
  int time_uitil_sel;
  int time_res_off;
  int time_sleep_end; 
} Machine;

   

void _init_monitor(void)
{
  //Inicialização do Display
  lcd.init(); 
  lcd.backlight(); 
  lcd.clear(); 

  //Mostrando início da maquina
  lcd.print("...Iniciando...");
  lcd.setCursor(0, 1);
  lcd.print("Seladora a Vacuo");
  delay(1000);
  lcd.clear();

  // Aguardando comandos
  lcd.setCursor(0, 0);
  lcd.print("Seladora a Vacuo");
  lcd.setCursor(0, 1);
  lcd.print("Aguard. inicio");
  delay(500);
}

void init_machine(void)
{
  // pisca o led de inicialização
  for (int i=0; i<3; i++)
  {
    set_Bit(PORTB, LED_INIT);
    _delay_ms(250);
    clr_Bit(PORTB, LED_INIT);
    _delay_ms(250);
  }
  // inicializa display
  _init_monitor();
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
      // testa se tem vacuo
      if (test_Bit(PIND, VACUOSTAT)) 
      {
      // se não, seta bomba de vácuo
      set_Bit(PORTB, PUMP_VACUUM);
      // testa dentro do if se tem vácuo 
      while(test_Bit(PIND, VACUOSTAT)); 
      {
        // se sim desliga bomba
        clr_Bit(PORTB, PUMP_VACUUM); 
      }
      }
      else
      {
        asm("JMP 0");// REINICIA
      }
}

void proc_selar(int time_sel, int time_sleep, int time_to_sel)
{
  // Libera dulto de vácuo
  clr_Bit(PORTC, MAGN_KEY_VAC); 
  my_delay_ms(time_to_sel*1000);
  // Avança para selar
  clr_Bit(PORTC, MAGN_KEY_SEL); 
  my_delay_ms(time_sel*1000);
  //desliga resistencia
  clr_Bit(PORTC, RESISTENCE); 
  my_delay_ms(time_sleep*1000);
}

void selar_key_init()
{
  for(int i=0;i<10;i++)
  {
    set_Bit(PORTC, MAGN_KEY_VAC);
    _delay_ms(250);
    // Libera dulto de vácuo
    clr_Bit(PORTC, MAGN_KEY_VAC); 
    _delay_ms(250);
  }
  set_Bit(PORTC, MAGN_KEY_VAC);
}

void setup(void)
{
    DDRB = 0xFF; // Definindo Todos os pinos do portB como saída
    DDRC = 0xFF; // Definindo Todos os pinos do portC como Saída
    PORTB = 0x00;//
    PORTC = 0x00;//
    DDRD = 0x00; // Definindo o primeiro nyble do portD como Entrada
    PORTD = 0xFF; //PULL- UP  
    
    init_machine();
}

void loop()
{
     int on_press;
     selar_key();
     set_Bit(PORTC, MAGN_KEY_VAC);
     set_Bit(PORTB, LED_INIT);
     //se o botão de menu UP for precionado
     if(!test_Bit(PIND, OPTION_UP))
     {
        lcd.clear();
        // Aguardando comandos
        lcd.setCursor(0, 0);
        lcd.print("Menu de Tempos");
        lcd.setCursor(0, 1);
        lcd.print("Tempo Selando ->");         
        // Entra no Menu
        menu_function();
     }
     

     
    
  
    switch (option)
    {
      case 1:
        //
        //option = 0;
        PORTB |= 0x10; //desliga todos menos o PB4
        PORTC = 0x00;
        set_Bit(PORTB, LED_WB1);
        if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
        {
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de vácuo
          proc_selar(TIME_SEL, TIME_SLEEP );
        
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
          PORTB |= 0x00; //desliga todos
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
          /*else
          {
            _delay_ms(10000); // se não aguarda 10 segundos e reinicia o processo
            asm("JMP 0"); // Reinicia
          }*/
            
      break;
    
      case 2:
        PORTB |= 0x10; //desliga todos menos o PB4
        PORTC = 0x00;
        set_Bit(PORTB, LED_WB2);
        if (!test_Bit(PIND, AND_COURSE)) //testa se o pedal foi acionado
        {
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de vácuo
          proc_selar(TIME_SEL2, TIME_SLEEP2);
        
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
          PORTB |= 0x00; //desliga todos menos o PB4
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
          /*else
          {
            _delay_ms(10000); // se não aguarda 10 segundos e reinicia o processo
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
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de vácuo
          proc_selar(TIME_SEL3, TIME_SLEEP3);
        
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
          PORTB |= 0x00; //desliga todos menos o PB4
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
         /* else
          {
            _delay_ms(10000); // se não aguarda 10 segundos e reinicia o processo
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
          while(!test_Bit(PIND, AND_COURSE)); // aguarda o botão ser solto  remover quando for valendo
          //_delay_ms(10);
          set_Bit(PORTC, RESISTENCE); // se sim liga a resistência
          _delay_ms(20);
          exc_vacuo();  // inicia o processo de vácuo
          proc_selar(TIME_SEL4, TIME_SLEEP4);
        
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
          PORTB |= 0x00; //desliga todos menos o PB4
          PORTC = 0x00;
          _delay_ms(2);
          //while(!test_Bit(PIND, AND_COURSE));
          //asm("JMP 0");
          option = 0;
          }
      
         /* else
          {
            _delay_ms(10000); // se não aguarda 10 segundos e reinicia o processo
            asm("JMP 0"); // Reinicia
          }*/
        //
      
      break;
    
      //default:
        //asm("JMP 0");
    
    }
}
void menu_function()
{
  
}
