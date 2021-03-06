#include <stdio.h>
#include <string.h>

// PDA definition
struct PDA
{
	char currentstate;
	char initialstate;
	char finalstates[5];
};

// End of PDA definition

// Stack implementation
char stack[100];
int stacktop = -1;

void pushStack(char c)
	{stack[++stacktop] = c; stack[stacktop+1]='\0';}

void popStack()
	{stacktop--; stack[stacktop+1]='\0';}

char* topStack(int l)
	{return stack+stacktop-l+1;}

int isStackEmpty()
	{return stacktop==-1;}
// End of stack implementation

// Stack implementation
int treestack[10];
int treestacktop = -1;

void pushTreeStack(int c)
	{treestack[++treestacktop] = c;}

void popTreeStack()
	{treestacktop--;}

int topTreeStack()
	{return treestack[treestacktop];}

int isTreeStackEmpty()
	{return treestacktop==-1;}
// End of stack implementation

// Rule and ruleset definition
struct rule
{
	char state;
	char stacktop[5];
	char input;
	char newstate;
	char newstacktop[5];
	char label[3][10];
};

#define RULETABLESIZE 7
typedef struct rule ruleset[RULETABLESIZE];
// End of rule and ruleset definition

char* revstring(char *string)
{
	int l = strlen(string);
	int i;
	char temp;
	for(i=0;i<l/2;i++)
	{
		temp = string[i];
		string[i] = string[l-i-1];
		string[l-i-1] = temp;
	}
	return string;
}

int active = 0;
int processInput(ruleset RS, struct PDA *p, char input)
{
	//printf("ENTER:%c %c %s\n",input,(*p).currentstate,topStack(1));
	int i,l,j,parent;
	for(i=0; i<RULETABLESIZE; i++)
	{
		if(RS[i].state==(*p).currentstate && RS[i].input==input)
		{
			l = (RS[i].stacktop[0] == 'e') ? (0) : (strlen(RS[i].stacktop));
			if(strcmp(RS[i].stacktop,topStack(l))==0 || RS[i].stacktop[0]=='e')
			{
				(*p).currentstate = RS[i].newstate;
				for(j=0; j<l; j++)
					popStack();
				l = (RS[i].newstacktop[0] == 'e') ? (0) : (strlen(RS[i].newstacktop));
				for(j=l-1; j>=0; j--)
					pushStack(RS[i].newstacktop[j]);
				
				if(i==0)
				{
					FILE* fp = fopen("notes.txt","a");
					fprintf(fp,"%s ",RS[i].label[0]);
					fclose(fp);
					pushTreeStack(active++);
				}
				else
				{
					parent = topTreeStack();
					popTreeStack();
					active += l;
					for(j=l;j>=0;j--)
					{
						if(RS[i].label[j][0]=='<')
							pushTreeStack(active);
						active--;
					}
					
					for(j=0;j<=l;j++)
					{
						FILE* fp = fopen("notes.txt","a");
						fprintf(fp,"%d %s ",parent,RS[i].label[j]);
						fclose(fp);
						active++;
					}
					active++;
				}
				return i;
			}
		}
	}
	return -1;
}

int isAccepted(struct PDA p)
{
	int i;
	// Check if stack is empty
	if(stacktop==-1)
	{
		for(i=0; i<strlen(p.finalstates); i++)
		{
			if(p.currentstate == p.finalstates[i])
				return 1;
		}
	}
	return 0;
}

