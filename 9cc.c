#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    // コマンドの第1引数に直接コードを与えることにするので、
    // ダブルポインタとしてコマンドライン引数を定義する
    if(argc != 2){
        fprintf(stderr, "引数が正しくありません。\n");
        return  1;
    }

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".globl _main\n");
    printf("_main:\n");
    // strtolは、数値を読み込んだ後、第2引数のポインタをアップデートして、読み込んだ最後の文字の次の文字を指すように値を更新
    printf("        mov rax, %ld\n", strtol(p, &p, 10));

    while(*p){
        if(*p == '+'){
            p++; // 数値まで進める
            printf("        add rax, %ld\n", strtol(p, &p, 10));
            continue;
        }else if (*p == '-'){
            p++;
            printf("        sub rax, %ld",  strtol(p, &p, 10));
        }else{
            fprintf(stderr, "予期しない文字です: %c\n", *p);
            break;
        }
    }
    printf("        ret");
    return 1;
}