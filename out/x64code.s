
./out/ex1.text.dump:     file format binary


Disassembly of section .data:

0000000000000000 <.data>:
   0:	55                   	push   rbp
   1:	89 e5                	mov    ebp,esp
   3:	81 ec 00 00 00 00    	sub    esp,0x0
   9:	b8 00 00 00 00       	mov    eax,0x0
   e:	50                   	push   rax
   f:	e8 fc ff ff ff       	call   0x10
  14:	83 c4 04             	add    esp,0x4
  17:	b8 00 00 00 00       	mov    eax,0x0
  1c:	c9                   	leave
  1d:	c3                   	ret
