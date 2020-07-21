// ステップ３：トークナイザー
#include "9cc.h"

char *user_input;
Token *token;
Node *code[100];

int main(int argc, char **argv){
    if(argc != 2){
        error("引数の個数が正しくありません\n");
        return 1;
    }
    user_input = argv[1];

    // コマンドライン引数で与えられた計算式をトークナイズ
    // グローバル変数名なので、consume()やexpect_number()に引数として渡さなくていい
    token = tokenize();

    // 上記でトークナイズしたトークン列から構文木を生成する
    program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // プロローグ
    // 変数26個分（小文字アルファベット分）の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for(int i=0; code[i]; i++){
        gen(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("  pop rax\n");
    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}
