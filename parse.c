#include "9cc.h"

// 構文木をnodeから生成する（パーサ）
// TODO:ここのパースされる流れを理解できてない

Node *expr(){
    return equality();
}

Node *equality(){
    Node *node = relational();

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
    // １回目だけ明示的にprimary()まで掘られる
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
