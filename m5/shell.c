/*
  Jacob Gathof, Eric Moorman
  Team 3 - Sultans of Swing
*/

void makeCommand(char*, char*, char*);
void processCommand(char*);
int strEqual(char*, char*);
int div(int,int);
void runGraphics(char*);
void getMap(char *, char*);
int mod(int, int);

int main(){
  enableInterrupts();

  /*interrupt(0x21, 13, "graphics", 0, 0);*/
  runGraphics("ghosty");

  while(1){
    char buffer[512];
    interrupt(0x21, 0, "S:>", 0, 0);
    interrupt(0x21, 1, buffer, 0, 0);
    interrupt(0x21, 0, "\n\r", 0, 0);
    processCommand(buffer);
  }
}

void makeCommand(char* cmd, char* cmdBuffer, char* arg){
  int i = 0;
  int j = 0;
  while(cmd[i] != ' ' && cmd[i] != 0){
    cmdBuffer[i] = cmd[i];
    i++;
  }
  i++;
  while(cmd[i] != 0 && cmd[i] != '\n'){
    arg[j++] = cmd[i++];
  }
}

void processCommand(char* cmd){
  char cmdBuffer[64];
  char arg[512];
  char response[13312];
  int i;
  for (i = 0; i < 13312; i++){
    response[i] = 0;
    if (i < 512){
      arg[i] = 0;
    }
  }

  makeCommand(cmd, cmdBuffer, arg);

  if(strEqual(cmdBuffer, "type") || strEqual(cmdBuffer, "cat")){
    interrupt(0x21, 3, arg, response, 0);
    interrupt(0x21, 0, response, 0, 0);
  }
  else if(strEqual(cmdBuffer, "execute") || strEqual(cmdBuffer, "run")){
    interrupt(0x21, 4, arg, 0x2000, 0);
  }
  else if(strEqual(cmdBuffer, "delete")){
    interrupt(0x21, 7, arg, 0, 0);
  }
  else if(strEqual(cmdBuffer, "copy")){
    char arg2[512];
    char arg3[512];
    int len;
    makeCommand(arg, arg2, arg3);
    interrupt(0x21, 3, arg2, response, 0);
    interrupt(0x21, 12, arg2, &len, 0);
    interrupt(0x21, 8, arg3, response, len);
  }
  else if(strEqual(cmdBuffer, "dir") || strEqual(cmdBuffer, "ls")){
    interrupt(0x21, 9, 0, 0, 0);
  }
  else if(strEqual(cmdBuffer, "create")){
    char readBuffer[512];
    int p = 0;
    int q = 0;
    int i = 0;
    char totalBuffer[13312];

    for(i = 0; i < 13312; i++){
      totalBuffer[i] = 0;
    }

    while(readBuffer[0]!='\n'){

      for(i = 0; i < 512; i++){
        readBuffer[i] = 0;
      }

      interrupt(0x21, 0, "Input text: ", 0, 0);
      interrupt(0x21, 1, readBuffer, 0, 0);
      interrupt(0x21, 0, "\n\r", 0, 0);

      p=0;
      while(readBuffer[p]!=0){
        totalBuffer[q++] = readBuffer[p++];
      }
      totalBuffer[q++] = '\r';
    }

    interrupt(0x21, 8, arg, totalBuffer, div(q, 512)+1);
  }
  else if(strEqual(cmdBuffer, "kill")){
    interrupt(0x21, 10, arg[0] - '0', 0, 0);
  }
  else if(strEqual(cmdBuffer, "reset")){
    interrupt(0x21, 11, 0, 0, 0);
  }
  else if(strEqual(cmdBuffer, "execforeground") || strEqual(cmdBuffer, "ps")){
    interrupt(0x21, 13, arg, 0, 0);
  }
  else if(strEqual(cmdBuffer, "quit")){
    interrupt(0x21, -1, 0, 0, 0);
  }
  else if(strEqual(cmdBuffer, "set-b")){
    interrupt(0x21, 14, arg[0]-'0', 0, 0);
  }
  else if(strEqual(cmdBuffer, "set-f")){
    interrupt(0x21, 15, arg[0]-'0', 0, 0);
  }
  else if(strEqual(cmdBuffer, "help") || strEqual(cmdBuffer, "sos")){
    interrupt(0x21, 0,"(type | cat) <filename> : display the contents of the provided file\n\r", 0, 0);
    interrupt(0x21, 0,"(execute | run) <filename> : execute the provided file\n\r", 0, 0);
    interrupt(0x21, 0,"(delete) <filename> : delete the provided file\n\r", 0, 0);
    interrupt(0x21, 0,"(copy) <from> <to> : copy the contents of from into the file to\n\r", 0, 0);
    interrupt(0x21, 0,"(dir | ls)\n\r", 0, 0);
    interrupt(0x21, 0,"(create) <filename> : create a new file with the given filename\n\r", 0, 0);
    interrupt(0x21, 0,"(kill) <process #> : Kill the given process\n\r", 0, 0);
    interrupt(0x21, 0,"(reset) : clear the current console screen\n\r", 0, 0);
    interrupt(0x21, 0,"(execforeground | ps) <filename> : execute the given process while blocking the shell\n\r", 0, 0);
    interrupt(0x21, 0,"(quit) : exit the OS\n\r", 0, 0);
    interrupt(0x21, 0,"(set-b) <number> : set background color\n\r", 0, 0);
    interrupt(0x21, 0,"(set-f) <number> : set foreground color\n\r", 0, 0);
    interrupt(0x21, 0,"(draw) <filename> : draw an image\n\r", 0, 0);
  }
  else if(strEqual(cmdBuffer, "status")){
    interrupt(0x21, 16,0, 0, 0);
  }
  else if(strEqual(cmdBuffer, "draw")){
      runGraphics(arg);
  }
  else{
    interrupt(0x21, 0, "    Bad Command!\n\r", 0, 0);
  }

}

void runGraphics(char* name) {
  int i=0;
  int j = 0;
  int a=0;
  int b=0;
  char image[512];
  int counter= 0;
  char buffer[512];
  getMap(name, image);


  interrupt(0x10, 0x000d, 0x0000, 0x0000, 0x0000);

  for (i = 0; i < 512; i++) {
    char currentChar = image[i];
    for (j = 1; j <= 128; j*=2){
      char col = 15;
      if ((currentChar & j) == 0){
        col = 0;
      }
      for(a = 0; a < 2; a++){
        for(b =0; b < 2; b++){
            interrupt(0x10, 0x0C00+col, 0x0000, mod(counter,64) * 2+a+100, div(counter, 64) * 2+b);
        }
      }
      counter++;
    }
  }
  interrupt(0x21, 1, buffer, 0, 0);
  interrupt(0x10, 0x0003, 0x0000, 0x0000, 0x0000);
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
