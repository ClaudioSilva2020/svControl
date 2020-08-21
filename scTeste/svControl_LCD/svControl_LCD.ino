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

#define ADDRESS  0x38 // Endereço do I2C
#define COLUMN   16
#define ROW    2

//--------------------------------------------Entrtadas-----------------------------------------//
#define OPTION_UP PD0  //Botão de seleção de peso 1
#define OPTION_DOWN PD1 //Botão de seleção de peso 2
#define OPTION_LEFT PD2 //Botão de seleção de peso 3
#define OPTION_RIGHT PD3 //Botão de seleção de peso 4
#define VACUOSTAT PD4 //Vacuostato
#define AND_COURSE PD5  //Chave d fim de curso do pedal

//----------------------------------------------Saídas-------------------------------------------//
#define AUX_OUT1 PB0     //Led de indicação Peso 1
#define AUX_OUT2 PB1     //Led de indicação Peso 2
//#define LED_WB3 PB2     //Led de indicação Peso 3
//#define LED_WB4 PB3     //Led de indicação Peso 4
#define LED_INIT PB4    //Led de indicação INICIAL
#define PUMP_VACUUM PB5   //Contactora da bomba de vácuo
#define MAGN_KEY_SEL PC2  //Chave magnética de SELAGEM
#define MAGN_KEY_VAC PC1  //Chave magnética de VÁCUO
#define RESISTENCE PC0    //Resistência


//-------------------------------------   Protótipos de Função   ---------------------------------
void init_machine(void);
void _init_monitor(void);
void my_delay_ms(int ms);
void exc_vacuo(void);
void proc_selar(int time_sel, int time_sleep, int time_to_sel);
void selar_key_init(void);


// Instanciando Objeto lcd
LiquidCrystal_I2C lcd(ADDRESS, COLUMN, ROW);

String msg_monitor [2] [2] = {{"Ajuste de tempo","tseg-Selagem:"},{"Ajuste de Tempo","tseg-Esfriar:"}};

// Estrutura da máquina com os atributos de tempo
typedef struct
{
  int time_uitil_sel;
  int time_res_off;
  int time_sleep_end; 
} Machine;

int on_press = 0;
// Instância de objeto
Machine parameter;





//------------------------------------Inicio do Setup------------------------------------------
void setup(void)
{
    Serial.begin(9600);
    parameter.time_res_off = 5;
    parameter.time_sleep_end = 5;
    parameter.time_uitil_sel = 5;


    DDRB = 0xFF; // Definindo Todos os pinos do portB como saída
    DDRC = 0xFF; // Definindo Todos os pinos do portC como Saída
    PORTB = 0x00;//
    PORTC = 0x00;//
    DDRD = 0x00; // Definindo o primeiro nyble do portD como Entrada
    PORTD = 0xFF; //PULL- UP  
    
    init_machine();
    Serial.println("Aguardando");
    my_delay_ms(10);    
}

//-------------------------------------Inicio Do Loop ------------------------------------------
void loop()
{
    //selar_key_init();
    set_Bit(PORTC, MAGN_KEY_VAC);
    set_Bit(PORTB, LED_INIT);
    if (!test_Bit(PIND, OPTION_UP) && on_press == 0)
    { 
        while (!test_Bit(PIND, OPTION_UP));
        my_delay_ms(10);
        menu_function();
    }else if (on_press == 3)
    {
        lcd.clear();
        lcd.print("Seladora a Vacuo");
        lcd.setCursor(0, 1);
        lcd.print("Aguard. Pedal");
        while (test_Bit(PIND, AND_COURSE))
        {
          /* code */
        }
        
    }
    
}












// ---------------------------------------- Funções ---------------------------------------------

void menu_function()
{
    my_delay_ms(20);        
    msg_tsel();
    msg_tesfriar();
}
// ------------------------------ Função de inicializar a tela ---------------------------------- 
void _init_monitor(void)
{
  //Inicialização do Display
  lcd.backlight(); 
  lcd.init(); 
  lcd.clear(); 

  //Mostrando início da maquina
  lcd.print("...Iniciando...");
  lcd.setCursor(0, 1);
  lcd.print("Seladora a Vacuo");
  delay(2000);
  lcd.clear();

  // Aguardando comandos
  lcd.setCursor(0, 0);
  lcd.print("Seladora a Vacuo");
  lcd.setCursor(0, 1);
  lcd.print(" ---Operando--- ");
  delay(20);
}
// --------------------------------------Inicia a máquina----------------------------------------
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
// ---------------------------------------- meu delay -------------------------------------------
void my_delay_ms(int ms)
{
  while (0 < ms)
  {
    _delay_ms(1);
    --ms;
  }
}
// ------------------------------------- Função de vácuo ----------------------------------------
void exc_vacuo(void)
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
// ------------------------ Função do processo de selagem com parÂmetros ------------------------
// --------------------- De tempos realacionados as esposição á resistência ---------------------
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
// -------------------------- Função para centralizar a chave magnética -------------------------
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
// ---------------------------- Função inrenmento do tempo ate selagem --------------------------
void msg_tsel(void)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg_monitor[0][0]);
    lcd.setCursor(0, 1);
    lcd.print(msg_monitor[0][1]);
    lcd.setCursor(14, 1);
    lcd.print(parameter.time_uitil_sel);

   
    while (test_Bit(PIND, OPTION_DOWN))
    {
        if (!test_Bit(PIND, OPTION_RIGHT))
        {
          while (!test_Bit(PIND, OPTION_RIGHT));
          my_delay_ms(20);
          parameter.time_uitil_sel++;
          lcd.setCursor(14, 1);
          lcd.print(parameter.time_uitil_sel);
        }else if (!test_Bit(PIND, OPTION_LEFT))
        {
          while (!test_Bit(PIND, OPTION_LEFT));
          my_delay_ms(20);
          parameter.time_uitil_sel--;
          lcd.setCursor(14, 1);
          lcd.print(parameter.time_uitil_sel);
        }
    }
}
// ----------------------- Função de incremento de tempo pos resistência off --------------------
void msg_tesfriar(void)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg_monitor[1][0]);
    lcd.setCursor(0, 1);
    lcd.print(msg_monitor[1][1]);
    lcd.setCursor(14, 1);
    lcd.print(parameter.time_sleep_end);
    
    while (test_Bit(PIND, OPTION_UP))
    {
      Serial.println("on press:");
      Serial.println(on_press);
        if (!test_Bit(PIND, OPTION_RIGHT))
        {
            while (!test_Bit(PIND, OPTION_RIGHT));
            my_delay_ms(20);
            parameter.time_sleep_end++;
            lcd.setCursor(14, 1);
            lcd.print(parameter.time_sleep_end);   
        }
        else if (!test_Bit(PIND, OPTION_LEFT))
        {
            while (!test_Bit(PIND, OPTION_LEFT));
            my_delay_ms(20);
            parameter.time_sleep_end--;
            lcd.setCursor(14, 1);
            lcd.print(parameter.time_sleep_end);
        }
      on_press = 3;
    }  
}
