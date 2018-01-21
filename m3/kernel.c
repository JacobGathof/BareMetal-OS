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
void printEntireString(char *, int);
void readString(char *);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
int getInterruptX(int, int);
void handleInterrupt21(int,int,int,int);
int readFile(char*, char*);
void executeProgram(char*, int);
void terminate();


int main(){

  makeInterrupt21();
  interrupt(0x21, 4, "shell\0", 0x2000, 0);

}

void terminate(){

  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';

  interrupt(0x21, 4, shell, 0x2000, 0);
}

void executeProgram(char* name, int seg){
  char buffer[13312];
  int c;
  int found = 0;
  found = readFile(name, buffer);

  if(!found){
    return;
  }

  c = 0;
  for(c = 0; c < 13312; c++){
    putInMemory(seg, c, buffer[c]);
  }

  launchProgram(seg);
  interrupt(0x21, 5, 0, 0, 0);

}


int readFile(char* filename, char* buffer){
  char buffer2[512];
  int i;
  int sectors[26];
  int count;
  int c;
  int bc;
  int found = 1;

  readSector(buffer2, 2);
  for(i = 0; i < 16; i++){
    int start = i*32;
    int j;
    found = 1;
    for(j = 0; j < 6; j++){
      if(filename[j] != buffer2[j+start]){
        found = 0;
        break;
      }
      if(filename[j] == 0){
        found = 1;
        break;
      }
    }
    if(found == 1){
      break;
    }
  }

  if(found == 0){
    return 0;
  }

  count = 32*i+6;
  c = 0;
  while(buffer2[count] != 0 && c < 26){
    sectors[c++] = (int)(buffer2[count++]);
  }
  c = 0;
  bc = 0;
  while(sectors[c] != 0 && c < 26){
    int sector;
    sector = sectors[c];
    readSector(buffer+512*c, sector);
    c++;
  }

  return 1;

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

void printEntireString(char * str, int len){
    char current;
    int c;
    for(c = 0; c < len; c++){
      char val = 0xe;
      current = str[c];
      interrupt(0x10, getInterruptX(val,current), 0, 0, 0);
    }
}

void readString(char* buffer){
  int count = 0;
  char currentKey = '\0';
  while(1){
    currentKey = interrupt(0x16, 0, 0, 0, 0);
    interrupt(0x10, getInterruptX(0xe, currentKey), 0, 0, 0);

    if(currentKey == BACKSPACE){
      if(count > 0){
        count--;
        interrupt(0x10, getInterruptX(0xe, ' '), 0, 0, 0);
        interrupt(0x10, getInterruptX(0xe, BACKSPACE), 0, 0, 0);
      }
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
  }else if(ax == 3){
    readFile(bx, cx);
  }else if(ax == 4){
    executeProgram(bx, cx);
  }else if(ax == 5){
    terminate();
  }else{
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
