/******************************************************************************* 
 ******************************************************************************* 
 *   parse.c  -  The parsing skeleton of your small shell                   
 *
 *   Syntax:     myshell command1 [ < infile] [ | command ]* [ > outfile] [&]       
 *
 *   parse a new line 
 *
 *   Accepts: nothing
 *   Returns: parse information structure
 * 
 *******************************************************************************
 *******************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define MAXLINE 81


/* -----------------------------------------------------------------------------
init_info()
DESCRIPTION:  Initializes info structure
-------------------------------------------------------------------------------*/
void init_info( parseInfo *p )
{
  int i;
  p->boolInfile=0;
  p->boolOutfile=0;
  p->boolBackground=0;
  p->pipeNum=0;

  for( i=0; i<PIPE_MAX_NUM; i++ )
  {
    	p->CommArray[i].command=NULL;
    	p->CommArray[i].VarList[0]=NULL;
    	p->CommArray[i].VarNum=0;
  }
}


/* -----------------------------------------------------------------------------
parse_command()
DESCRIPTION:  
-------------------------------------------------------------------------------*/
int parse_command( char * command, struct commandType *comm )
{
  int i=0;
  int pos=0;
  char word[MAXLINE];

  comm->VarNum=0;
  comm->command=NULL;
  comm->VarList[0]=NULL;

  while( isspace(command[i]) )  // skip over spaces 
    	i++; 


  if( command[i] == '\0' )   	// end of word
    	return 1;

  while( command[i] != '\0' )	// while not end of word
  {
  	while( command[i] != '\0'  && !isspace(command[i]) )
	{
      		word[pos++] = command[i++];
    	}
    	word[pos]='\0';

    	if( comm->VarNum == MAX_VAR_NUM )
    	{
      		fprintf( stderr, "Too many arguments to command.\n" );
      		return 0;
    	}

    	comm->VarList[comm->VarNum] = malloc((strlen(word)+1)*sizeof(char));
    	strcpy( comm->VarList[comm->VarNum], word );
    	comm->VarNum++;
    	word[0]='\0';
    	pos=0;
    	while( isspace(command[i]) )
      		i++; 
  }
  comm->command=malloc((strlen(comm->VarList[0])+1)*sizeof(char));
  strcpy(comm->command, comm->VarList[0]);
  comm->VarList[comm->VarNum]=NULL;
  return 1;
}



/* -----------------------------------------------------------------------------
parse_info()

DESCRIPTION:   Takes in a string cmdline, and returns a pointer to a struct
parseInfo. The members of parseInfo can be seen in parse.h.  Commands
are always stored in the array CommArray. 

This can accommodate multiple pipes. In commandType, command is the 
executable name and VarList is the argv to pass to the program.  cmdline 
can end either with '\n' or '\0'.

TODO: Need to be modulariszed a bit more (not required)
-------------------------------------------------------------------------------*/
parseInfo *parse( char *  cmdline )
{
  parseInfo *Result;
  int i=0;
  int pos;
  int end=0;
  char command[MAXLINE];
  int com_pos;
  int iscommproper = 0;

  if( cmdline[i] == '\n' && cmdline[i] == '\0' )
    return NULL;

  Result = malloc(sizeof(parseInfo));
  if( Result == NULL)
	{
    	return NULL;
  	}

  init_info( Result );
  com_pos=0;
  while (cmdline[i] != '\n' && cmdline[i] != '\0') 
  {
    	if( cmdline[i] == '&' )
	{
      		Result->boolBackground=1;

      		if( cmdline[i+1] != '\n' && cmdline[i+1] != '\0') 
		{
			fprintf(stderr, "Ignore anything beyond &.\n");
      		}

      		break;
    	}

    	else if (cmdline[i] == '<') 
	{
      		Result->boolInfile +=1;
      		while( isspace( cmdline[++i] ) );
      		pos=0;
      		while (cmdline[i] != '\0' && !isspace(cmdline[i])) 
		{
			if( pos==FILE_MAX_SIZE )
			{
	  			fprintf(stderr, "Error. The input redirection "
					"file name exceeds the size limit 40\n");
	  			free_info( Result );
	  			return NULL;
			}
			Result->inFile[pos++] = cmdline[i++];
      		}

      		Result->inFile[pos]='\0';
      		end = 1;

      		while( isspace(cmdline[i])) 
		{
		if( cmdline[i] == '\n' )
	  		break;
		i++;
      		}
    	}

    	else if (cmdline[i] == '>') 
	{
      		Result->boolOutfile +=1;
      		while (isspace(cmdline[++i]));
      		pos=0;
      		while( cmdline[i] != '\0' && !isspace(cmdline[i])) 
		{
			if( pos==FILE_MAX_SIZE) 
			{
	  			fprintf( stderr, "Error.The output redirection file "
					"name exceeds the size limit 40\n");
	  			free_info(Result);

	  			return NULL;
			}
			Result->outFile[pos++] = cmdline[i++];
      		}

      		Result->outFile[pos]='\0';
      		end = 1;
      		while( isspace(cmdline[i])) 
		{
			if( cmdline[i] == '\n' )
	  			break;
			i++;
      		} 
    	}

    	else if (cmdline[i] == '|') 
	{
      		command[com_pos]='\0';
      		iscommproper = parse_command( command, 
					&Result->CommArray[Result->pipeNum]);
      		if( ! iscommproper ) 
		{
			free_info( Result );
			return NULL;
      		}

      		com_pos = 0;
     		end = 0;
      		Result->pipeNum++;
      		i++;
    	}

    	else 
	{
      		if( end == 1 )
		{
	 		fprintf( stderr, "Error.Wrong format of input\n" );
	 		free_info( Result );
	 		return NULL;
      		}

      		if( com_pos == MAXLINE-1 )
		{
			fprintf( stderr, "Error. The command length exceeds "
				"the limit 80\n" );
			free_info( Result );
			return NULL;
      		}

      	command[com_pos++] = cmdline[i++];
    	}
  }

  command[com_pos]='\0';

  iscommproper = parse_command( command, &Result->CommArray[Result->pipeNum] );
  if ( ! iscommproper ) 
  {
    	free_info(Result);
    	return NULL;
  }

  //Result->pipeNum++;
  return Result;
}


