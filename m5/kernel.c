/*
  Jacob Gathof, Eric Moorman
  Team 3 - Sultans of Swing
*/

#define ENTER 0xd
#define BACKSPACE 0x8

typedef struct{
  int active;
  int sp;
  int waiting;
  char name[6];
} process;

process processTable[8];
int currentProcess;
int currFore;
int currBack;

void printString(char *);
void readString(char *);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void writeSector(char*, int);
int getInterruptX(int, int);
void handleInterrupt21(int,int,int,int);
int readFile(char*, char*);
int executeProgram(char*);
void terminate();
int strEqualL(char*, char*, int);
void deleteFile(char*);
void writeFile(char*, char*, int);
void printDirectory();
void getSize(char*, int*);
void handleTimerInterrupt(int, int);
int findFreeEntry(int);
int findNextRunning(int);
void killProcess(int);
void clearScreen();
void wait(int);
void stpKnl();
void actOnSector(int, char *, int);
void setScreenColor(int, int);
void printActiveProcesses();

int main(){
  int i = 0;

  clearScreen();
  /*interrupt(0x10, getInterruptX(0x0B,0), 0xe, 1, 0);*/

  for(i = 0; i < 8; i++){
    process p;
    p.active = 0;
    p.sp = 0xff00;
    p.waiting = -1;
    p.name[0] = '\0';
    processTable[i] = p;
  }

  /*drawLogo();*/

  currFore = 15;
  currBack = 0;

  currentProcess = -1;

  makeTimerInterrupt();
  makeInterrupt21();

  interrupt(0x21, 4, "shell\0", 0x2000, 0);

  setScreenColor(currBack, currFore);

  while(1){}

}

void setScreenColor(int b, int f){
  if(b == -1){
    currFore = f;
  }
  if(f == -1){
    currBack = b;
  }

  interrupt(0x10, 0x0700, currBack*4096 + currFore*256, 0x0000, 0x184f);
  interrupt(0x10, 0x0200, 0x0000, 0x0000, 0x0000);
}

void stpKnl(){
  interrupt(0x15, 0x5307, 0x0001, 0x0003, 0);
}

void wait(int waitOn){
  setKernelDataSegment();
  processTable[0].waiting = waitOn;
  restoreDataSegment();
}

void clearScreen(){
  interrupt(0x10, 0x0700, 0x0700, 0x0000, 0x184f);
  interrupt(0x10, 0x0200, 0x0000, 0x0000, 0x0000);
}

void killProcess(int i){
  int c = 0;
  setKernelDataSegment();
  processTable[i].active = 0;
  for(c = 0; c < 8; c++){
    if(processTable[c].waiting == i){
      processTable[c].waiting = -1;
    }
  }
  restoreDataSegment();
}

int findFreeEntry(int base){
  int i = base;
  int c = 0;
  setKernelDataSegment();
  for(c = 0; c < 8; c++){
    if(processTable[mod(c+i, 8)].active == 0){
      restoreDataSegment();
      return mod(c+i,8);
    }
  }
  restoreDataSegment();
  return -1;
}

int findNextRunning(int base){
  int i = base;
  int c = 1;
  setKernelDataSegment();
  for(c = 1; c < 9; c++){
    int d = mod(c+i, 8);
    if((processTable[d].active == 1) && (processTable[d].waiting == -1)){
      restoreDataSegment();
      return d;
    }
  }
  restoreDataSegment();
  return -1;
}

void handleTimerInterrupt(int seg, int sp){
  int nextFree = 0;
  int newSeg = 0;
  int newSp = 0;
  int currentSeg = seg;
  int current = div(seg,0x1000) - 2;
  int stackPointer = sp;

  nextFree = findNextRunning(current);

  setKernelDataSegment();
  if (currentProcess >= 0){
    processTable[currentProcess].sp = stackPointer;
  }

  if(nextFree == -1){
    restoreDataSegment();
    returnFromTimer(currentSeg, stackPointer);
  }

  currentProcess = nextFree;

  newSeg = (currentProcess+2)*0x1000;

  newSp = processTable[currentProcess].sp;
  restoreDataSegment();

  returnFromTimer(newSeg, newSp);
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
  int c = 0;
  setKernelDataSegment();
  processTable[currentProcess].active = 0;
  for(c = 0; c < 8; c++){
    if(processTable[c].waiting == currentProcess){
      processTable[c].waiting = -1;
    }
  }
  restoreDataSegment();

  while(1){
  }
}

int executeProgram(char* name){
  char buffer[13312];
  int c;
  int found = 0;
  int freeIndex;
  int freeI;
  int t = 0;
  char newName[6];
  while(t < 6){
    newName[t] = name[t];
    t++;
  }
  newName[t] = '\0';

  found = readFile(name, buffer);

  freeIndex = findFreeEntry(0);

  if(freeIndex == -1 || !found){
      return -1;
  }

  freeI = (freeIndex+2)*0x1000;

  c = 0;
  for(c = 0; c < 13312; c++){
    putInMemory(freeI, c, buffer[c]);
  }

  setKernelDataSegment();
  processTable[freeIndex].sp = 0xff00;
  processTable[freeIndex].active = 1;
  for(t = 0; t < 6; t++){
    processTable[freeIndex].name[t] = newName[t];
  }

  restoreDataSegment();
  initializeProgram(freeI);

  return freeIndex;
}

void printActiveProcesses() {
  unsigned int i;
  char * proc = "Yo";
  char * procn;
  char c;

  setKernelDataSegment();
  printString("Process #  Process Name\n\r\0");
  for (i = 0; i < 8; i++){
    if (processTable[i].active){
      c = (char)('0' + i);
      proc[0] = c;
      proc[1] = 0;
      procn = processTable[i].name;
      printString(proc);
      printString("          \0");
      printString(procn);
      printString("\n\r\0");
    }
  }
  restoreDataSegment();
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
  actOnSector(2, buffer, sector);
}

void writeSector(char* buffer, int sector){
  actOnSector(3, buffer, sector);
}

void actOnSector(int read, char * buffer, int sector){
  int rel = mod(sector, 18)+1;
  int head = mod(div(sector, 18), 2);
  int track = div(sector, 36);
  interrupt(0x13, getInterruptX(read, 1), buffer, getInterruptX(track, rel), getInterruptX(head, 0));
}

void printString(char * str){
  char current = str[0];
  int c = 0;
  while(current!='\0'){
    char val = 0xe;

    interrupt(0x10, getInterruptX(val,current), 0x1, 1, 0);

    c++;
    current = str[c];
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
  }else if(ax == -1){
    stpKnl();
  }else if(ax == 1){
    readString(bx);
  }else if(ax == 2){
    readSector(bx, cx);
  }else if(ax == 3){
    readFile(bx, cx);
  }else if(ax == 4){
    executeProgram(bx);
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
  }else if(ax == 10){
    killProcess(bx);
  }else if(ax == 11){
    clearScreen();
  }else if(ax == 12){
    getSize(bx, cx);
  }else if(ax == 13){
    int i = executeProgram(bx);
    wait(i);
  }else if(ax == 14){
    setScreenColor(bx, -1);
  }else if(ax == 15){
    setScreenColor(-1, bx);
  }else if (ax == 16){
    printActiveProcesses();
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
