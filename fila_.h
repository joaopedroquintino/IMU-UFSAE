#ifndef fila_h
#define fila_h

#define MAXSIZE 256

uint8_t fila[MAXSIZE]; // 
int i = 0;
void enfileirar(uint16_t dados){
  int8_t lb = (uint8_t)((uint16_t)(dados)  & 0x00FF);   
  int8_t hb = (uint8_t)((uint16_t)(dados)>>8); 
    fila[i] = lb;
    i++;
    fila[i] = hb;
    i++;
    if(i == MAXSIZE)
    {
      i = 0;
      digitalWrite(7,!digitalRead(7));
    }
}
#endif fila_h
