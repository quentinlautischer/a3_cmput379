#ifndef MAIN_H 
#define MAIN_H

#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>


#include "select_servermod.h"
#include "clerkNanny.h"

#include "memwatch.h"

#endif

void teardown(void);
void signalCallbackHandler(int signum);
void signalNewConfig(int signum);

char *configFileName;
int killedProcesses;

char *msgData;
pid_t parent_pid;

int main(int argc, char* argv[]){

	signal(SIGINT, signalCallbackHandler);
	signal(SIGHUP, signalNewConfig);

	if ( argc == 3 ){ /* Ensure only one argument provided */
		configFileName = malloc(strlen(argv[2]) + 1);
		strcpy(configFileName, argv[2]);
	
		clerkNannySetup();
		clerkNannyParseConfigFile();
		serverBoot(argc, argv);

	}

	return EXIT_SUCCESS;
}

void teardown(void){
	clerkNannyTeardown();
}

void signalCallbackHandler(int signum){
	if(getpid() == parent_pid){
		sprintf(msgData, "Info: Caught SIGINT. Exiting cleanly. %d process(es) killed.", killedProcesses);
		clerkNannyReceiveData(msgData, BOTH);	
	}
	serverTeardown();
	teardown();
	exit(signum);
}

void signalNewConfig(int signum){
	clerkNannyParseConfigFile();
	clerkNannySendNewConfigToClients();
}



