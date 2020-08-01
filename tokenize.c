#include "9cc.h"

// 次のトークンが期待する「記号」かどうか確かめる
// 真偽値を返す方
// 比較演算子も考慮して文字列を受け取る
bool consume(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len)){
        // トークン文字を引数と比較する
        return false;
    }

    token = token->next;
    return true;
}



// エラーを検出する方、期待された値なら次へ進める
void expect(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len)){
        error_at(token->str, "expected \"%s\"", op);
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
    // グローバル変数のtokenの連結リストを進める
    token = token->next;
    return val;
}


bool at_eof(){
    return token->kind == TK_EOF;
}

// 与えられた文字がトークンを構成する文字かどうかを判定
bool is_plpha (char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// 数値も含めたis_plpha
bool is_alnum (char c) {
    return is_plpha(c) || ('0' <= c && c <= '9');
}


// 新しいトークンを作成して元のトークンにつなげる
// 最後の連結リストにあるトークンを返す
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// トークンの比較演算子を検出する
bool startswith(char *p, char *q){
    return memcmp(p, q, strlen(q)) == 0;
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

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (isdigit(*p)) {
            char *q = p;
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }
        if (strchr("+-*/()<>;=", *p)) {
            cur = new_token(TK_RESERVED, cur, p, 1);
            cur->val = *p;
            p++;
            continue;
        }

        if (startswith(p, "return") && !is_plpha(p[6])) {
            cur = new_token(TK_RESERVED, cur, p, 6);
            p += 6;
            continue;
        }

        if (is_alnum(*p)) {
            char *q = p++;
            while(is_alnum(*p))
                p++;
            cur = new_token(TK_IDENT, cur, q, p - q);
            continue;
        }

        error_at(p, "予期しない文字列です");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}


