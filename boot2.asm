
boot2.o:     file format elf32-i386


Disassembly of section .text:

00000000 <boot2_main>:
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	53                   	push   ebx
   4:	83 ec 14             	sub    esp,0x14
   7:	83 ec 08             	sub    esp,0x8
   a:	6a 3c                	push   0x3c
   c:	6a 03                	push   0x3
   e:	e8 95 00 00 00       	call   a8 <read_sectors>
  13:	83 c4 10             	add    esp,0x10
  16:	c7 45 ec 00 00 00 01 	mov    DWORD PTR [ebp-0x14],0x1000000
  1d:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  24:	eb 48                	jmp    6e <boot2_main+0x6e>
  26:	e8 56 00 00 00       	call   81 <wait>
  2b:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  32:	eb 2d                	jmp    61 <boot2_main+0x61>
  34:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  37:	c1 e0 08             	shl    eax,0x8
  3a:	89 c2                	mov    edx,eax
  3c:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  3f:	01 d0                	add    eax,edx
  41:	8d 14 00             	lea    edx,[eax+eax*1]
  44:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  47:	8d 1c 02             	lea    ebx,[edx+eax*1]
  4a:	83 ec 0c             	sub    esp,0xc
  4d:	68 f0 01 00 00       	push   0x1f0
  52:	e8 03 01 00 00       	call   15a <inw>
  57:	83 c4 10             	add    esp,0x10
  5a:	66 89 03             	mov    WORD PTR [ebx],ax
  5d:	83 45 f0 01          	add    DWORD PTR [ebp-0x10],0x1
  61:	81 7d f0 ff 00 00 00 	cmp    DWORD PTR [ebp-0x10],0xff
  68:	7e ca                	jle    34 <boot2_main+0x34>
  6a:	83 45 f4 01          	add    DWORD PTR [ebp-0xc],0x1
  6e:	83 7d f4 3b          	cmp    DWORD PTR [ebp-0xc],0x3b
  72:	7e b2                	jle    26 <boot2_main+0x26>
  74:	b8 00 00 00 01       	mov    eax,0x1000000
  79:	ff d0                	call   eax
  7b:	90                   	nop
  7c:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
  7f:	c9                   	leave  
  80:	c3                   	ret    

00000081 <wait>:
  81:	55                   	push   ebp
  82:	89 e5                	mov    ebp,esp
  84:	83 ec 08             	sub    esp,0x8
  87:	90                   	nop
  88:	83 ec 0c             	sub    esp,0xc
  8b:	68 f7 01 00 00       	push   0x1f7
  90:	e8 b0 00 00 00       	call   145 <inb>
  95:	83 c4 10             	add    esp,0x10
  98:	0f b6 c0             	movzx  eax,al
  9b:	25 c0 00 00 00       	and    eax,0xc0
  a0:	83 f8 40             	cmp    eax,0x40
  a3:	75 e3                	jne    88 <wait+0x7>
  a5:	90                   	nop
  a6:	c9                   	leave  
  a7:	c3                   	ret    

000000a8 <read_sectors>:
  a8:	55                   	push   ebp
  a9:	89 e5                	mov    ebp,esp
  ab:	83 ec 08             	sub    esp,0x8
  ae:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  b1:	c1 f8 18             	sar    eax,0x18
  b4:	83 e0 0f             	and    eax,0xf
  b7:	83 c8 e0             	or     eax,0xffffffe0
  ba:	0f b6 c0             	movzx  eax,al
  bd:	83 ec 08             	sub    esp,0x8
  c0:	50                   	push   eax
  c1:	68 f6 01 00 00       	push   0x1f6
  c6:	e8 a6 00 00 00       	call   171 <outb>
  cb:	83 c4 10             	add    esp,0x10
  ce:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  d1:	0f b6 c0             	movzx  eax,al
  d4:	83 ec 08             	sub    esp,0x8
  d7:	50                   	push   eax
  d8:	68 f2 01 00 00       	push   0x1f2
  dd:	e8 8f 00 00 00       	call   171 <outb>
  e2:	83 c4 10             	add    esp,0x10
  e5:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  e8:	0f b6 c0             	movzx  eax,al
  eb:	83 ec 08             	sub    esp,0x8
  ee:	50                   	push   eax
  ef:	68 f3 01 00 00       	push   0x1f3
  f4:	e8 78 00 00 00       	call   171 <outb>
  f9:	83 c4 10             	add    esp,0x10
  fc:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  ff:	c1 f8 08             	sar    eax,0x8
 102:	0f b6 c0             	movzx  eax,al
 105:	83 ec 08             	sub    esp,0x8
 108:	50                   	push   eax
 109:	68 f4 01 00 00       	push   0x1f4
 10e:	e8 5e 00 00 00       	call   171 <outb>
 113:	83 c4 10             	add    esp,0x10
 116:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 119:	c1 f8 10             	sar    eax,0x10
 11c:	0f b6 c0             	movzx  eax,al
 11f:	83 ec 08             	sub    esp,0x8
 122:	50                   	push   eax
 123:	68 f5 01 00 00       	push   0x1f5
 128:	e8 44 00 00 00       	call   171 <outb>
 12d:	83 c4 10             	add    esp,0x10
 130:	83 ec 08             	sub    esp,0x8
 133:	6a 20                	push   0x20
 135:	68 f7 01 00 00       	push   0x1f7
 13a:	e8 32 00 00 00       	call   171 <outb>
 13f:	83 c4 10             	add    esp,0x10
 142:	90                   	nop
 143:	c9                   	leave  
 144:	c3                   	ret    

00000145 <inb>:
 145:	55                   	push   ebp
 146:	89 e5                	mov    ebp,esp
 148:	83 ec 10             	sub    esp,0x10
 14b:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 14e:	89 c2                	mov    edx,eax
 150:	ec                   	in     al,dx
 151:	88 45 ff             	mov    BYTE PTR [ebp-0x1],al
 154:	0f b6 45 ff          	movzx  eax,BYTE PTR [ebp-0x1]
 158:	c9                   	leave  
 159:	c3                   	ret    

0000015a <inw>:
 15a:	55                   	push   ebp
 15b:	89 e5                	mov    ebp,esp
 15d:	83 ec 10             	sub    esp,0x10
 160:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 163:	89 c2                	mov    edx,eax
 165:	66 ed                	in     ax,dx
 167:	66 89 45 fe          	mov    WORD PTR [ebp-0x2],ax
 16b:	0f b7 45 fe          	movzx  eax,WORD PTR [ebp-0x2]
 16f:	c9                   	leave  
 170:	c3                   	ret    

00000171 <outb>:
 171:	55                   	push   ebp
 172:	89 e5                	mov    ebp,esp
 174:	83 ec 04             	sub    esp,0x4
 177:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 17a:	88 45 fc             	mov    BYTE PTR [ebp-0x4],al
 17d:	0f b6 45 fc          	movzx  eax,BYTE PTR [ebp-0x4]
 181:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
 184:	ee                   	out    dx,al
 185:	90                   	nop
 186:	c9                   	leave  
 187:	c3                   	ret    
