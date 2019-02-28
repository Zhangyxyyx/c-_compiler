#include<iostream>
#include"global.h"
using namespace std;
void getToken() {
	if (tokenstore[curindex].num >= 0)
		token = tokenstore[curindex++];
	else token.num = -1;
}
void init() {
	for (int i = 0; i < 100000; i++) {
		tokenstore[i].num = -1;
	}
	int count = 0;
	while (!source.eof()) {
		source >> tokenstore[count].num;
		source >> tokenstore[count++].Identity;
	}
	getToken();
}
static void error() {
}
TreeNode * newStmtNode(StmtKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;

	for (i = 0; i<MAXCHILDREN; i++) t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = StmtK;
	t->kind.stmt = kind;
	//t->lineno = lineno;

	return t;
}
TreeNode * newExpNode(ExpKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;

	for (i = 0; i<MAXCHILDREN; i++) t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = ExpK;
	t->kind.exp = kind;
	//t->lineno = lineno;
	//t->type = Void;

	return t;
}
static void match(TokenType expected)
{
	if (token.num == expected)  getToken();
	else error();
}
TreeNode *declaration_list() {
	TreeNode* t = NULL;
	TreeNode* result = NULL;
	if (token.num >= 3 && token.num <= 8)
		t = declaration();
	result = t;
	while (token.num >= 3 && token.num <= 8) {
		TreeNode* p = declaration();
		t->sibling = p;
		t = p;
	}
	return result;
}
TreeNode *declaration() {
	TreeNode* t = NULL;
	int temp = curindex;
	temp += 1;
	if (tokenstore[temp].num == LPAREN) {
		t = fun_declaration();
	}
	else {
		t = var_declaration();
	}
	return t;
}
TreeNode* var_declaration() {
	TreeNode* t = newStmtNode(DecvarK);
	t->child[0] = type_specifier();
	t->child[1] = getID();
	match(SEMI);
	return t;
}
TreeNode* type_specifier() {
	TreeNode* t = newExpNode(VarkindK);
	t->attr.type = (VarKind)(token.num - 3);
	getToken();
	return t;
}
TreeNode* getID() {
	TreeNode* t = newExpNode(IdK);
	strcpy(t->attr.name, token.Identity);
	match(ID);
	return t;
}
TreeNode* fun_declaration() {
	TreeNode* t = newStmtNode(DecfunctionK);
	t->child[0] = type_specifier();
	t->child[1] = getID();
	match(LPAREN);
	t->child[2] = params();
	match(RPAREN);
	if (token.num == SEMI)
		match(SEMI);
	else if (token.num == LBRACE)
		t->child[3] = compound_stmt();
	else error();
	return t;
}
TreeNode* params() {
	TreeNode* t = newStmtNode(ParamK);
	if (token.num == VOID) {
		t->attr.type = VOIDk;
		match(VOID);
	}
	else if (token.num >= 3 && token.num <= 8) {
		t = param_list();
	}
	else error();
	return t;
}
TreeNode* param_list() {
	TreeNode* t = NULL;
	t = param();
	TreeNode* result = t;
	while (token.num == COMMA) {
		match(COMMA);
		TreeNode* p = param();
		t->sibling = p;
		t = p;
	}
	return result;
}
TreeNode* param() {
	TreeNode* t = newStmtNode(ParamK);
	t->child[0] = type_specifier();
	t->child[1] = getID();
	match(ID);
	return t;
}
TreeNode* compound_stmt() {
	TreeNode* t = NULL;
	match(LBRACE);
	t = newStmtNode(ComstmtK);
	t->child[0] = local_declarations();
	t->child[1] = statement_list();
	match(RBRACE);
	return t;
}
TreeNode* local_declarations() {
	TreeNode* t = NULL;
	if (token.num >= 3 && token.num <= 8) {
		t = var_declaration();
	}
	TreeNode* result = t;
	while (token.num >= 3 && token.num <= 8) {
		TreeNode* p = var_declaration();
		t->sibling = p;
		t = p;
	}
	return result;
}
TreeNode* statement_list() {
	TreeNode* t = NULL;
	TreeNode* result = NULL;
	if (token.num == RBRACE)
		return t;
	else {
		t = statement();
		result = t;
		while (token.num != RBRACE) {
			TreeNode* p = statement();
			t->sibling = p;
			t = p;
		}
		match(RBRACE);
	}
	return result;
}
TreeNode* statement() {
	TreeNode* t = NULL;
	switch (token.num) {
	case IF:
		t = If_stmt();
		break;
	case SWITCH:
		t = Switch_stmt();
		break;
	case WHILE:
		t = While_stmt();
		break;
	case CONTINUE:
		t = Continue_stmt();
		break;
	case BREAK:
		t = Break_stmt();
		break;
	case RETURN:
		t = Return_stmt();
		break;
	default:
		t = Expression_stmt();
		break;
	}
	return t;
}
TreeNode* Expression_stmt() {
	TreeNode* t = NULL;
	if (token.num == SEMI) {
		match(SEMI);
	}
	else {
		t = expression();
		match(SEMI);
	}
	return t;
}
TreeNode* If_stmt() {
	TreeNode* t = newStmtNode(If);
	match(IF);
	match(LPAREN);
	t->child[0] = expression();
	match(RPAREN);
	t->child[1] = statement();
	if (token.num == ELSE) {
		match(ELSE);
		t->child[2] = statement();
	}
	return t;
}
TreeNode* Switch_stmt() {
	TreeNode* t = newStmtNode(Switch);
	match(SWITCH);
	match(LPAREN);
	t->child[0] = var();
	match(RPAREN);
	match(LBRACE);
	TreeNode* temp;
	t->child[1] = Case_list();
	match(RBRACE);
	return t;
}
TreeNode* Case_list() {
	TreeNode* t = NULL;
	TreeNode* result = t;
	if (token.num == CASE) {
		t = Case_stmt();
		result = t;
	}
	while (token.num == CASE) {
		TreeNode* p = Case_stmt();
		t->sibling = p;
		t = p;
	}
	return result;

}
TreeNode* Case_stmt() {
	TreeNode* t = newStmtNode(Case);
	match(CASE);
	t->child[0] = factor();
	match(COLON);
	t->child[1] = statement_list();
	return t;
}
TreeNode* While_stmt() {
	TreeNode* t = newStmtNode(While);
	match(WHILE);
	match(LPAREN);
	t->child[0] = expression();
	match(RPAREN);
	match(LBRACE);
	t->child[1] = statement_list();
	match(RBRACE);
	return t;
}
TreeNode* Continue_stmt() {
	TreeNode* t = newStmtNode(Continue);
	match(CONTINUE);
	match(SEMI);
	return t;
}
TreeNode* Break_stmt() {
	TreeNode* t = newStmtNode(Break);
	match(BREAK);
	match(SEMI);
	return t;
}
TreeNode* Return_stmt() {
	TreeNode* t = newStmtNode(Return);
	match(RETURN);
	if (token.num != SEMI) {
		t->child[1] = expression();
	}
	match(SEMI);
	return t;
}
TreeNode* expression() {
	TreeNode* t = NULL;
	int temp = curindex;
	if (tokenstore[temp].num == ASSIGN) {
		t = newExpNode(AssignK);
		t->attr.op = ASSIGN;
		t->child[0] = var();
		match(ASSIGN);
		t->child[1] = expression();
	}
	else t = simple_expression();
	return t;
}
TreeNode* var() {
	TreeNode* t = newExpNode(IdK);
	strcpy(t->attr.name, token.Identity);
	match(ID);
	return t;
}
TreeNode* simple_expression() {
	TreeNode* t = NULL;
	TreeNode* p = NULL;
	int temp = curindex;
	if (tokenstore[temp].num >= 18 && tokenstore[temp].num <= 28) {
		p = additive_expression();
		t = relop();
		t->child[0] = p;
		t->child[1] = additive_expression();
	}
	else {
		t = additive_expression();
	}
	return t;
}
TreeNode* relop() {
	TreeNode* t = newExpNode(OpK);
	switch ((TokenType)token.num) {
	case SMALLER:
		t->attr.op = SMALLER;
		match(SMALLER);
		break;
	case SMALLER_EQ:
		t->attr.op = SMALLER_EQ;
		match(SMALLER_EQ);
		break;
	case BIGGER:
		t->attr.op = BIGGER;
		match(BIGGER);
		break;
	case BIGGER_EQ:
		t->attr.op = BIGGER_EQ;
		match(BIGGER_EQ);
		break;
	case EQ:
		t->attr.op = EQ;
		match(EQ);
		break;
	case NOT_EQ:
		t->attr.op = NOT_EQ;
		match(NOT_EQ);
		break;
	case OR:
		t->attr.op = OR;
		match(OR);
		break;
	case AND:
		t->attr.op = AND;
		match(AND);
		break;
	default:
		error();
	}
	return t;
}
TreeNode* additive_expression() {
	TreeNode* t = NULL;
	TreeNode* p = NULL;
	int temp = curindex;
	if (tokenstore[temp].num == 18 || tokenstore[temp].num == 19) {
		p = additive_expression();
		t = addop();
		t->child[0] = p;
		t->child[1] = term();
	}
	else t = term();
	return t;
}
TreeNode* addop() {
	TreeNode* t = newExpNode(OpK);
	switch (token.num) {
	case PLUS:
		t->attr.op = PLUS;
		match(PLUS);
		break;
	case MINE:
		t->attr.op = MINE;
		match(MINE);
		break;
	default:
		error();
	}
	return t;
}
TreeNode* term() {
	TreeNode* t = NULL;
	TreeNode* p = NULL;
	int temp = curindex;
	if (tokenstore[temp].num == 20) {
		p = term();
		t = mulop();
		t->child[0] = p;
		t->child[1] = factor();
	}
	else t = factor();
	return t;
}
TreeNode* mulop() {
	TreeNode* t = newExpNode(OpK);
	t->attr.op = MUL;
	match(MUL);
	return t;
}
TreeNode* factor() {
	TreeNode* t = NULL;
	int temp = curindex;
	if (tokenstore[temp].num == LBRACE&&token.num == ID) {
		t = call();
	}
	else if (token.num == LPAREN) {
		match(LPAREN);
		t = expression();
		match(LPAREN);
	}
	else if (token.num == ID) {
		t = var();
	}
	else if (token.num == INTNUM || token.num == FLOATNUM) {
		switch (token.num) {
		case INTNUM:
			t = newExpNode(ConstK);
			strcpy(t->attr.value.val, token.Identity);
			t->attr.value.type = INTk;
			match(INTNUM);
			break;
		case FLOATNUM:
			t = newExpNode(ConstK);
			strcpy(t->attr.value.val, token.Identity);
			t->attr.value.type = FLOATk;
			match(FLOATNUM);
			break;
		default:
			error();
		}
	}
	return t;
}
TreeNode* call() {
	TreeNode* t = newExpNode(CallK);
	strcpy(t->attr.name, token.Identity);
	match(ID);
	match(LPAREN);
	if (token.num != RPAREN) {
		t->child[0] = args();
	}
	match(RPAREN);
	return t;
}
TreeNode* args() {
	TreeNode* t = expression();
	TreeNode* result = NULL;
	if (token.num == COMMA) {
		match(COMMA);
		TreeNode* p = expression();
		t->sibling = p;
		t = p;
	}
	return result;
}


