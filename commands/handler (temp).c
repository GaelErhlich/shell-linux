/*
#include "headers/handler.h"

#include <stdio.h>
#include <string.h>

#include "../datastructures/headers/job.h"


void handleCommand(char* cmd) {

	//struct job newJob = { .flag = true, .value = 123, .stuff = 0.456 };
	// TODO : Fill newJob
	

	char title[32];
	char args[1024];

	size_t iCmd = 0;
	while (cmd[iCmd] != '\0') {


		// Skipping spaces after the title
		while (cmd[iCmd] == ' ') {
			iCmd++;
		}

		// Title of the command
		size_t iTitl = 0;
		while (cmd[iCmd] != ' ' && cmd[iCmd] != '|' && cmd[iCmd] != '\0')
		{
			title[iTitl] = cmd[iCmd];
			iCmd++;
			iTitl++;
		}
		title[iTitl] = '\0';


		// Skipping spaces after the title
		while (cmd[iCmd] == ' ') {
			iCmd++;
		}


		// Copying arguments
		size_t iArgs = 0;
		while (cmd[iCmd] != '|' && cmd[iCmd] != '\0')
		{
			args[iArgs] = cmd[iCmd];
			iCmd++;
			iArgs++;
		}
		args[iArgs] = '\0';
		if (cmd[iCmd] == '|') {
			iCmd++;
		}
		

		printf("Command : %s, Arguments : %s\n", title, args);
		

	}


	printf("qeqchose : %ld (*) ou %ld\n", sizeof(*cmd), sizeof(cmd) );
}
*/