// Token para armazenar o script em forma de tokens
//-------------------------------------------------
typedef struct token 
{
	int		tokenType;  	// what token is that
	int		tokenCode; 		// the code of a function if applicable
	char	*tokenSource;	// Source token
	double 	tokenValue;		// if token is a number
	int 	sourceLine; 	// source code line
	int	 	sourceCol;      // source code column
} TToken;

#define TK_COMMAND  1
#define TK_NUMBER   2
#define TK_STRING   3
#define TK_OPERATOR 4

#define TK_NUMFUNC 100
#define TK_STRFUNC 200

#define TK_PRINT 2

#define TK_SIN  100
#define TK_COS	 101
#define TK_TAN  102
#define TK_CDBL 103
#define TK_ABS  104
#define TK_POW  105
#define TK_ASIN 106
#define TK_ACOS 107
#define TK_ATAN 108
#define TK_LOG  109

#define TK_LEN  200
#define TK_LEFT 201
#define TK_MID  202
#define TK_RIGHT 203


#define true 1
#define false 0

#define MAX_TOKENS 5000



