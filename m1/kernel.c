/*
  Jacob Gathof, Eric Moorman
  Team 3 - TBD
*/

#define VID_MEM 0xB8000
#define BASE_ADDR 0x8000
/*
  Base address altered to show message
  at the top left corner of the screen
*/

void putStringInMemory(char *);

int main(){

  putStringInMemory("Hello World!\0");

  while(1){
  }
}


void putStringInMemory(char * str){

  char current = str[0];
  int c = 0;
  while(current!='\0'){
      putInMemory(0xB000, BASE_ADDR+(2*c), current);
      putInMemory(0xB000, BASE_ADDR+(2*c)+1, 0xB);
    c++;
    current = str[c];
  }

}
