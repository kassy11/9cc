#include "9cc.h"

Node *expr(){
    return equality();
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
    }else{
        return new_num(expect_number());
    }
}
