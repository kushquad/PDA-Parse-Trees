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

#define RULETABLESIZE 26
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
			//printf("%d HELLO",i);
			l = (RS[i].stacktop[0] == 'e') ? (0) : (strlen(RS[i].stacktop));
			//printf("%d %s\n",l,RS[i].stacktop);
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
	//printf("%d %c %c\n",stacktop,p.finalstates[0],p.currentstate);
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
	strcpy(RS[0].label[0],"<S>");
	
	RS[1].state = 'q';
	RS[1].stacktop[0] = 'S'; RS[1].stacktop[1] = '\0';
	RS[1].input = 'F';
	RS[1].newstate = 'q';
	RS[1].newstacktop[0] = 'o', RS[1].newstacktop[1] = '\0';
	strcpy(RS[1].label[0],"for");
	strcpy(RS[1].label[1],"<open>");
	
	
	RS[2].state = 'q';
	RS[2].stacktop[0] = 'o'; RS[2].stacktop[1] = '\0';
	RS[2].input = '(';
	RS[2].newstate = 'q';
	RS[2].newstacktop[0] = 'b', RS[2].newstacktop[1] = '\0';
	strcpy(RS[2].label[0],"(");
	strcpy(RS[2].label[1],"<init>");
	
	RS[3].state = 'q';
	RS[3].stacktop[0] = 'b'; RS[3].stacktop[1] = '\0';
	RS[3].input = 'I';
	RS[3].newstate = 'q';
	RS[3].newstacktop[0] = 'd', RS[3].newstacktop[1] = '\0';
	strcpy(RS[3].label[0],"int");
	strcpy(RS[3].label[1],"<dec>");
	
	RS[4].state = 'q';
	RS[4].stacktop[0] = 'b'; RS[4].stacktop[1] = '\0';
	RS[4].input = 'i';
	RS[4].newstate = 'q';
	RS[4].newstacktop[0] = 'E', RS[4].newstacktop[1] = '\0';
	strcpy(RS[4].label[0],"i");
	strcpy(RS[4].label[1],"<eq>");
	
	RS[5].state = 'q';
	RS[5].stacktop[0] = 'd'; RS[5].stacktop[1] = '\0';
	RS[5].input = 'i';
	RS[5].newstate = 'q';
	RS[5].newstacktop[0] = 'E', RS[5].newstacktop[1] = '\0';
	strcpy(RS[5].label[0],"i");
	strcpy(RS[5].label[1],"<eq>");
	
	RS[6].state = 'q';
	RS[6].stacktop[0] = 'E'; RS[6].stacktop[1] = '\0';
	RS[6].input = '=';
	RS[6].newstate = 'q';
	RS[6].newstacktop[0] = 'n', RS[6].newstacktop[1] = '\0';
	strcpy(RS[6].label[0],"=");
	strcpy(RS[6].label[1],"<num>");
	
	RS[7].state = 'q';
	RS[7].stacktop[0] = 'n'; RS[7].stacktop[1] = '\0';
	RS[7].input = 'D';
	RS[7].newstate = 'r';
	RS[7].newstacktop[0] = '\0';
	strcpy(RS[7].label[0],"digit");
	strcpy(RS[7].label[1],"<num>");

	RS[24].state = 'q';
	RS[24].stacktop[0] = 'n'; RS[24].stacktop[1] = '\0';
	RS[24].input = '+';
	RS[24].newstate = 'r';
	RS[24].newstacktop[0] = '\0';
	strcpy(RS[24].label[0],"+");
	strcpy(RS[24].label[1],"<numr>");
	
	RS[25].state = 'q';
	RS[25].stacktop[0] = 'n'; RS[25].stacktop[1] = '\0';
	RS[25].input = '-';
	RS[25].newstate = 'r';
	RS[25].newstacktop[0] = '\0';
	strcpy(RS[25].label[0],"-");
	strcpy(RS[25].label[1],"<numr>");
	
	RS[8].state = 'r';
	RS[8].stacktop[0] = 'e'; RS[8].stacktop[1] = '\0';
	RS[8].input = 'D';
	RS[8].newstate = 'r';
	RS[8].newstacktop[0] = '\0';
	strcpy(RS[8].label[0],"digit");
	strcpy(RS[8].label[1],"<numr>");

	RS[9].state = 'r';
	RS[9].stacktop[0] = '\0';
	RS[9].input = ';';
	RS[9].newstate = 'q';
	RS[9].newstacktop[0] = 'C'; RS[9].newstacktop[1] = '\0';
	strcpy(RS[9].label[0],";");
	strcpy(RS[9].label[1],"<crest>");

	RS[10].state = 'q';
	RS[10].stacktop[0] = 'C'; RS[10].stacktop[1] = '\0';
	RS[10].input = 'i';
	RS[10].newstate = 'q';
	RS[10].newstacktop[0] = 'c', RS[10].newstacktop[1] = '\0';
	strcpy(RS[10].label[0],"i");
	strcpy(RS[10].label[1],"<cond>");
	
	RS[11].state = 'q';
	RS[11].stacktop[0] = 'c'; RS[11].stacktop[1] = '\0';
	RS[11].input = '<';
	RS[11].newstate = 'q';
	RS[11].newstacktop[0] = 'N', RS[11].newstacktop[1] = '\0';
	strcpy(RS[11].label[0],"<");
	strcpy(RS[11].label[1],"<num>");
	
	RS[12].state = 'q';
	RS[12].stacktop[0] = 'c'; RS[12].stacktop[1] = '\0';
	RS[12].input = '>';
	RS[12].newstate = 'q';
	RS[12].newstacktop[0] = 'N', RS[12].newstacktop[1] = '\0';
	strcpy(RS[12].label[0],">");
	strcpy(RS[12].label[1],"<num>");
	
	RS[13].state = 'q';
	RS[13].stacktop[0] = 'N'; RS[13].stacktop[1] = '\0';
	RS[13].input = 'D';
	RS[13].newstate = 's';
	RS[13].newstacktop[0] = '\0';
	strcpy(RS[13].label[0],"digit");
	strcpy(RS[13].label[1],"<numr>");

	RS[22].state = 'q';
	RS[22].stacktop[0] = 'N'; RS[22].stacktop[1] = '\0';
	RS[22].input = '+';
	RS[22].newstate = 's';
	RS[22].newstacktop[0] = '\0';
	strcpy(RS[22].label[0],"+");
	strcpy(RS[22].label[1],"<numr>");
	
	RS[23].state = 'q';
	RS[23].stacktop[0] = 'N'; RS[23].stacktop[1] = '\0';
	RS[23].input = '-';
	RS[23].newstate = 's';
	RS[23].newstacktop[0] = '\0';
	strcpy(RS[23].label[0],"-");
	strcpy(RS[23].label[1],"<numr>");
	
	RS[14].state = 's';
	RS[14].stacktop[0] = 'e'; RS[14].stacktop[1] = '\0';
	RS[14].input = 'D';
	RS[14].newstate = 's';
	RS[14].newstacktop[0] = '\0';
	strcpy(RS[14].label[0],"digit");
	strcpy(RS[14].label[1],"<numr>");
	
	RS[15].state = 's';
	RS[15].stacktop[0] = 'e'; RS[15].stacktop[1] = '\0';
	RS[15].input = ';';
	RS[15].newstate = 'q';
	RS[15].newstacktop[0] = 'U', RS[15].newstacktop[1] = 'O', RS[15].newstacktop[2] = '\0';
	strcpy(RS[15].label[0],";");
	strcpy(RS[15].label[1],"<upd>");
	strcpy(RS[15].label[2],"<close>");
		
	RS[16].state = 'q';
	RS[16].stacktop[0] = 'U'; RS[16].stacktop[1] = '\0';
	RS[16].input = 'i';
	RS[16].newstate = 'q';
	RS[16].newstacktop[0] = 'u', RS[16].newstacktop[1] = '\0';	
	strcpy(RS[16].label[0],"i");
	strcpy(RS[16].label[1],"<oper>");
	
	RS[17].state = 'q';
	RS[17].stacktop[0] = 'u'; RS[17].stacktop[1] = '\0';
	RS[17].input = '+';
	RS[17].newstate = 'q';
	RS[17].newstacktop[0] = 'P', RS[17].newstacktop[1] = '\0';
	strcpy(RS[17].label[0],"+");
	strcpy(RS[17].label[1],"<plus>");
	
	RS[18].state = 'q';
	RS[18].stacktop[0] = 'P'; RS[18].stacktop[1] = '\0';
	RS[18].input = '+';
	RS[18].newstate = 'q';
	RS[18].newstacktop[0] = '\0';
	strcpy(RS[18].label[0],"+");
	
	RS[19].state = 'q';
	RS[19].stacktop[0] = 'u'; RS[19].stacktop[1] = '\0';
	RS[19].input = '-';
	RS[19].newstate = 'q';
	RS[19].newstacktop[0] = 'M', RS[19].newstacktop[1] = '\0';
	strcpy(RS[19].label[0],"-");
	strcpy(RS[19].label[1],"<minus>");
	
	RS[20].state = 'q';
	RS[20].stacktop[0] = 'M'; RS[20].stacktop[1] = '\0';
	RS[20].input = '-';
	RS[20].newstate = 'q';
	RS[20].newstacktop[0] = '\0';
	strcpy(RS[20].label[0],"-");

	RS[21].state = 'q';
	RS[21].stacktop[0] = 'O'; RS[21].stacktop[1] = '\0';
	RS[21].input = ')';
	RS[21].newstate = 'q';
	RS[21].newstacktop[0],'\0';
	strcpy(RS[21].label[0],")");	
}

void tokenize(char *string, char *tokens)
{
	int l = strlen(string);
	int j = 0;
	int i;
	
	for(i=0;i<l;i++)
	{
		if(string[i]==' ')
			continue;
		else if(string[i]=='f' && i<l-1 && string[i+1]=='o' && i<l-2 && string[i+2]=='r')
			tokens[j++] = 'F', i = i+2;
		else if(string[i]=='i' && i<l-1 && string[i+1]=='n' && i<l-2 && string[i+2]=='t')
			tokens[j++] = 'I', i = i+2;
		else if((string[i]>='0' && string[i]<='9'))
			tokens[j++] = 'D';
		else if(string[i]=='i' || string[i]==';' || string[i]=='=' || string[i]=='<' || string[i]=='>'
				|| string[i] == '(' || string[i] == ')' || string[i]=='+' || string[i]=='-')
			tokens[j++] = string[i];
		else
		{
			strcpy(tokens,"Lexer Error.");
			return;
		}
	}
	tokens[j] = '\0';
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
	{
		char lexed[100];
		tokenize(argv[1],lexed);
		strcpy(input, lexed);
	}
	
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
