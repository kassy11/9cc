#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
    TK_RESERVED, //記号
    TK_NUM, // 数値
    TK_EOF, // 入力の終わり
} TokenKind;

typedef struct Token Token;

struct Token{
    TokenKind kind; // トークンの種類
    Token *next; // 次の入力トークン, 次の構造体を指す
    int val;  // kindが数値のときの値
    char *str;  // トークン文字列
} ;

// 現在着目しているトークンの構造体を指す
// パーサが読み込むトークン列
Token *token;

/ 入力プログラム
char *user_input;


// エラー関数
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// トークンが期待した値ならトークンを一つ進めてtrueを返す
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    token = token->next;
    return true;
}

// トークンが期待した値ならトークンを一つ進める
void expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        error("%cではありません\n", op);
    }
    token = token->next;
}

// 次のトークンが数値なら一つ進めて、その数値を返す
int expect_number(){
    if(token->kind != TK_NUM){
        error("数ではありません\n");
    }
    int val = token->val;
    token = token->next;
    return val;
}

// 次のトークンが最後の文字ならtrueを返す
bool at_eof(){
    return token->kind == TK_EOF;
}

// Token型構造体へのポインタを返す関数
// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str){
    // callocはmallocとほぼ同じ
    Token *tok = calloc(1, sizeof(Token);
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// 入力文字列のトークナイズ
// 戦闘要素がダミーの連結リスト
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }

        if(*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p++);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        error("トークナイズできません\n");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char** argv){
    // コマンドの第1引数に直接コードを与えることにするので、
    // ダブルポインタとしてコマンドライン引数を定義する
    if(argc != 2){
        fprintf(stderr, "引数が正しくありません。\n");
        return  1;
    }
    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".globl _main\n");
    printf("_main:\n");
    // strtolは、数値を読み込んだ後、第2引数のポインタをアップデートして、読み込んだ最後の文字の次の文字を指すように値を更新
    printf("        mov rax, %ld\n", expect_number());


    while(!at_eof()){
        if(consume('+')){
            printf("  add rax, %d", expect_number());
        }

        // consume()とexpect()のちがいは？
        expect('-');
        printf("  sub rax, %d", expect_number());
    }

    printf("        ret");
    return 0;
}

