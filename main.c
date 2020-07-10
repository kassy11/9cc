// ステップ３：トークナイザー

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ユーザのコマンドライン引数での入力をグローバルで保存する
char *user_input;

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
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
};

// 現在着目しているトークン
Token *token;

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// locは文字列の途中へのポインタ
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    // 入力の先頭との差を取ることでエラー箇所を検出する
    // TODO:ここも理解できてない
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // エラー箇所個までの空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待する「記号」かどうか確かめる
// 真偽値を返す方
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }

    token = token->next;
    return true;
}

// エラーを検出する方
void expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        error_at(token->str, "expected '%c'", op);
    }
    token = token->next;
}

// トークンが数値ならその数値を返す
// その後進める
int expect_number(){
    if(token->kind != TK_NUM){
        error_at(token->str, "数ではありません\n");
    }
    int val = token->val;
    token = token->next;
    return val;
}

//
bool at_eof(){
    return token->kind == TK_EOF;
}

// 新しいトークンを作成して元のトークンにつなげる
// 最後の連結リストにあるトークンを返す
Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// 文字列pをトークナイズする
// トークンに分解＆連結することで、トークン以外からなる空白文字などを削除する
// 連結リストで構成する
// TODO:連結リストの復習
Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while(*p){
        // 空白文字はスキップ
        if(isspace(*p)){
            p++;
            continue;
        }

        if(*p == '+' || *p == '-'){
            // TODO:ここのp++が理解できない
            cur = new_token(TK_RESERVED, cur, p++);
        }else if(isdigit(*p)){
            // 10進数の数値かどうか
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }else{
            error_at(p, "トークンが正しくありません\n");
        }
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv){
    if(argc != 2){
        error("引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];

    // コマンドライン引数で与えられた計算式をトークナイズして
    // グローバル変数tokenに代入
    token = tokenize();
    // グローバル変数名なので、consume()やexpect_number()に引数として渡さなくていい

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number());
    while(!at_eof()){
        if(consume('+')){
            printf("  add rax, %d\n", expect_number());
        }else if(consume('-')){
            printf("  sub rax, %d\n", expect_number());
        }
    }
    printf("  ret\n");
    return 0;
}