void fillRules(ruleset RS)
{
	RS[0].state = 'p';
	RS[0].stacktop[0] = 'e'; RS[0].stacktop[1] = '\0';
	RS[0].input = 'e';
	RS[0].newstate = 'q';
	RS[0].newstacktop[0] = 'S', RS[0].newstacktop[1] = '\0';
	strcpy(RS[0].label[0],"S");
	
	RS[1].state = 'q';
	RS[1].stacktop[0] = 'S'; RS[1].stacktop[1] = '\0';
	RS[1].input = 'a';
	RS[1].newstate = 'q';
	RS[1].newstacktop[0] = 'A', RS[1].newstacktop[1] = 'B', RS[1].newstacktop[2] = '\0';
	strcpy(RS[1].label[0],"a");
	strcpy(RS[1].label[1],"<A>");
	strcpy(RS[1].label[2],"<B>");
	
	RS[2].state = 'q';
	RS[2].stacktop[0] = 'S'; RS[2].stacktop[1] = '\0';
	RS[2].input = 'b'; 
	RS[2].newstate = 'q';
	RS[2].newstacktop[0] = 'B', RS[2].newstacktop[1] = 'A', RS[2].newstacktop[2] = '\0';
	strcpy(RS[2].label[0],"b");
	strcpy(RS[2].label[1],"<B>");
	strcpy(RS[2].label[2],"<A>");
	
	RS[3].state = 'q';
	RS[3].stacktop[0] = 'A'; RS[3].stacktop[1] = '\0';
	RS[3].input = 'b';
	RS[3].newstate = 'q';
	RS[3].newstacktop[0] = 'S', RS[3].newstacktop[1] = '\0';
	strcpy(RS[3].label[0],"b");
	strcpy(RS[3].label[1],"<S>");
	
	RS[4].state = 'q';
	RS[4].stacktop[0] = 'A'; RS[4].stacktop[1] = '\0';
	RS[4].input = 'a';
	RS[4].newstate = 'q';
	RS[4].newstacktop[0] = '\0';
	strcpy(RS[4].label[0],"a");
	
	RS[5].state = 'q';
	RS[5].stacktop[0] = 'B'; RS[5].stacktop[1] = '\0';
	RS[5].input = 'a';
	RS[5].newstate = 'q';
	RS[5].newstacktop[0] = 'S', RS[3].newstacktop[1] = '\0';
	strcpy(RS[5].label[0],"a");
	strcpy(RS[5].label[1],"<S>");
	
	RS[6].state = 'q';
	RS[6].stacktop[0] = 'B'; RS[6].stacktop[1] = '\0';
	RS[6].input = 'b';
	RS[6].newstate = 'q';
	RS[6].newstacktop[0] = '\0';
	strcpy(RS[6].label[0],"b");
}

int main(int argc, char *argv[])
{
	// PDA defined
	struct PDA p;
	p.initialstate = 'p';
	p.finalstates[0] = 'q';
	p.finalstates[1] = '\0';
	p.currentstate = p.initialstate;
	// Stack is made initally empty (above)

	// Input string taken from argument
	char input[100];
	if(argc==1)
		input[0] = '\0';
	else
		strcpy(input, argv[1]);
	FILE* fp = fopen("notes.txt","w");
	fclose(fp);
	
	// Define rules
	ruleset ruletable;
	fillRules(ruletable);
	printf("%-10s %-10s %-20s %-15s %-20s %-12s\n","","State","Leftover Input","Stack","Transition No.","Transition");
	
	// Read input character and process by rule table
	int i,l,n=0;
	l = strlen(input);
	// Force initial null transition

	printf("%-10s %-10c %-20s %-15s ","",p.currentstate,input,topStack(stacktop+1));
	processInput(ruletable, &p, 'e');
	printf("%-20d (%c,%c,%s)->(%c,%s)\n",n,
	ruletable[n].state, ruletable[n].input, ruletable[n].stacktop, ruletable[n].newstate,
	strcmp(ruletable[n].newstacktop,"")==0?"e":ruletable[n].newstacktop);
	sleep(1);
	
	for(i=0; i<l; i++)
	{
		sleep(1);
		printf("%-10s %-10c %-20s %-15s ","",p.currentstate,input+i,revstring(topStack(stacktop+1)));
		revstring(topStack(stacktop+1));
		n = processInput(ruletable, &p, input[i]);
		if(n==-1)
			break;
		printf("%-20d (%c,%c,%s)->(%c,%s)\n",n,
		ruletable[n].state, ruletable[n].input, ruletable[n].stacktop, ruletable[n].newstate,
		strcmp(ruletable[n].newstacktop,"")==0?"e":ruletable[n].newstacktop);
	}
	
	printf("\n%-10s %-10c %-20s %-15s ","",p.currentstate,input+i,revstring(topStack(stacktop+1)));
	
	if(input[0]=='\0')
		input[0]='e', input[1]='\0';
		
	if(isAccepted(p) && i==l)
		printf("\n%s was accepted.\n",input);
	else
		printf("\n%s was rejected.\n",input);
		
	return 0;
}
