.intel_syntax noprefix
.global main
main:
  push 12
  push 34
  push 5
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  ret
