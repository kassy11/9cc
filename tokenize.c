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

    while(*p){
        // 空白文字はスキップ
        if(isspace(*p)){
            p++;
            continue;
        }

        if(startswith(p, "==")|| startswith(p, "!=")||
           startswith(p, "<=") || startswith(p, ">=")){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p+=2;
            continue;
        }

        // strchr()で、"+-*/()<>"から*pを検索してそれ以降のアドレスを返す
        if(strchr("+-*/()<>", *p)){
            // curがどんどん更新されていく
            cur = new_token(TK_RESERVED, cur, p++, 1);
            // ここはpを関数に渡してからインクリメント
            // new_token(TK_RESERVED, cur, p++); p++; と同じ意味
            continue;
        }else if(isdigit(*p)){
            // 10進数の数値かどうか
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            // strtol()はポインタから数値を読み込んで、ポインタを一つ進める（次の数値の文字列を読み込む）
            // よってp++;の処理は不要

            // 数字の桁数を
            // すべてのトークンのlenを管理しているので
            cur->len = p-q;
            continue;
        }else{
            error_at(p, "トークンが正しくありません\n");
        }
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}


// ノード生成の共通処理
Node *new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind; // 演算子のどれか
    return node;
}

// ２項演算子のノードの生成
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 数値のノードの生成
Node *new_num(int val){
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}
