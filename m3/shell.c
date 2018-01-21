/*
  Jacob Gathof, Eric Moorman
  Team 3 - Sultans of Swing
*/

void makeCommand(char*, char*, char*);
void processCommand(char*);
int strEqual(char*, char*);

int main(){
  char buffer[512];

  while(1){
    interrupt(0x21, 0, "S:>", 0, 0);
    interrupt(0x21, 1, buffer, 0, 0);
    interrupt(0x21, 0, "\n\r", 0, 0);
    processCommand(buffer);
  }
}

void makeCommand(char* cmd, char* cmdBuffer, char* arg){
  int i = 0;
  int j = 0;
  while(cmd[i] != ' '){
    cmdBuffer[i] = cmd[i];
    i++;
  }
  i++;
  while(cmd[i] != 0){
    arg[j++] = cmd[i++];
  }
}

void processCommand(char* cmd){
  char cmdBuffer[64];
  char arg[512];

  char response[13312];



  makeCommand(cmd, cmdBuffer, arg);

  if(strEqual(cmdBuffer, "type")){
    interrupt(0x21, 3, arg, response, 0);
    interrupt(0x21, 0, response, 0, 0);
  }
  else if(strEqual(cmdBuffer, "execute")){
    interrupt(0x21, 4, arg, 0x2000, 0);
  }
  else{
    interrupt(0x21, 0, "    Bad Command!\n\r", 0, 0);
  }

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
