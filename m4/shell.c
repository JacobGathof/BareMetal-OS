/*
  Jacob Gathof, Eric Moorman
  Team 3 - Sultans of Swing
*/

void makeCommand(char*, char*, char*);
void processCommand(char*);
int strEqual(char*, char*);
int div(int,int);

int main(){

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
  }

  makeCommand(cmd, cmdBuffer, arg);

  if(strEqual(cmdBuffer, "type")){
    interrupt(0x21, 3, arg, response, 0);
    interrupt(0x21, 0, response, 0, 0);
  }
  else if(strEqual(cmdBuffer, "execute")){
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
  else if(strEqual(cmdBuffer, "dir")){
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
  else{
    interrupt(0x21, 0, "    Bad Command!\n\r", 0, 0);
  }

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
