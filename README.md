## Projeto - Controle de Rotação (malha aberta) motor DC com MSP430G 

Esse projeto foi realizado como trabalho final para disciplina de Microcontroladores e Microprocessadores.

O projeto consiste em medir e controlar a rotação do motor DC. Todos os resultados de medição e informações são mostrados em um display LCD 16x2. O ajuste de velocidade pode ser feito por dois sitemas distintos: 
1) Analogicamente, por meio de um potênciometro conectado. 
2) Ou por meio de pré-sets de velocidades atribuidos em botões tácteis.
###### O código desenvolvido utiliza recursos do msp430g:
 - Timers: gerar pulsos PWM, realizar calculo de medição em RPM.
 - ADC: converte o sinal analógico recebido do potênciometro.

###### Componentes utilizados:
- Microcontrolador MSP430G
- Display LCD 16x2
- Sensor Óptico foto interruptor
- Potênciometro
- Chaves tacteis (Push Buttons)


