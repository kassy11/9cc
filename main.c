// ステップ３：トークナイザー
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

char *user_input;
Token *token;

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

    // 上記でトークナイズしたトークン列から構文木を
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 作成した構文木から木を降りながら再帰的にアセンブリを生成する
    gen(node);

    // 最後のスタックトップに残っている全体の計算結果をraxにロードして終了
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
