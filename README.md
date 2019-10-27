# Unidade de Medições Inerciais - UFSAE
Desenvolvimento de uma unidade de medições inerciais para obtenção de aceleração e ângulos de roll, pitch e yaw de um veículo do tipo fórmula da Escuderia UFSAE.

## Mais detalhes
Projeto desenvolvido utilizando o microcontrolador ATMEGA328P, para o qual foi desenvolvido o firmware para medição de aceleração e velocidade angular do dispositivo MPU-6050 via comunicação I²C. Tais dados foram utilizados para obter dados de aceleração de um carro e ângulos de roll, pitch e yaw. Os valores lidos são gravados em um cartão de memória micro SD, que utiliza comunicação via SPI. 

## Hardware utilizado
- 1 Microcontrolador ATMEGA328P
- 1 Módulo MPU-6050 (Acelerômetro e Giroscópio)
- 1 Cartão micro SD
- 1 Módulo adaptador para cartão micro SD

## Software desenvolvido
- Leitura via I²C do MPU-6050
- Escrita via SPI no cartão micro SD
- Filtro Complementar para fusão de sensores
- Filtro Passa Altas para eixo z do giroscópio
