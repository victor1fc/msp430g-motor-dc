#include <msp430.h> 
#include <math.h>

char    num_sensor[10] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
char      saudacao[12] = "UFPR - TE328";
char         medida[9] = {'R','o','t','a','c','a','o',':',' '};

unsigned int   contador = 0;
unsigned int        rpm = 0;
unsigned char    cursor = 0;
unsigned char      menu = 1;
unsigned int      dig_1 = 3;
unsigned int       duty = 0;
unsigned int    rotacao = 0;
float    a ;
float    b ;
float    c ;
float    d ;
float    e ;

char            saida_1 = 1;
char            saida_2 = 0;
char            sentido = 0;
char           seletora = 0;


#define RS_1 P1OUT |= BIT6;                 //RS = 1 -> dado
#define RS_0 P1OUT &= ~BIT6;                //RS = 0 -> instrucao

void    config_uC (void)
{
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    WDTCTL = WDT_MDLY_8;                    //Habilita WATCHDOG como interval timer com reset a cada 8 ms

    P1DIR = BIT1+BIT2 + BIT6+BIT7;          //Bit6 e 7 do P1 como saidas
    P2DIR = BIT3 + BIT4+BIT5+BIT6+BIT7;     //Bit4 ao 7 do P2 como saidas

    P1SEL = 0;
    P1SEL2 = 0;
    P2SEL = 0;                              //Entradas/saidas digitais do P2
    P2SEL2 = 0;
}
void config_Interrup(void)
{   P1IE  |= BIT0;                          // Habilita interrupção pino 1.0 (1=habilita/0=desabilita)
    P1IES &= ~BIT0;                         // Seleciona borda de interrup pino 1.0 0=+subida 1=-descida)
    P1IFG &= ~BIT0;                         // Limpa o flag de interrupção P1.0

    P2IE  |= BIT0+BIT1+BIT2;                // Habilita interrupção pino 2.0 ao 2.2  (1=habilita/0=desabilita)
    P2IES |= (BIT0+BIT1+BIT2);              // Seleciona borda de interrup pino 2.0 ao 2.2 0=+subida 1=-descida)
    P2IFG &= ~(BIT0+BIT1+BIT2);             // Limpa o flag de interrupção P2.0 ao 2.2
}
void pulso_E(void)
{
    P1OUT |= BIT7;                          // E=1
    P1OUT &= ~BIT7;                         // E=0
}

void enviar_lcd(char tipo,char byte)
{
    if(tipo == 0)
    {
      RS_0;                                 // instrucao
      __delay_cycles(1000);
      P2OUT=byte;                           // envia upper nibble
      pulso_E();
      P2OUT=byte<<4;                        // envia  lower nibble
      pulso_E();
    }
    if(tipo == 1)
    {
      RS_1;                                 // dado
      __delay_cycles(1000);
      P2OUT=byte;                           // envia upper nibble
      pulso_E();
      P2OUT=byte<<4;                        //envia  lower nibble
      pulso_E();
    }
}

void start_lcd (void)
{
    __delay_cycles(15000);
    RS_0;
    P2OUT = 0x30;
    pulso_E();
    __delay_cycles(5000);
    P2OUT = 0x30;
    pulso_E();
    __delay_cycles(1000);
    P2OUT = 0x30;
    pulso_E();
    P2OUT = 0x20;
    __delay_cycles(1000);
    pulso_E();
    enviar_lcd(0, 0x28);
    enviar_lcd(0, 0x08);                // display OFF
    enviar_lcd(0, 0x01);                //display CLEAR
    enviar_lcd(0, 0x06);                //Incrementa AC e nao desloca display
    enviar_lcd(0, 0x0F);                //Liga display, mostra cursor, cursor piscante

    while (cursor<12)                   //Escreve saudacao no display
    {
        enviar_lcd(1, saudacao[cursor]);
        __delay_cycles(300000);
        cursor = cursor + 1;
    }
    __delay_cycles(3000000);
    enviar_lcd(0, 0x01);                //display CLEAR
}

