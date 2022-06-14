#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum{
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;
struct Token{
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUM型の場合の数値
    char *str;      // トークン文字列 
};

// 着目しているトークン
Token *token;


// error報告のための関数
// printfと同じ引数を持つ
void error(char *fmt, ...){
    va_list ap;         //error関数の可変長引数．配列として扱える．
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待される記号ならばトークンを一つ進めて真を，そうでなければ偽を返す
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// 次のトークンが期待している記号のときには，トークンを1つ読み進める．それ以外の場合にはエラーを報告する．
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("'%c'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合，トークンを1つ読み進めてその数値を返す．それ以外の場合にはエラーを報告する．
int expect_number() {
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv){
    if(argc != 2){
        error("引数の個数が正しくありません");
        return 1;
    }

    // トークナイズする

    token = tokenize(argv[1]);

    // *pは第2引数の一文字目を指すポインタ

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    //式の最初は数でなければならないのでそれをチェック,mov命令を出力
    printf("  mov rax, %d\n", expect_number());

    while (!at_eof()){
        if (consume('+')){
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }
    
    printf("  ret\n");
    return 0;
}