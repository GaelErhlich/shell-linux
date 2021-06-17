/* Initialisation */
#include "./core/init.c"

/* Functions */
#include "commands/headers/handler.h"



int main(int argc, char* argv[]) {
	init_shell();

	char* aString = "Ah";
	char* bString;
	handleCommand();
	//handleCommand();
	
	return 0;
}