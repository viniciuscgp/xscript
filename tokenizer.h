// Prototypes
//------------------------------------------------
int  tokenize(TToken **tkl, char *src);
void showTokens(TToken *tokenList[]);

int tokenizeCommands();
int tokenizeNumbers();
int tokenizeStrings();
int tokenizeOperators();

void addToken(TToken *tk);

void nextChar();
void match(char c);
void skipSpaces();
void getIdent(char *buffer);
void getNumber(char *buffer);
void getString(char *buffer);

int isOperator(char c);




