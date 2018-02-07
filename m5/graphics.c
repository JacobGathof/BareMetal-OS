int div(int , int);
int mod(int, int);
void drawLogo();
void draw();
void getMap(char *, char*);

int main(){
  char buffer[512];

  enableInterrupts();
  draw();
  interrupt(0x21, 1, buffer, 0, 0);
  interrupt(0x10, 0x0003, 0x0000, 0x0000, 0x0000);
  interrupt(0x21, 0, "S:>", 0, 0);
  interrupt(0x21, 5, 0, 0, 0);
}

void draw() {
  int i=0;
  int j = 0;
  int a=0;
  int b=0;
  char image[512];
  int counter= 0;
  getMap("picture", image);


  interrupt(0x10, 0x000d, 0x0000, 0x0000, 0x0000);

  for (i = 0; i < 128; i++) {
    char currentChar = image[i];
    for (j = 1; j <= 128; j*=2){
      char col = 15;
      if ((currentChar & j) == 0){
        col = 0;
      }
      for(a = 0; a < 5; a++){
        for(b =0; b < 5; b++){
            interrupt(0x10, 0x0C00+col, 0x0000, mod(counter,32)*5+a+100, div(counter, 32)*5+b);
        }
      }
      counter++;
    }
  }
}

void drawLogo(){
  int i=0;
  int j=0;
  int a=0;
  int b=0;
  char* c[10];
  c[0] = "----------------";
  c[1] = "AAAA------------";
  c[2] = "A---------------";
  c[3] = "AAAA-AAAAAA-----";
  c[4] = "---A-A----A-----";
  c[5] = "AAAA-A----A-AAAA";
  c[6] = "-----A----A-A---";
  c[7] = "-----AAAAAA-AAAA";
  c[8] = "---------------A";
  c[9] = "------------AAAA";

  interrupt(0x10, 0x000d, 0x0000, 0x0000, 0x0000);
  for(i = 0; i < 16; i++){
    for(j = 0; j < 10; j++){
      char* cc = c[j];
      int col = 0x7;
      if(cc[i] == 'A'){
        col = 0xa;
      }
      if(cc[i] == '-'){
        continue;
      }
      for(a = mod(j,2); a < 20; a+=2){
        for(b = mod(a,2); b < 20; b+=2){
          interrupt(0x10, 0x0C00+col, 0x0000, i*20+a, j*20+b);
        }
      }
    }
  }
}

void getMap(char * filename, char* buf) {
  interrupt(0x21, 3, filename, buf, 0);
}

int mod(int a, int b){
  while (a >= b){
    a = a - b;
  }
  return a;
}

int div(int a, int b){
  int quotient = 0;
  while ((quotient + 1) * b  <= a){
    quotient  = quotient  + 1;
  }
  return quotient;
}
