#ifndef   GLOBAL       //如果没有定义这个宏  
#define   GLOBAL 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include<fstream>
#define MaxTokenLen 40
#define NumofReserve 51
#define NumofOperator 19
#define NumofType 3
using namespace std;
ifstream source;
FILE* out;

struct TOKEN {
	int num = -1;
	char Identity[MaxTokenLen];
};
TOKEN token;
int curindex = 0;
TOKEN tokenstore[100000];

typedef enum {
	//mul token
	ID, INTNUM, FLOATNUM,
	//vartype
	CHAR, FLOAT, INT, LONG, SHORT, VOID,
	//reserve word
	CONTINUE, DEFAULT, BREAK, CASE, ELSE, IF,
	RETURN, SWITCH, WHILE,

	//special symbol
	PLUS, MINE, MUL, SMALLER, SMALLER_EQ, BIGGER, BIGGER_EQ, EQ, NOT_EQ, OR,
	AND, LSHIFT, RSHIFT, ASSIGN, SEMI, LPAREN, RPAREN, LBRACE, RBRACE, COMMA, COLON
}TokenType;
typedef enum { ExpK, StmtK } NodeKind;
typedef enum { CHARk, FLOATk, INTk, LONGk, SHORTk, VOIDk } VarKind;
typedef enum { If, Switch, While, Continue, Break, Return, Case, DecvarK, DecfunctionK, ParamK, ComstmtK } StmtKind;
typedef enum { OpK, ConstK, IdK, ParK, CallK, ArgsK, VarkindK, AssignK } ExpKind;
typedef enum { Void, Integer, Float, Boolean } ExpType;

#define MAXCHILDREN 4

typedef struct Const {
	char val[100];
	VarKind type;
}number;
typedef struct treeNode
{
	struct treeNode * child[MAXCHILDREN];
	struct treeNode * sibling;
	//int lineno;
	NodeKind nodekind;
	union { StmtKind stmt; ExpKind exp; } kind;
	union {
		TokenType op;
		number value;
		char  name[40];
		VarKind type;
	} attr;
	//ExpType type; /* for type checking of exps */
} TreeNode;

void printToken(TokenType, const char*);
void printTree(TreeNode * root);

TreeNode* While_stmt();
TreeNode* var();
TreeNode* var_declaration();
TreeNode* type_specifier();
TreeNode* term();
TreeNode* Switch_stmt();
TreeNode* statement();
TreeNode* statement_list();
TreeNode* simple_expression();
TreeNode* Return_stmt();
TreeNode* relop();
TreeNode* param();
TreeNode* param_list();
TreeNode* params();
TreeNode* mulop();
TreeNode* If_stmt();
TreeNode* local_declarations();
TreeNode* getID();
TreeNode* fun_declaration();
TreeNode* factor();
TreeNode* expression();
TreeNode* Expression_stmt();
TreeNode* Continue_stmt();
TreeNode* declaration();
TreeNode* compound_stmt();
TreeNode* Case_stmt();
TreeNode* call();
TreeNode* args();
TreeNode* addop();
TreeNode* Case_list();
TreeNode* Break_stmt();
TreeNode* additive_expression();

#endif
