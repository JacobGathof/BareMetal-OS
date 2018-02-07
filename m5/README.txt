Jacob Gathof, Eric Moorman 
Team 3 - Sultans of Swing

Build and run project by running compileOS.sh
Project uses qemu, bcc, as86, and ls86

Running the project will launch the shell.
Enter a string and press enter.
There are several commands available:

(type | cat) <filename> 	: display the contents of the provided file
(execute | run) <filename> 	: execute the provided file
(delete) <filename> 		: delete the provided file
(copy) <from> <to> 		: copy the contents of from into the file to
(dir | ls)			: list the contents of the directory
(create) <filename> 		: create a new file with the given filename
(kill) <process #> 		: Kill the given process
(reset) 			: clear the current console screen
(execforeground | ps) <filename> : execute the given process while blocking the shell
(set-b) <number>		: set the background color
(set-f) <number>		: set the foreground color
(status)			: list the active processes
(quit) 				: exit the OS


Bonus Features:
Image drawing - 64 x 64 images can drawn by the OS with the draw command
Status - displays all of the currently active processes and their names
Sector Display - when running dir, the number of sectors a file takes up is also displayed
Background Color - the set-b command can change the background color
Foreground Color - the set-f command can change the text color
Quit - the quit command causes the Operating System to terminate
Help - the help command shows the various commands that can be given to the shell along with what they do