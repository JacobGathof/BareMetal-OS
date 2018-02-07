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


