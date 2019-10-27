/*==================================================== NOTAS ======================================================
 * O MPU possui filtros passa-baixas de 260Hz no acelerômetro e 256Hz no giroscópio.
 * O MPU faz a aquisição do acelerômetro a 1kHz e do giroscópio a 8kHz.
 * A velocidade da comunicação I²C é de 100khz ou 400khz no modo rápido
 * A escala do acelerômetro pode ser alterada entre 2g, 4g, 8g e 16g. 
   Para alterar, use a função sensor.setFullScaleAccelRange(uint8_t range);
   0 -> +/- 2  g
   1 -> +/- 4  g
   2 -> +/- 8  g
   3 -> +/- 16 g
 * A escala do giroscópio pode ser alterada entre 250, 500, 1000, 2000 °/s.
   Para alterar, use a função sensor.setFullScaleGyroRange(uint8_t range); 
   0 -> +/- 250  °/s
   1 -> +/- 500  °/s
   2 -> +/- 1000 °/s
   3 -> +/- 2000 °/s
    
 * A conexão do MPU com o arduino é feita da seguinte forma: 
   SCL -> A5; 
   SDA -> A4;
   GND -> GND;
   VDD -> 3.3V(Preferencialmente). Pode ser utilizado o 5V. 
*/
#include <I2Cdev.h>
#include <MPU6050.h>   
#include <Wire.h>  
#include "gravar.h"



#define FREQ        100         // Frequência de exibição em Hz
#define SCALE_GYRO  131         // Escala de sensibilidade do giroscópio  | 250-> 131  | 500-> 65.5|1000-> 32.8|2000->16.4|
#define SCALE_ACCEL 16384       // Escala de sensibilidade do acelerômetro| 2g -> 16384| 4g -> 8192| 8g -> 4096| 16g->2048|
#define ALPHA_X     0.006
#define X_OFFSET    0.006
#define ALPHA_Y     0.05
#define Y_OFFSET_ACCEL    0.2
#define Y_OFFSET_GYRO     0.01

MPU6050 sensor;
int   ax, ay, az,
      gx, gy, gz;
unsigned long time_prev = 0;
float ax_g, ay_g, az_g,                     // Valores do acelerômetro em G
      ax_deg, ay_deg,                       // Valores do acelerômetro em ângulo(graus).
      ax_deg_old,ay_deg_old,az_deg_old,
      gx_dps, gy_dps, gz_dps,               // Valores do giroscópio em °/s
      gx_old, gy_old, gz_old,               // Valores da medida anterior de g_dps
      gx_deg, gy_deg, gz_deg,               // Valores do giroscópio em ângulo(graus).
      gx_deg_old, gy_deg_old, gz_deg_old,   // Valores da medida anterior de g_deg
      ang_x, ang_y, ang_z, dt;                                   // Intervalo de tempo entre medidas
const unsigned int period  = 65536 - (16000000/256/FREQ);

void setup() {
  Serial.begin(2000000);              // Inicia a comunicação serial com BaudRate de 2Mbits/s
  Wire.begin();                       // Inicia protocolo I²C
  Wire.setClock(400000);              // Seleciona clock de 400kHz para protocolo I²C
  sensor.initialize();                // Inicializa o MPU-6050

  // ---------Inicializar a gravaçao-----------//

  pinMode(7,OUTPUT); 
  pinMode(6,INPUT);
    
  Serial.println("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  //Serial.println("initialization done...");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //myFile = SD.open("newtest.txt", FILE_WRITE);

  // SOFTWARE INTERRUPTION
  PCICR = bit(PCIE2); // ENABLE EXTERNAL INTERRUPT 2
  PCIFR = bit(PCIF2); // FLAG 
  PCMSK2 = bit(PCINT23); // PIN 7 ENABLE  

  //---------------------------------------Início da configuração do TIMER1--------------------------------------.
  TCCR1A = 0;                         // Configura timer para operação normal pinos OC1A e OC1B desconectados  |
  TCCR1B = 0;                         // Limpa registrador                                                     |
  TCCR1B |= (0<<CS10)|(1 << CS12);    // Configura prescaler para 1024: CS12 = 1 e CS10 = 0                    |
  TCNT1 = period;                     // Inicia o timer com valor para que estouro ocorra em 1/FREQ            |
  TIMSK1 |= (1 << TOIE1);             // Habilita a interrupção do TIMER1                                      |
  //---------------------------------------Fim da configuração do TIMER1-----------------------------------------'
  time_prev = micros();               // Atualização da referência de tempo para cálculo de dt

}

void loop(){
  sensor.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);// Leitura do MPU-6050, dos dados de acelerômetro e giroscópio
  ax_g = (float)ax/SCALE_ACCEL;              // Conversão dos dados de aceleração para G
  ay_g = (float)ay/SCALE_ACCEL;              //
  az_g = (float)az/SCALE_ACCEL;              // ----------------------------------------
  gx_dps = (float)gx/SCALE_GYRO;             // Conversão dos dados do giroscópio para °/s
  gy_dps = (float)gy/SCALE_GYRO;             //
  gz_dps = (float)gz/SCALE_GYRO;             // ------------------------------------------

  
  
  if(digitalRead(6) == 1){
        myFile.close();
        Serial.println("gravou");
        cli();
      }
}