/* -----------------------------------------------------------------------------
print_info()
DESCRIPTION:  
-------------------------------------------------------------------------------*/
void print_info (parseInfo *info) 
{
  int i, j;
  struct commandType *comm;

  if (NULL == info)
  {
    	printf("Null info\n");
    	return;
  }
  printf("Parse struct:\n\n");
  printf("# of pipes:%d\n", info->pipeNum);

  for( i=0; i<=info->pipeNum;i++ ) 
  {
    	comm=&(info->CommArray[i]);
    	if( (NULL == comm) || (NULL == comm->command))
	{
      		printf("Command %d is NULL\n", i);
    	} 
	else 
	{
      		printf("Command %d is %s.\t", i+1, comm->command);
      		printf("Number of Arguments: %d\n", comm->VarNum);
      		for (j=0; j<comm->VarNum; j++) 
		{
			printf("Arg %d: %s ", j, comm->VarList[j]);
      		}
      	printf("\n");
    	}
  }
  printf("\n");

  if (info->boolInfile)
  {
    	printf("infile: %s\n", info->inFile);
  }
  else 
  {
    	printf("no input redirection.\n");
  }

  if (info->boolOutfile) 
  {
    	printf("outfile: %s\n", info->outFile);
  }
  else 
  {
    	printf("no output redirection.\n");
  }
  if (info->boolBackground)
  {
    	printf("Background process.\n");
  } 
  else 
  {
    	printf("Foreground process.\n");
  }
}
  
/* -----------------------------------------------------------------------------
free_info()
DESCRIPTION:  
-------------------------------------------------------------------------------*/
void free_info (parseInfo *info) 
{
  int i,j;
  struct commandType *comm;

  if( NULL == info) return;
  for( i=0; i<PIPE_MAX_NUM;i++ ) 
  {
    	comm=&(info->CommArray[i]);
    	for (j=0; j<comm->VarNum; j++) 
	{
      		free(comm->VarList[j]);
    	}
    	if (NULL != comm->command)
	{
      		free(comm->command);
    	}
  }
  free(info);
}


void _insert(struct node ** headaddr, int x, parseInfo val, int pid, int localPid)
{
	if(*headaddr == NULL && x == 0)
		{
			struct node* newnode = malloc(sizeof(struct node));
			newnode-> val = val;
			newnode->pid = pid;
			newnode->localPid = localPid;
			*headaddr = newnode;
			
		}
	else
	{
		int i = 0;
		struct node* pointer = *headaddr;
		while (i < x-1 && pointer->next != NULL)
		{
			pointer = pointer->next;
			i++;
		}

		struct node* newnode = malloc(sizeof(struct node));
		newnode-> val = val;
		newnode->pid = pid;
		newnode->localPid = localPid;
		newnode->next = pointer->next;
		pointer->next = newnode;
	}
}

void _remove(struct node ** headaddr, int x)
{
	if (headaddr!=NULL){
		if (x == 0){
			struct node * pointer = *headaddr;
			pointer = pointer->next;
			*headaddr = pointer;
		}
		else{
			int i = 0;
			
			struct node *pointer = *headaddr;
			while (i < x-1 && pointer->next->next != NULL)
			{
				pointer = pointer->next;
				i++;
			}
			pointer->next = pointer->next->next;
			
		}

	}
}

