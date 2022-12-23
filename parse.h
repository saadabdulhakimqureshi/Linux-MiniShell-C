#define MAX_VAR_NUM 11
#define PIPE_MAX_NUM 11
#define FILE_MAX_SIZE 41
#define MY_Q_H


struct commandType {
  char *command;
  char *VarList[MAX_VAR_NUM];
  int VarNum;
};

/* parsing information structure */
typedef struct {
  int   boolInfile;		       /* boolean value - infile specified */
  int   boolOutfile;		       /* boolean value - outfile specified */
  int   boolBackground;		       /* run the process in the background? */

  struct commandType CommArray[PIPE_MAX_NUM];
  int   pipeNum;
  char  inFile[FILE_MAX_SIZE];	       /* file to be piped from */
  char  outFile[FILE_MAX_SIZE];	       /* file to be piped into */
} parseInfo;

/* the function prototypes */
parseInfo *parse(char *);
void free_info(parseInfo *);
void print_info(parseInfo *);

struct node {
	parseInfo val;
	int pid;
	int localPid;
	struct node *next;
};

void _insert(struct node ** headaddr, int x, parseInfo val, int pid, int localPid);
void _remove(struct node ** headaddr, int x);
void _print (struct node * head);
void _printBackground(struct node * head);
void _updateBackground(struct node * head, int *); 
int _checkBackground(struct node * head); 
void _printPid(struct node * head, int pid);
void _set(struct node * head, int localPid);
int _getPid(struct node * head, int pid);
void _killall(struct node * head);
void _get(struct node ** headaddr, int x, char * returnString);
