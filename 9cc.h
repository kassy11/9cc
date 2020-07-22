
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifndef INC_9CC_9CC_H
#define INC_9CC_9CC_H
// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子（変数名）
    TK_NUM,      // 整数
    TK_EOF,      // トークン列の終わりを表す特別な型：コンパイラのときは必要
} TokenKind;

// トークンの情報を格納する構造体
typedef struct Token Token;
struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

// 変数は連結リストで表すことにする
// LVarという構造体で一つの変数を表すことにして、先頭の要素をlocalsというポインタで持つ

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
};

// ローカル変数
extern LVar *locals;


// 抽象構文木のノードの種類
typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_NUM,
    ND_ASSIGN,
    ND_LVAR
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val; // kindがND_NUMのときつかう
    // ベースレジスタからのオフセットを表す
    int offset; // kindがND_LVARのときつかう
};

extern char *user_input;
extern Token *token;
// ポインタの配列
extern Node *code[100];

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize();
Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);
Token *consume_ident();
LVar *find_lvar(Token *tok);
bool is_plpha (char c);
bool is_alnum (char c);


        Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();
Node *assign();
Node *stmt();
void program();

void gen(Node *node);
void gen_lval(Node *node);

#endif //INC_9CC_9CC_H