ISR(TIMER1_OVF_vect)                                                  //Interrupção de TIMER1. Ocorre a cada (1/FREQ) segundos 
{
  TCNT1 = period;                                                     // Renicia TIMER
  dt = (float)(micros() - time_prev)/1000000.0;                       // cálculo de tempo entre medidas em segundos
// Início do cálculo dos ângulos pelo giroscópio-----------
  gx_deg += 0.5*(gx_dps + gx_old)*dt;                                 // Cálculo integral de posição angular no eixo x;
  gx_old = gx_dps;                                
  gx_deg = (1*gx_deg - ALPHA_X*gx_deg_old);// + X_OFFSET;             // Filtro Passa-Altas feito no olho
  gx_deg_old = gx_deg;                            

  gy_deg += 0.5*(gy_dps + gy_old)*dt;                                 // Cálculo integral de posição angular no eixo y;
  gy_old = gy_dps;                                
  gy_deg = (1*gy_deg - ALPHA_Y*gy_deg_old) + Y_OFFSET_GYRO;           // Filtro Passa-Altas feito no olho
  gy_deg_old = gy_deg;                            
    
  gz_deg += 0.5*(gz_dps + gz_old)*dt;                                 // Cálculo integral de posição angular no eixo z;
  gz_old = gz_dps;  
  gz_deg = (1*gz_deg - 0.006*gz_deg_old) + 0.013;
  gz_deg_old = gz_deg;
  
  time_prev = micros();                                               // Atualização da referência de tempo para cálculo de dt
// Fim do cálculo dos ângulos pelo giroscópio -------------------------------------------------------------------------------
// Início do cálculo dos ângulos pelo acelerômetro --------------------------------------------------------------------------
//  ax_deg = atan(ay_g/sqrt(pow(ax_g,2) + pow(az_g,2)))*(180.0/3.14);         // Cálculo dos ângulos do acelerômetro por arcotangente
//  ay_deg = atan(-ax_g/sqrt(pow(ay_g,2) + pow(az_g,2)))*(180.0/3.14);        //  "   "
  ax_deg = ax_g;
  ay_deg = ay_g;
  
  ax_deg = (ALPHA_X*ax_deg + (1.0 - ALPHA_X)*ax_deg_old);             // Filtro Passa-Baixas feito na intuição
  ay_deg = (ALPHA_Y*ay_deg + (1.0 - ALPHA_Y)*ay_deg_old) + Y_OFFSET_ACCEL;  // Filtro Passa-Baixas feito na intuição
  ax_deg_old = ax_deg;
  ay_deg_old = ay_deg;
  ang_x = ax_deg + gx_deg;                                            // Complemento mágico
  ang_y = ay_deg + gy_deg;                                            // Complemento mágico
  ang_z = gz_deg;                                                     // Angulo no eixo z não possui complemento
// Fim do cálculo dos ângulos pelo acelerômetro ------------------------------------------------------------------------------

  //Serial.print(tempo_atan_fim - tempo_atan_ini);Serial.print("\t");
  //------Impressão dos valores lidos do MPU------.
 
  //Serial.print("ax_deg:");
  Serial.print(gx_dps);Serial.print("\t");    //|
  //Serial.print("ay_deg:");
  Serial.print(gy_dps);Serial.print("\t");   //|
  Serial.println(gz_dps);//Serial.println("\t");   //|
//  Serial.print(gx_deg);Serial.print("\t");  //|
//  Serial.print(gy_deg);Serial.print("\t");  //|
//  Serial.println(gz_deg);                   //|
//-------------Fim da impressão-----------------'
  
//-------------Inicio da gravação------------//
 
  enfileirar((uint16_t)(ax_g*1000));
  enfileirar((uint16_t)(ay_g*1000));
  enfileirar((uint16_t)(az_g*1000));
  enfileirar((uint16_t)(gx_dps*100));
  enfileirar((uint16_t)(gy_dps*100));
  enfileirar((uint16_t)(gz_dps*100));  
  enfileirar((uint16_t)(gz_dps*100));
  enfileirar((uint16_t)(gz_dps*100));
}
