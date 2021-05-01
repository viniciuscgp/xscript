#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <strings.h>

#include "types.h"
#include "xscript.h"

#define MAX_STRING_SIZE 1024
extern char message[255];


TToken *tokenList[MAX_TOKENS];

int lookPos;
TToken *look;

int main(int argc, char *argv[])
{
	char *source = NULL;
	if (argc < 2) {
		printf("You must especify a script file!");	
		return 0;
	}
	
	if ((source = loadSource(argv[1]))== NULL)
	{
		printf("Error reading %s", argv[1]);
		return -1;
	}
	
	printf("\nSource loaded..");
	if (tokenize(tokenList, source))
	{
		showTokens(tokenList);
		printf("\nXScript->Running %s", argv[1]);
		run();
		printf("\nXSCript->Finished");
	} else
	{
		printf("Error: %-30.30s", message);
	}
	printf("\nXScript->freeing token memory...");
	freeTokens(tokenList);

	// Libera memoria gasta pelo codigo fonte
	//------------------------------------------------
	printf("\nXScript->freeing source memory...");
	free(source);
	
	return 0;
}

int run()
{
	lookPos = 0;
	next();
	while (look != NULL)
	{
		switch(look->tokenType)
		{
			case TK_COMMAND:
				runCommands();
				break;
			default:
				showError("Expected command in line %d col %d", look->sourceLine, look->sourceCol);				
				break;
		}
	}
	return true;
}

int runCommands()
{
	switch(look->tokenCode)
	{
		case TK_PRINT:
			runPrint();
			break;
		default:
			showError("Unexpected command:%s in line %d col %d", look->tokenSource, look->sourceLine, look->sourceCol);
	}	
}

int runPrint()
{
	next();
	if (look->tokenType == TK_STRING || look->tokenType == TK_STRFUNC)
		puts(strExpression());		
	else 
	if (look->tokenType == TK_NUMBER || look->tokenType == TK_NUMFUNC || look->tokenType == TK_OPERATOR)
		printf("%10.2f\n", numExpression());
	else
		showError("Print: String or numeric expression expected in line %d col %d", look->sourceLine, look->sourceCol);

}

double numExpression()
{
	double result = getFactor();
	//printf("%f", result);
	while (look != NULL)
	{
		switch(look->tokenSource[0]){
			case '+':
				next();
				result = result + getFactor();
				continue;
			case '-':
				next();
				result = result - getFactor();
				continue;
			default:
				break;
		}	
		break;
	}
	
		
	return result;
}

double getFactor()
{
	double result = getNum();
	while (look != NULL){
		switch(look->tokenSource[0]) {
			case '*':
				next();
				result = result * getNum();
				continue;
			case '/':
				next();
				result = result / getNum();
				continue;
			default:
				break;
		}		
		break;
	}
	return result;
}

double getNum()
{
	double result = 0;
	if (look->tokenSource[0] == '(')
	{
		matchChar('(');
		result = result + numExpression();
		matchChar(')');
		return result;
	}
	switch(look->tokenType) {
		case TK_NUMBER:
			result = look->tokenValue;
			next();
			break;
		case TK_NUMFUNC:
			result = result + getNumFunc();
			break;
		default:
			showError("Number or numeric function expected in line %d col %d", look->sourceLine, look->sourceCol);
			break;
	}
	return result;
}

double getNumFunc()
{
	double result = 0;
	double n1, n2;
	switch(look->tokenCode) {
		case TK_SIN:
			next();
			matchChar('(');
			result = result + sin(M_PI * getNum() / 180);
			matchChar(')');
			break;
		case TK_ASIN:
			next();
			matchChar('(');
			result = result + asin(getNum());
			matchChar(')');
			break;
			
		case TK_COS:
			next();
			matchChar('(');
			result = result + cos(M_PI * getNum() / 180);
			matchChar(')');
			break;			
		case TK_ACOS:
			next();
			matchChar('(');
			result = result + 180 * acos(getNum()) / M_PI;
			matchChar(')');
			break;			
		case TK_TAN:
			next();
			matchChar('(');
			result = result + tan(M_PI * getNum() / 180);
			matchChar(')');
			break;			
		case TK_ATAN:
			next();
			matchChar('(');
			result = result + 180 * atan(getNum()) / M_PI;
			matchChar(')');
			break;			
		case TK_POW:
			next();
			matchChar('(');
			n1 = getNum();
			matchChar(',');
			n2 = getNum();
			matchChar(')');
			result = result + pow(n1, n2);
			break;			
		default:
			showError("Numeric function %s unimplemented in line %d col %d", look->tokenSource, 
					  look->sourceLine, look->sourceCol);
			break;					  
		
	}
	return result;
}
char *strExpression()
{
	char *buffer;
	buffer = malloc(512);
	memset(buffer, 0, 512);
	strcat(buffer, getStr());
	while (look != NULL)
	{
		switch(look->tokenType) {
			case TK_OPERATOR:
				if (look->tokenSource[0] == '+')
				{
					next();
					strcat(buffer, getStr());
				}					
				break;
			default:
				break;	
		}				
		if (look->tokenSource[0] != '+') break;
	} 
	return buffer;
}

char *getStr()
{
	char *buffer = NULL;
	switch(look->tokenType) {
		case TK_STRING:
			buffer = malloc(strlen(look->tokenSource) + 1);
			strcpy(buffer, look->tokenSource);
			next();
			break;
		default:
			showError("String or String function expected in line %d col %d", look->sourceLine, look->sourceCol);
			break;
	}
	return buffer;
	
}

int next()
{
	look = tokenList[lookPos++];
}

int matchChar(char c)
{
	if (look == NULL)
	{
		showError("Unexpected end of script");
	}
	if (look->tokenSource[0] != c)
	{
		showError("Expected %c found %c in line %d col %d", c, look->tokenSource[0], look->sourceLine, look->sourceCol);		
	}
	next();	
	
}

char *loadSource(char *file)
{
	FILE *f = fopen(file, "rb");
	char *source;
	if (f == NULL)
	{
		return NULL;
	}
	// Verifica o tamanho
	fseek(f, 0L, SEEK_END);
	int size = ftell(f);
	// Volta ao inicio do arquivo
	fseek(f, 0L, SEEK_SET);	
	
	source = (char *) malloc(size + 1);

	fread(source, size, 1, f);
	fclose(f);
	source[size] = '\0';
	return source;
}

void showError(const char *error, ...)
{
    va_list args;
    va_start(args, error);
    vprintf(error, args);
	va_end(args);
	exit(-1);
}