void config_timer0_A(void)
{
    TA0CTL = TASSEL_2;                  // Fonte de clock
    TA0CTL |=ID_3;                      // divisor=8
    TA0CTL |=MC_0;                      // Timer off
    TA0CCTL0 |= CCIE;                   // habilita interrup
    TA0CCR0 = 46875;                    // valor de reset 46875 para obter 750 ms em modo up/down
}

void    config_timer1_A(void)
{
    TA1CTL=TASSEL_2;                    //define SM como FONTE DE CLOCK
    TA1CTL |=MC_0;                      //TIMER off
    TA1CTL |=ID_0;                      // define DIVISOR POR 1
    TA1CCTL0 |= CCIE;                   //habilita interrupcao CCTL0
    TA1CCTL1 |= CCIE;                   //habilita interrupcao CCTL1
}
void    config_AD(void)
{
    ADC10CTL0 |= SREF_0;                //Ref.: VR+ = Vcc, VR- = Vss
    ADC10CTL0 |= ADC10SHT_2;            //Sample and Hold Time = 16*ADC10CLKs
    ADC10CTL0 |= MSC;                   //Início da conversão com pulso em SHI
    ADC10CTL0 |= ADC10ON ;              //ADC on
    ADC10CTL0 |= ADC10IE ;              //Habilita Interrupção do ADC
    ADC10CTL1 |= INCH_5 ;               //Entrada através do Canal 5 (A5)
    ADC10AE0  |= BIT5 ;                 //Habilita P1.5 hab. para entrada analógica
    TA1CCR0 = 1030;
}

int main(void)
{
    config_uC();
    config_Interrup();
    config_timer0_A();
    config_timer1_A();
    config_AD();
    start_lcd();
    __enable_interrupt();
      a = 3*pow(10,-12);
      b = 4*pow(10,-8);
      c = 2*pow(10,-4);
      d = 0.317;
      e = 355.47;

/*
 * ********************************* AREA DE INICIALIZACAO DO DISPLAY ********************************
 */
    enviar_lcd(0, 0xC0);
    cursor = 0;
    while (cursor<9)
    {
        enviar_lcd(1, medida[cursor]);                //Escreve texto na linha 2
        cursor = cursor + 1;
    }

/*
* ********************************* INICIALIZACAO DOs TIMERS ****************************************
*/

    TA0CTL |=MC_3;                                      // INICIA o timer0, modo UP/down
    TA1CTL |=MC_1;                                      // INICIA o timer1, modo UP
    IE1 |= WDTIE;                                       //Habilita interrupcao do WATCHDOG

    while(1)
    {
        enviar_lcd(0, 0xC9);
        enviar_lcd(1,num_sensor[rpm/1000]);
        enviar_lcd(1,num_sensor[(rpm/100)%10]);
        enviar_lcd(1,num_sensor[(rpm/10)%10]);
        enviar_lcd(1,num_sensor[rpm%10]);
        if (sentido == 0)
            {

                enviar_lcd(0, 0x80);
                enviar_lcd(1,0x3E);
                enviar_lcd(0, 0x81);
                enviar_lcd(1,0x3E);
            }
       else
            {
               enviar_lcd(0, 0x80);
               enviar_lcd(1,0x3C);
               enviar_lcd(0, 0x81);
               enviar_lcd(1,0x3C);
            }
        if(P1IN & BIT4)
        {
            WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
            TA1CCR1 = duty;
            enviar_lcd(0, 0x83);
            enviar_lcd(1,0x44);         //D
            enviar_lcd(1,0x49);         //I
            enviar_lcd(1,0x47);         //G
            enviar_lcd(1,0x20);         //_
            enviar_lcd(1,0x53);         //S
            enviar_lcd(1,0x54);         //T
            enviar_lcd(1,0x45);         //E
            enviar_lcd(1,0x50);         //P
            enviar_lcd(1,0x20);         //_
            enviar_lcd(1,num_sensor[dig_1]);
            menu = 1;
        }
        else
        {
            WDTCTL = WDT_MDLY_8;
            enviar_lcd(0, 0x83);
            enviar_lcd(1,0x41);         //A
            enviar_lcd(1,0x4E);         //N
            enviar_lcd(1,0x41);         //A
            enviar_lcd(1,0x4C);         //L
            enviar_lcd(1,0x4F);         //O
            enviar_lcd(1,0x47);         //G
            enviar_lcd(1,0x49);         //I
            enviar_lcd(1,0x43);         //C
            enviar_lcd(1,0x4F);         //O
            enviar_lcd(1,0x20);         //_
            enviar_lcd(1,0x20);         //_
            menu = 0;
        }


    }

}


