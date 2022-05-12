## Projeto - Controle de Rotação (malha aberta) de motor DC com Microcontrolador MSP430G 

Esse projeto foi realizado como trabalho final para disciplina de Microcontroladores e Microprocessadores.

O projeto consiste em medir e controlar a rotação do motor DC. Todos os resultados de medição e informações são mostrados em um display LCD 16x2. O ajuste de velocidade pode ser feito por dois sitemas distintos: 
1) Analogicamente, por meio de um potênciometro. 
2) Ou por meio de pré-sets de velocidades atribuidos em botões tácteis.
###### O código desenvolvido utiliza recursos do msp430g:
 - Timers: gerar pulsos PWM, realizar cálculo de medição em RPM.
 - ADC: converte o sinal analógico recebido do potênciometro.

###### Componentes utilizados:
- Microcontrolador MSP430G
- Display LCD 16x2
- Sensor Óptico foto interruptor
- Potênciometro
- Chaves tacteis (Push Buttons)

  
  
![Projeto_Foto](https://user-images.githubusercontent.com/97235503/168105059-b1aa714f-cf34-4f9c-9cc9-06bf57b18c03.png)