void _killall(struct node * head)
{
	if (head==NULL)
		fprintf(stdout, "No ongoing processes.\n");

	else {
		while (head!=NULL){
			//printf("killing pid : %d\n", head->pid);
			kill (head->pid, SIGSEGV);
			head = head->next;	
		}
		
	}
}

void _print(struct node * head) {
	if (head==NULL)
		fprintf(stdout, "empty queue\n");

	else {
		int i = 0;
		printf("\n");
		while (head!=NULL){
			if (head->localPid != -1) printf("localPid : %d ", head->localPid);
			fprintf(stdout, "cmd %d pid : %d, command : ", i, head->pid);
			for (int j = 0 ; j < head->val.pipeNum+1; j++){
				if (j > 0) printf("| ");
				for (int i = 0; i < head->val.CommArray[j].VarNum; i++){
					printf("%s ", head->val.CommArray[j].VarList[i]);
				}
			}
			if (head->val.boolInfile)
				printf("%s ", head->val.inFile);
			if (head->val.boolOutfile)
				printf("%s ", head->val.outFile);
				
			//print_info(&head->val);
			printf("\n");
			head = head->next;
			i++;
		}
		printf("\n");
		
	}
}

void _printPid(struct node * head, int pid) {
	if (head==NULL)
		fprintf(stdout, "empty queue\n");

	else {
		while (head!=NULL){
			if (pid == head->pid)
				printf("Command %s pid %d localpid %d completed", head->val.CommArray[0].command, head->pid, head->localPid);
			head=head->next;
		}
		
	}
}


void _printBackground(struct node * head) {
	if (head==NULL)
		fprintf(stdout, "empty queue\n");

	else {
		int i = 0;
		while (head->next!=NULL){
			if (head->localPid != -1){
				fprintf(stdout, "%d pid : %d, localPid : %d, command : ", i, head->pid, head->localPid);
				
				for (int j = 0 ; j < head->val.pipeNum+1; j++){
					for (int i = 0; i < head->val.CommArray[j].VarNum; i++){
						printf("%s ", head->val.CommArray[j].VarList[i]);
					}	
				}
				printf("\n");
			}
				head = head->next;
		}
		
	}
}

void _updateBackground(struct node * head, int * status) {
	if (head==NULL)
		fprintf(stdout, "empty queue\n");

	else {
		int i = 0;
		while (head->next!=NULL){
			if (head->localPid != -1){
				int pid = waitpid(head->pid,status,WNOHANG);
				if (pid > 0){
					printf("exited pid %d local pid %d with status %d command ", pid, head->localPid, *status);
					for (int i = 0; i < head->val.CommArray[head->val.pipeNum].VarNum; i++){
						printf("%s ", head->val.CommArray[head->val.pipeNum].VarList[i]);
					}
					printf("\n");
					head->localPid = -1;
				}
				
			}
			head = head->next;
		}
		
	}
}

int _checkBackground(struct node * head) {
	if (head==NULL){
		fprintf(stdout, "empty queue\n");
		return 0;
		}
	else {
		int i = 0;
		while (head!=NULL){
			if (head->localPid != -1){
				i+=1;
				
			}
			head=head->next;
		}
		return i;
	}
}


void _set(struct node * head, int pid) {
	if (head==NULL)
		fprintf(stdout, "empty queue\n");

	else {
		printf("%d\n", pid);
		while (head!=NULL){
			if (head->pid == pid){
				head->localPid = -1;
				printf("%d\n", head->localPid);
			}
			head = head->next;
		}
	}
}

int _getPid(struct node * head, int localPid){
	if (head==NULL)
	return -1;

	else {
		while (head!=NULL){
			if (head->localPid == localPid)
			{ 
				head->localPid = -1;
				return head->pid;
			}
			head = head->next;
		}
		
	}
	return -1;
}

void strcon(char str1 [], char str2[])
{
    int i = 0;
    while(str1[i] != '\0'){
        i++;
    }
    int j = 0;
    while (str2[j] != '\0')
    {
    	str1[i] = str2[j];
    	i++;
    	j++;
    } 
    str1[i] = '\0';
}

void _get(struct node ** headaddr, int x, char * returnString)
{
	if (headaddr!=NULL){
		char * space = " ";
		
		int i = 0;
		
		struct node *pointer = *headaddr;
		while (i < x && pointer != NULL)
		{
			pointer = pointer->next;
			i++;
		}	
		for (int j = 0 ; j < pointer->val.pipeNum+1; j++){
			if (j > 0) strcon(returnString, "|" );
			for (int i = 0; i < pointer->val.CommArray[j].VarNum; i++){
				strcon(returnString, pointer->val.CommArray[j].VarList[i]);
				strcon(returnString, space);
			}
			
		}
		
		if (pointer->val.boolInfile){
			strcon(returnString, pointer->val.inFile);
			strcon(returnString, space);
		}
		if (pointer->val.boolOutfile)
			strcon(returnString, pointer->val.outFile);
			
		
	}
}
