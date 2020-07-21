.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  mov rax, rsp
  sub rax, 8
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rsp, rbp
  pop rbp
  ret
