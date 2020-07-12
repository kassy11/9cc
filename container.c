#include "9cc.h"

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
