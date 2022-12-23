#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "parse.h"
#include<fcntl.h>
#include "stringFunctions.c"

#define MAXLINE 81




void sigint_handler(int signum){
}
 


int main()
{

	char input [MAXLINE];
	char directory[MAXLINE] = "/home/";
	
	parseInfo* command;
	struct node * head = NULL;
	static int commandNum = 0;
	static int localPid = 0;
 	int status;	
	

	chdir(directory);	
	while(1){
		
		//printf("newCommand %s \n", newCommand);
		//Display Prompt				
		printf("Type help for Built-In Commands or if you are having trouble. Enter Command #%d:%s ", commandNum, directory);
	
		//Input Command
		fgets(input, MAXLINE, stdin);
		
		//Parse the command args
		command = malloc(sizeof(parseInfo));
		command = parse(&input);
		//print_info(command);
				
		
		
		//!CMD
		char *cmd = "!";
		if ( my_strcmp(command->CommArray[command->pipeNum].command, cmd) == 0){
			char newCommand [81] = "";
			if (command->CommArray[command->pipeNum].VarNum == 1){
				printf("Please enter command number as well!\n");
				continue;
				}
			_get(&head, atoi(command->CommArray[command->pipeNum].VarList[1]), newCommand);
			command = parse(&newCommand);
			//print_info(command);
		}
		
		//exit
		char *exitt = "exit";
		char *force = "exit!";
		if (my_strcmp(command->CommArray[command->pipeNum].command, exitt) == 0){
			int j = _checkBackground(head);
			if (j!=0){//If a background process is runnning we can't exist shell.
				printf("Cannot exit since %d background process are running.\n", j);
				commandNum++;
			}
			else
			{
			//Kill all child processes if there is no current background process.
			_killall(head);
			exit(1);		
			}
		}
		
		if (my_strcmp(command->CommArray[command->pipeNum].command, force) == 0){//Force exit
			//Kill all child processes if there is no current background process.
			_killall(head);
			exit(1);		
		}
		
		//CD
		char *cd = "cd";
		if ( my_strcmp(command->CommArray[command->pipeNum].command, cd) == 0){
			char directory2[MAXLINE] = "";
			char path[MAXLINE] = "";
			
			for (int i = 1; i < command->CommArray[command->pipeNum].VarNum; i++){
				if (i>1)mystrconcatinate(path, " ");
				mystrconcatinate(path, command->CommArray[command->pipeNum].VarList[i]);
				
			}
			printf("%s\n", path);
			
			mystrcopy(directory, directory2);
			mystrconcatinate(directory2, path);
			
			
			if (chdir(path) == 0){	
				//mystrcopy(command->CommArray[command->pipeNum].VarList[1],directory);
				printf("\n");
				printf("directory changed to %s\n", path);
				printf("\n");
				
				
			  	char str2[40];

			  	strncpy ( str2, path, 5);
			  	str2[5] = '\0';
		
				if (my_strcmp("/home", str2) == 0)//For cd related to home directory.
				{
					mystrcopy(path, directory);
					char str3[40];
				}
				else
				{
					mystrcopy(directory2, directory);
				}
			  	if (directory[strlen(directory)-1] != '/')
					mystrconcatinate(directory, "/");
			}
			else//If cd to a non-existing path.
				printf("\nInvalid directory. Use ls or pwd command to check where you are. For path changes relevant to home directory use /home/path.\n\n");	
		}
		
				
		//Signal: Sending signal to indicate termination or completion of a task.
		signal(SIGCHLD, sigint_handler);
		
		//Pipe
		int fd[2];
		pipe(fd);
		
		int pid = fork();
		if (!command->boolBackground){//Inserting with -1 indicating not a background process
 			_insert(&head, commandNum, *command, pid, -1);
		}
		else//Inserting with local pid indicating background process
		{
			_insert(&head, commandNum, *command, pid, localPid);
			localPid++;
		}
	 	 
		if (pid < 0){
			// fork failed: exit
			fprintf(stderr, "fork failed\n");
			exit(1);
		}
		if (pid == 0) {
			//
			
			//Help
			char *help = "help";
			char *kil = "kill";
			char *history = "history";
			
			char *jobs = "jobs";
			if ( my_strcmp(command->CommArray[command->pipeNum].command, help) == 0){
				printf("\n");
				printf("The followig are the background process you can execute.\n\n1. jobs - provides a list of all background processes and their local pIDs.\n\n2. cd PATHNAME - sets the PATHNAME as working directory. For space seperated folders just add spaces as you would. For home directory path changes just use /home/path.\n\n3. history - prints a list of previously executed commands. \n\n4. kill PID - terminates the background process identified locally with PID in the jobs list.\n\n5. ! CMD - runs the command numbered CMD in the command history\n\n6. exit - terminates the shell only if there are no background jobs. Sometimes you will have to exit again to make sure the background list is updated.\n\n7. help - prints the list of builtin commands along with their description.\n\n8. exit! - force exits the shell\n");		
				printf("\n");
				exit(1);
			}
			
			
			//History
			else if ( my_strcmp(command->CommArray[command->pipeNum].command, history) == 0){
				_print(head);	
				exit(1);	
			}
	
			//Jobs
			else if ( my_strcmp(command->CommArray[command->pipeNum].command, jobs) == 0){
				printf("\n");
				_printBackground(head);		
				printf("\n");
				exit(1);
			}
				
			else if ( my_strcmp(command->CommArray[command->pipeNum].command, cd) == 0){
				
				exit(1);
			}
			
			else if (my_strcmp(command->CommArray[command->pipeNum].command, exitt) == 0){
				
				exit(1);
			}
	
			//Kill PID		
			else if ( my_strcmp(command->CommArray[command->pipeNum].command, kil) == 0){
				int killPid = _getPid(head,atoi(command->CommArray[command->pipeNum].VarList[1]));
				if (killPid != -1){
					kill(killPid, SIGSEGV); 
					printf("\n");
					printf("killed %d\n",killPid);
					printf("\n");
				}
				else{
					printf("\n");
					printf("Invalid Local Pid!\n");
					printf("\n");
				}
				exit(1);
			}
			
			else{
				//IO Direction
				char fileLocation1 [MAXLINE];
				mystrcopy(directory, fileLocation1);
				char fileLocation2 [MAXLINE];
				mystrcopy(directory, fileLocation2);
				int in;
				int out;
				
				if (command->boolInfile){//Redirecting input stream
					mystrconcatinate(fileLocation1, command->inFile);
					
					in = open(fileLocation1, O_RDONLY | O_APPEND);
					if (in == -1){ 
						printf("Non existing input file!\n"); 
						exit(1);
					}
					
					int f = access(fileLocation1, R_OK);//Checking access
				    	if(f == -1){
						perror("Access Denied to input file!");
						exit(1);
				    	}
					    
					dup2(in, 0); 
					close(in);
				}
				if (command->boolOutfile){//Redirecting output stream
					mystrconcatinate(fileLocation2, command->outFile);
					out = open(fileLocation2, O_WRONLY | O_APPEND);
				    	
					if (out == -1) 
					{ 
					printf("Creating output file\n"); 
					fopen (command->outFile, "w");
					}
					
					int f = access(fileLocation2, W_OK);//Checking access
				        if(f == -1){
						printf("It looks like the output file exists but you don't have access to it :(");
						exit(1);
				    	}
					
					
					out = open(fileLocation2, O_WRONLY | O_APPEND);
					dup2(out, 1); 
					close(out);
				}
				
				
				
				//Piping
				if(command->pipeNum>0)
				{
					dup2(fd[1], 1);
				    	close(fd[0]);
				}
				
				//Exec
				int i = execvp(command->CommArray[0].VarList[0],command->CommArray[0].VarList);	
				
				//Checking for ambigous command
				if (i == -1)
				{
					printf("\n");
					printf("Exec failed! Invalid Command!\n");
					printf("\n");
					exit(1);
				}
			}
			
		}
		

		if(command->pipeNum>0){//If piping
			pid = fork();
			if(pid==0)
			{
				dup2(fd[0], 0);
				close(fd[1]);
				int i = execvp(command->CommArray[command->pipeNum].VarList[0],command->CommArray[command->pipeNum].VarList);	
				if (i == -1)
				{
					printf("\n");
					printf("Exec failed! Invalid Command!\n");
					printf("\n");
				}
			}
			else
			{
				close(fd[0]);
        			close(fd[1]);
        			_updateBackground(head, &status);
				if (!command->boolBackground){
				 	wait(NULL);
				}
		
			}
				    
		}
		
		sleep(1);//Added sleep so that the next prompt can wait in case of background processes.
		
		_updateBackground(head, &status);//Checking status of background processes and waiting for current process.
		
		//Background
		if (!command->boolBackground){
		 	wait(NULL);
		}
		
		//Incrementing command num
		commandNum++;
	}

	return 0;
}
