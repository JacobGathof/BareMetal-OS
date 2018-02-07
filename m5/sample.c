
int main(){

  enableInterrupts();

  interrupt(0x21, 0, "Sample program running\n\r", 0, 0);

  while(1){

  }
}
