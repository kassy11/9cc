#include "9cc.h"

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

// 次のトークンがTOKEN_IDENTのときはそれを返して、次に進める
Token *consume_ident(){
    if(token->kind == TK_IDENT){
        Token *tok = token;
        token = token->next;
        return tok;
    }
    return NULL;
}

/* パーサの生成規則
program    = stmt*
stmt       = expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
*/


// program = stmt*
void program(){
    int i = 0;
    while(!at_eof()){
        // ポインタの配列に、プログラム自体の「文」の先頭ポインタを格納する
        code[i++] = stmt();
    }
    // NULLを入れておくと末尾がわかりやすい
    code[i] = NULL;
}

// stmt = expr ";"
Node *stmt(){
    Node *node = expr();
    expect(";");
    return node;
}

// expr = assign
Node *expr(){
    return assign();
}

// assign = equality ("=" assign)?
Node *assign(){
    Node *node = equality();

    if(consume("=")){
        node = new_binary(ND_ASSIGN, node, assign());
    }
    return node;
}

// どれも最左の演算子は無限ループに入る前に定義する

Node *equality(){
    // 一番左辺はrelationalから返ってきたノード
    Node *node = relational();

    // 最初の数値↑以外はループして解析する
    for(;;){
        if(consume("==")){
            node = new_binary(ND_EQ, node, relational());
        }else if(consume("!=")){
            node = new_binary(ND_NE, node, relational());
        }else{
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();
    for(;;){
        if(consume("<")){
            node = new_binary(ND_LT, node, add());
        }else if(consume("<=")){
            node = new_binary(ND_LE, node, add());
        }else if(consume(">")){
            node = new_binary(ND_LT,add(), node);
        }else if(consume(">=")){
            node = new_binary(ND_LE, add(), node);
        } else{
            return node;
        }
    }
}

Node *add(){
    Node *node = mul();

    for(;;){
        if(consume("+")){
            node = new_binary(ND_ADD, node, mul());
        }else if(consume("-")){
            node = new_binary(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node *mul(){
    Node *node = unary();
    for(;;){
        if(consume("*")){
            node = new_binary(ND_MUL, node, unary());
        }else if(consume("/")){
            node = new_binary(ND_DIV, node, unary());
        }else{
            return node;
        }
    }

}

// TODO:0-xっていうのがよく分かっていない
// パースの段階で+xをxに、-xを0-xに置き換えてしまう
Node *unary(){
    if(consume("+")){
        return unary();
    }else if(consume("-")){
        return new_binary(ND_SUB, new_num(0), unary());
    }
    return primary();
}

Node *primary(){
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }

    return new_num(expect_number());
}
