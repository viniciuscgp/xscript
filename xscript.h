//-------------------------------------------------
// Prototipos das funcoes
//-------------------------------------------------
char *loadSource(char *file);
void showError(const char *error, ...);

int run();
int runCommands();
int runPrint();
int next();
int matchChar(char c);
char *strExpression();
char *getStr();

double numExpression();
double getFactor();
double getNum();
double getNumFunc();