static int indentno = 0;

/* Procedure printToken prints a token
* and its lexeme to the listing file
*/
void printToken(TokenType token, const char* tokenString)
{
	switch (token)
	{
	case IF:
	case CHAR:
	case FLOAT:
	case INT:
	case LONG:
	case SHORT:
	case VOID:
	case CONTINUE:
	case DEFAULT:
	case BREAK:
	case CASE:
	case ELSE:
	case RETURN:
	case SWITCH:
	case WHILE:
		fprintf(out,
			"reserved word: %s\n", tokenString);
		break;
	case ASSIGN: fprintf(out, "=\n"); break;
	case BIGGER: fprintf(out, ">\n"); break;
	case EQ: fprintf(out, "==\n"); break;
	case LPAREN: fprintf(out, "(\n"); break;
	case RPAREN: fprintf(out, ")\n"); break;
	case SEMI: fprintf(out, ";\n"); break;
	case PLUS: fprintf(out, "+\n"); break;
	case MINE: fprintf(out, "-\n"); break;
	case MUL: fprintf(out, "*\n"); break;
	case SMALLER:fprintf(out, "<\n"); break;
	case SMALLER_EQ:fprintf(out, "<=\n"); break;
	case BIGGER_EQ:fprintf(out, ">=\n"); break;
	case NOT_EQ:fprintf(out, "!=\n"); break;
	case OR:fprintf(out, "||\n"); break;
	case AND:fprintf(out, "&&\n"); break;
	case LSHIFT:fprintf(out, "<<\n"); break;
	case RSHIFT:fprintf(out, ">>\n"); break;
	case LBRACE:fprintf(out, "{\n"); break;
	case RBRACE: fprintf(out, "}\n"); break;
	case COMMA:fprintf(out, ",\n"); break;
	case COLON:fprintf(out, ":\n"); break;
	case INTNUM:
		fprintf(out,
			"INTNUM, val= %s\n", tokenString);
		break;
	case FLOATNUM:
		fprintf(out,
			"FLOATNUM, val= %s\n", tokenString);
		break;
	case ID:
		fprintf(out,
			"ID, name= %s\n", tokenString);
		break;
	default: /* should never happen */
		fprintf(out, "Unknown token: %d\n", token);
	}
}