/*
 * ************************** SEÇAO DAS INTERRUPCOES *************************************************
 */
//////////////// INTERRUPCAO DA PORTA 2 //////////////////////

#pragma         vector=PORT2_VECTOR
__interrupt     void    interr_P2(void)
{

        if(P2IFG & BIT1)
        {
            if(menu==1)
            {
              dig_1++;
                  if(dig_1==7)
                  {
                      dig_1=1;
                  }
            }
        }

    if(P2IFG & BIT2)
    {
            if (sentido == 0)
            {
            saida_1 = 0;
            saida_2 = 1;

            sentido = 1;
            }
            else
            {
            saida_1 = 1;
            saida_2 = 0;

            sentido = 0;
            }
    }

    if(P2IFG & BIT0)
    {
        P1OUT &= ~BIT1;
        P1OUT &= ~BIT2;

        if (saida_1 == 1)
        {
            P1OUT |= BIT1;                          //P1.1 = 1
        }
        if (saida_2 == 1)
        {
            P1OUT |= BIT2;
        }
        if(menu==1)
        {
            rotacao = (dig_1*1000);
            duty = ((a*pow(rotacao,4))-(b*pow(rotacao,3))+(c*pow(rotacao,2))-(d*rotacao))+e;
            TA1CCR1 = duty;
        }
    }

    P2IFG &= ~(BIT0+BIT1+BIT2);                     // limpa flags de P2
}


//////////////// INTERRUPCAO DO T0A0 //////////////////////

#pragma         vector=PORT1_VECTOR
__interrupt     void    interr_P1(void)
{
     contador++;                                    // Cada pulso do sensor causa uma interrupção em P1 e incrementa o contador
     P1IFG &=~BIT0;                                 //limpa o flag de interrupcao P1.0
}

//////////////// INTERRUPCAO DO T0A0 //////////////////////

#pragma     vector = TIMER0_A0_VECTOR
__interrupt void interr_T0_A(void)
{
    if (contador>999)
            {
                contador=999;
            }

    rpm = (contador*10);                        // (contador*120)/8 -> 120*0.5segundos = 1 min  e  contador/4 = 1 rotacao .
    contador= 0;

}
//////////////// INTERRUPCAO DO T1A0 //////////////////////
#pragma         vector=TIMER1_A0_VECTOR         //interrupcao de comparacao 0
__interrupt     void    interr0_T1_A(void)       //apenas 1 interrupcao
{
        P2OUT |= BIT3;                          //P2.3 = 1

}

//////////////// INTERRUPCAO DO T1A1 //////////////////////
#pragma         vector=TIMER1_A1_VECTOR         //interrupcao de comparacao 1
__interrupt     void    interr1_T1_A(void)       //3 interrupcao = precisa resetar flag de interrup.
{
        P2OUT &= ~BIT3;                         //P2.3 = 0
        TA1CCTL1 &= ~CCIFG  ;                     //flag de interrupcao = 0 (no interrupt. pending)
}

//////////////// INTERRUPCAO DO ADC //////////////////////

#pragma         vector= ADC10_VECTOR        //interrupcao do ADC
__interrupt     void    interr_ADC(void)
{
       TA1CCR1 = ADC10MEM;
}

/////////////////// INTERRUPCAO DO WATCHDOG COMO INTERVAL TIMER ///////////////

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    while(!(ADC10CTL1 & ADC10BUSY))        //ADC LIBERADO? SIM->entra no while
                                           //ADC10CTL=BIT0, se =1 -> nao liberado
    {
        ADC10CTL0 |= ENC;                     //INICIO CONVERSAO
        ADC10CTL0 |=ADC10SC;

    }
}
