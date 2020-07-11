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
    // 指定位置のポインター先頭要素へのポインタ
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // エラー箇所個までの空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap); // 引数で受け取ったエラー文
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

// エラーを検出する方、期待された値なら次へ進める
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
    // グローバル変数のtokenの連結リストを進める
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
    Token head; // ダミーヘッダー
    head.next = NULL;
    Token *cur = &head;
    while(*p){
        // 空白文字はスキップ
        if(isspace(*p)){
            p++;
            continue;
        }

        if(*p == '+' || *p == '-'){
            // curがどんどん更新されていく
            cur = new_token(TK_RESERVED, cur, p++);
            // ここはpを関数に渡してからインクリメント
            // new_token(TK_RESERVED, cur, p++); p++; と同じ意味
        }else if(isdigit(*p)){
            // 10進数の数値かどうか
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            // strtol()はポインタから数値を読み込んで、ポインタを一つ進める
            // よってp++;の処理は不要
            continue;
        }else{
            error_at(p, "トークンが正しくありません\n");
        }
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}

// 抽象構文木のノードの種類
typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

// ２項演算子のノードの生成
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind; // 演算子のどれか
    node->rhs = rhs;
    node->lhs = lhs;
    return node;
}

// 数値のノードの生成
Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *primary();

// 構文木をnodeから生成する（パーサ）
// TODO:ここのパースされる流れを理解できてない

// expr = mul ("+" mul | "-" mul)*という生成規則と対応
Node *expr(){
    // 一回目だけ明示的にmul()まで掘られる
    Node *node = mul();

    for(;;){
        if(consume('+')){
            // 現在の着目トークンが+なら
            node = new_node(ND_ADD, node, mul());
        }else if(consume('-')){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

// mul = primary ( "*" primary | "/" primary)*という生成規則と対応
Node *mul(){
    // １回目だけ明示的にprimary()まで掘られる
    Node *node = primary();
    for(;;){
        if(consume('*')){
            node = new_node(ND_MUL, node, primary());
        }else if(consume('/')){
            node = new_node(ND_DIV, node, primary());
        }else{
            return node;
        }
    }

}

// primary = "(" expr ")" | num
Node *primary(){
    if(consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }else{
        return new_node_num(expect_number());
    }
}

// 生成された構文木からスタックマシンを構築して、アセンブリを出力する関数
void gen(Node *node){
    if(node->kind == ND_NUM){
        printf("  push %d\n", node->val);
        return;
    }

    // in-order
    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop tax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
    }

    printf("  push rax\n");
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