/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
	int i;
	for (i = 0; i<indentno; i++)
		fprintf(out, " ");
}

/* procedure printTree prints a syntax tree to the
* listing file using indentation to indicate subtrees
*/
void printTree(TreeNode * tree)
{
	int i;
	INDENT;
	while (tree != NULL) {
		printSpaces();
		if (tree->nodekind == StmtK)
		{
			switch (tree->kind.stmt) {
			case ParamK:
				fprintf(out, "Param:\n");
				break;
			case ComstmtK:
				fprintf(out, "ComstmtK\n");
				break;
			case If:
				fprintf(out, "If\n");
				break;
			case Continue:
				fprintf(out, "Continue\n");
				break;
			case Break:
				fprintf(out, "Break\n");
				break;
			case Return:
				fprintf(out, "Return\n");
				break;
			case Case:
				fprintf(out, "Case\n");
				break;
			case Switch:
				fprintf(out, "Switch\n");
				break;
			case While:
				fprintf(out, "While\n");
				break;
			case DecfunctionK:
				fprintf(out, "DecfunctionK:\n");
				break;
			case DecvarK:
				fprintf(out, "DecvarK:\n");
				break;
			default:
				fprintf(out, "Unknown ExpNode kind\n");
				break;
			}
		}
		else if (tree->nodekind == ExpK)
		{
			switch (tree->kind.exp) {
			case OpK:
				fprintf(out, "Op: ");
				printToken(tree->attr.op, "\0");
				break;
			case CallK:
				fprintf(out, "Call: %s\n", tree->attr.name);
				break;
			case ConstK:
				fprintf(out, "Const: %s Type: %d\n", tree->attr.value.val, tree->attr.value.type);
				break;
			case IdK:
				fprintf(out, "Id: %s\n", tree->attr.name);
				break;
			case VarkindK:
				fprintf(out, "VarKind: %d\n", tree->attr.type);
				break;
			default:
				fprintf(out, "Unknown ExpNode kind\n");
				break;
			}
		}
		else fprintf(out, "Unknown node kind\n");
		for (i = 0; i<MAXCHILDREN; i++)
			printTree(tree->child[i]);
		tree = tree->sibling;
	}
	UNINDENT;
}

void main() {

	source.open("C:\\Users\\Kelly\\Desktop\\compiler\\c-\\text\\word.txt");
	out = fopen("C:\\Users\\Kelly\\Desktop\\compiler\\c-\\text\\result.txt", "w");
	init();
	source.close();

	TreeNode* root = declaration_list();
	int a = 0;
	printTree(root);
}
