/*
  Jacob Gathof, Eric Moorman
  Team 3 - Sultans of Swing
*/

#define VID_MEM 0xB8000
#define BASE_ADDR 0x8000
#define ENTER 0xd
#define BACKSPACE 0x8
/*
  Base address altered to show message
  at the top left corner of the screen
*/

void printString(char *);
void readString(char *);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
int getInterruptX(int, int);
void handleInterrupt21(int,int,int,int);

int main(){

  char line[512];

  printString("Hello World\0");

  printString("Enter a line: \0");
  readString(line);
  printString(line);

  readSector(line, 30);
  printString(line);

  makeInterrupt21();
  printString("Enter message using interrupt handler: ");
  interrupt(0x21, 1,line,0,0);
  interrupt(0x21, 0,line,0,0);

  while(1){
  }
}

void readSector(char* buffer, int sector){
  int rel = mod(sector, 18)+1;
  int head = mod(div(sector, 18), 2);
  int track = div(sector, 36);
  interrupt(0x13, getInterruptX(2, 1), buffer, getInterruptX(track, rel), getInterruptX(head, 0));
}


void printString(char * str){

  char current = str[0];
  int c = 0;
  while(current!='\0'){
      char val = 0xe;
      interrupt(0x10, getInterruptX(val,current), 0, 0, 0);
    c++;
    current = str[c];
  }
}

void readString(char* buffer){
  int count = 0;
  char currentKey = '\0';
  while(1){
    currentKey = interrupt(0x16, 0, 0, 0, 0);

    if(currentKey == BACKSPACE){
      if(count > 0)
        count--;
    }

    else if(currentKey == ENTER){
      buffer[count++] = 0xa;
      buffer[count++] = 0x0;
      break;
    }

    else{
      buffer[count++] = currentKey;
    }

  }
}

void handleInterrupt21(int ax,int bx,int cx,int dx){

  if(ax == 0){
    printString(bx);
  }else if(ax == 1){
    readString(bx);
  }else if(ax == 2){
    readSector(bx, cx);
  }else{
    printString("Error thrown from interrupt 21\0");
  }
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

int getInterruptX(int h, int l){
  return 256*h + l;
}
