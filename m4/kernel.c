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
void writeSector(char*, int);
int getInterruptX(int, int);
void handleInterrupt21(int,int,int,int);
int readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
int strEqual(char*, char*);
int strEqualL(char*, char*, int);
void deleteFile(char*);
void writeFile(char*, char*, int);
void printDirectory();
void drawLogo();
void getSize(char*, int*);

int main(){
  interrupt(0x10, 0x0700, 0x0700, 0x0000, 0x184f);
  interrupt(0x10, 0x0200, 0x0000, 0x0000, 0x0000);

  /*drawLogo();*/


  makeInterrupt21();
  interrupt(0x21, 4, "shell\0", 0x2000, 0);

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
  /*interrupt(0x10, 0x0003, 0x0000, 0x0000, 0x0000);*/

}

void getSize(char* filename, int* cc){
  char dir[512];
  int i = 0;
  int k = 0;
  int start = 0;
  int count = 0;
  readSector(dir, 2);

  for(i = 0; i < 16; i++){
    start = i*32;
    if(strEqualL(&dir[start], filename, 6)){
      k = 6;
      while(dir[start+k]!= 0){
        count++;
        k++;
      }
      *cc = count;
      return;
    }
  }
  *cc = 0;
  return;
}

void printDirectory(){
  char dir[512];
  int i = 0;
  int j = 0;
  int k = 0;
  int start = 0;
  readSector(dir, 2);

  for(i = 0; i < 16; i++){
    start = i*32;
    if(dir[start] != 0){
      char info[128];
      int count = 0;

      for(j = 0; j < 6; j++){
        info[j] = dir[start+j];
        if(info[j] == 0) info[j] = ' ';
      }
      info[j++] = ' ';

      k = 6;
      while(dir[start+k]!= 0){
        count++;
        k++;
      }

      if(count >= 10){
        info[j++] = div(count, 10) + '0';
        info[j++] = mod(count, 10) + '0';
      }else{
        info[j++] = count+'0';
      }

      info[j++] = '\n';
      info[j++] = '\r';
      info[j] = 0;

      printString(info);
    }
  }
}

void writeFile(char* name, char* buffer, int numberOfSectors){
  char map[512];
  char dir[512];
  int i = 0;
  int j = 0;
  int start = 0;
  readSector(map, 1);
  readSector(dir, 2);

  for(i = 0; i < 16; i++){
    start = i*32;
    if(dir[start]==0){
      break;
    }
  }
  if(i == 16){ return; }

  for(i=0; i < 6; i++){
    dir[start+i] = name[i];
  }

  start+=6;
  for(i=0; i < numberOfSectors; i++){
    while(map[j] != 0){
      j++;
    }
    map[j]=0xff;
    dir[start+i] = j;
    writeSector(buffer, j);
    buffer = &buffer[512];
  }

  for(i = numberOfSectors; i < 26; i++){
    dir[start+i] = 0;
  }

  writeSector(map, 1);
  writeSector(dir, 2);
}

void deleteFile(char* name){
  char map[512];
  char dir[512];
  int i = 0;
  int found = 0;
  int count = 0;
  int c = 0;

  readSector(map, 1);
  readSector(dir, 2);

  for(i = 0; i < 16; i++){
    int start = i*32;
    if(strEqualL(name, &dir[start], 6)){
      found = 1;
      dir[start] = 0;
      break;
    }
  }

  if(found == 0){
    return;
  }

  count = 32*i+6;
  c = 0;
  while(dir[count+c] != 0 && c < 26){
    map[dir[count+c]] = 0;
    c++;
  }

  writeSector(map, 1);
  writeSector(dir, 2);


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
    found = 0;
    if(strEqualL(filename, &buffer2[start], 6)){
      found = 1;
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

void writeSector(char* buffer, int sector){
  int rel = mod(sector, 18)+1;
  int head = mod(div(sector, 18), 2);
  int track = div(sector, 36);
  interrupt(0x13, getInterruptX(3, 1), buffer, getInterruptX(track, rel), getInterruptX(head, 0));
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
  }else if(ax == 6){
    writeSector(bx,cx);
  }else if(ax == 7){
    deleteFile(bx);
  }else if(ax == 8){
    writeFile(bx, cx, dx);
  }else if(ax == 9){
    printDirectory();
  }else if(ax == 12){
    getSize(bx, cx);
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

int strEqual(char* a, char* b){
  int i = 0;
  while(a[i] != 0 && b[i]!= 0){
    if(a[i] != b[i]){
      return 0;
    }
    i++;
  }
  return 1;
}

int strEqualL(char* a, char* b, int j){
  int i = 0;
  while(a[i] != 0 && b[i]!= 0){
    if(a[i] != b[i]){
      return 0;
    }
    i++;
    if(i == j){
      return 1;
    }
  }
  if(a[i] == 0 && b[i] == 0){
    return 1;
  }
  return 0;
}
