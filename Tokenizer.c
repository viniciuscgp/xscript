#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "types.h"
#include "tokenizer.h"

extern char message[255];

TToken tokenTable[] = {TK_COMMAND, 	TK_PRINT, 		"print",	0.0,0,0,
					  TK_NUMFUNC, 	TK_SIN, 		"sin", 		0.0,0,0,
					  TK_NUMFUNC, 	TK_COS, 		"cos", 		0.0,0,0,
					  TK_NUMFUNC, 	TK_TAN, 		"tan", 		0.0,0,0,
					  TK_NUMFUNC, 	TK_CDBL, 		"cdbl",		0.0,0,0,
					  TK_NUMFUNC, 	TK_POW, 		"pow",		0.0,0,0,
					  TK_NUMFUNC, 	TK_ATAN, 		"atan",		0.0,0,0,
					  
					  TK_NUMFUNC, 	TK_LEN, 		"len", 		0.0,0,0,
					  TK_STRFUNC, 	TK_LEFT, 		"left", 	0.0,0,0,
					  TK_STRFUNC, 	TK_MID, 		"mid", 		0.0,0,0,
					  TK_STRFUNC, 	TK_RIGHT,		"right",	0.0,0,0,
					  0, 0, NULL, 0.0,0,0,};
					  
 
char message[255];
char look;
int  lookPos;
int  nextToken;
int  line, col;
int  error;
char *source;

TToken **tkList;

int tokenize(TToken **tkl, char *src)
{
	look       = '\0';
	lookPos    = 0;
	nextToken  = 0;
	
	line       = 1;
	col        = 1;
	
	source     = src;
	tkList     = tkl;
	error      = false;
	
	memset((char *) message, 0, 255);
	
	int i;
	for (i = 0; i <= MAX_TOKENS; i++)
	{
		tkList[i] = NULL;
	}
	
	nextChar();
	skipSpaces();
	while (! look == '\0')
	{
		if (isalpha(look))
		{
			if (!tokenizeCommands())
			{
				error = true;
				break;
			}
		} else
		if (isdigit(look))
		{
			tokenizeNumbers();
		} else
		if (look == '"')
		{
			tokenizeStrings();
		} else
		if (isOperator(look))
		{
			tokenizeOperators();
		} else
			showError("Unrecognized symbol %c in line %d col %d", look, line, col);
		skipSpaces();
		//nextChar();
	}
	if (error) return false; else return true;
}

void addToken(TToken *tk)
{
	tkList[nextToken++] = tk;
}

int tokenizeCommands()
{
	char *buffer;
	int  i;
	int found;
	TToken *find;
	// Get the ident on the source
	buffer = malloc(50);
	getIdent(buffer);

	// Make a new token
	i = 0;
	found = false;
	while (1)
	{
		find = &tokenTable[i++];
		if (find->tokenSource == NULL) break;
		
		if (strcmp(buffer, find->tokenSource) == 0)
		{
			TToken *tk;
			tk = malloc(sizeof(TToken));
			tk->tokenType	= find->tokenType;
			tk->tokenSource = find->tokenSource;
			tk->tokenCode 	= find->tokenCode;
			tk->tokenValue  = 0;
			tk->sourceLine  = line;
			tk->sourceCol   = col;
			addToken(tk);
			found = true;
			break;
		}
	}
	if (!found)
	{
		sprintf(message, "%s not found line %d col %d", buffer, line, col);
	}
	return found;
}

int tokenizeNumbers()
{
	char *buffer;
	buffer = malloc(50);
	getNumber(buffer);
	TToken *tk;
	tk = malloc(sizeof(TToken));
	tk->tokenType	= TK_NUMBER;
	tk->tokenCode   = 0;
	tk->tokenSource	= buffer;
	tk->tokenValue	= atof(buffer);
	tk->sourceLine  = line;
	tk->sourceCol   = col;
	addToken(tk);
	return 0;
}


int tokenizeStrings()
{
	char *buffer;
	buffer = malloc(255);
	getString(buffer);
	TToken *tk;
	tk 				= malloc(sizeof(TToken));
	tk->tokenType	= TK_STRING;
	tk->tokenCode   = 0;
	tk->tokenSource = buffer;
	tk->sourceLine  = line;
	tk->sourceCol   = col;
	addToken(tk);
	return 0;
}

int tokenizeOperators()
{
	char *buffer;
	buffer = malloc(2);
	buffer[0] = look;
	buffer[1] = '\0';
	match(look);
	TToken *tk;
	tk 				= malloc(sizeof(TToken));
	tk->tokenType	= TK_OPERATOR;
	tk->tokenCode 	=	0;
	tk->tokenSource	= buffer;
	tk->sourceLine  = line;
	tk->sourceCol   = col;
	addToken(tk);
	return 0;
}


void nextChar()
{
	look = source[lookPos];
	lookPos++;	
	col++;
}

void match(char c)
{
	if (look != c)
	{
		showError("Expected '%c' found '%c' in line %d col %d", c, look, line, col);
	}
	nextChar();
}

void skipSpaces()
{
	while ((!look == '\0') && isspace(look))
	{
		nextChar();
		if (look == 10) {
			line++;
			col = 0;
		}
	}	
}

void getIdent(char *buffer)
{
	int i = 0;
	while ((!look == '\0') && isalpha(look))		
	{
		buffer[i++] = look;
		nextChar();
	}
	buffer[i] = '\0';
}

void getNumber(char *buffer)
{
	int i = 0;
	while ((!look == '\0') && (isdigit(look) || look == '.'))
	{
		buffer[i++] = look;
		nextChar();
	}
	buffer[i] = '\0';
}

void getString(char *buffer)
{
	match('"');
	int i = 0;
	while ((!look == '\0') && look != '"')		
	{
		buffer[i++] = look;
		nextChar();
	}
	match('"');
	buffer[i] = '\0';
}

void freeTokens(TToken *tokenList[])
{
	// Libera memoria da lista de apontadores de Token
	//------------------------------------------------
	int i;
	for (i = 0; i < MAX_TOKENS; i++)
	{
		if (tokenList[i] != NULL)
		{
			if (tokenList[i]->tokenSource != NULL)
			{
				free((char *) tokenList[i]->tokenSource);
			}
			free((TToken *) tokenList[i]);	
		}	
	}
}

void showTokens(TToken *tokenList[])
{
	TToken *tk;
	printf("Token list:\n");
	printf("------------------------\n");
	int i;
	for (i = 0; i <= MAX_TOKENS; i++)
	{
		tk = tokenList[i];
		if (tk == NULL) break;
		
		printf("Tp: %5.5d Cde: %3.3d Str: %-30.30s Value:%10.2f\n", tk->tokenType, tk->tokenCode,
				tk->tokenSource, tk->tokenValue);
	}
}

int isOperator(char c)
{
	if (c == '+' || c == '-' || c == '/' || c == '*' || c == '(' || c == ')' || c == ',')
		return true;
	else
		return false;
}

int isAddOperator(char c)
{
	if (c == '+' || c == '-')
		return true;
	else
		return false;
}



