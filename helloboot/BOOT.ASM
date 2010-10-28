; BOOT.ASM : By Vivek Mohan
; =========================
;
; tasm	boot.asm
; tlink /x /3 boot.obj

 _Text SEGMENT PUBLIC USE16
  assume CS:_Text, DS:_Text
  org 0

; This is the entry point for the program

Entry:
  db   0EAh  ; jmp far SEG:OFS  ; Currently we are at 0:7C00
  dw  OFFSET AfterData, 7C0h    ; This makes us be at 7C0:0

 ; Our Message to the world
 ; ========================

 HelloWorld db "Hello World !!",0 ;

 AfterData: 
   push CS
   pop  DS     ; update DS to be 7C0 instead of 0

 ; Display Hello World
 ; ===================

  mov si , OFFSET HelloWorld 
  do:
  mov AL, DS:[SI]
  inc SI
  or AL, AL
  jz done;
  mov BX, 0
  mov AH, 0Eh
  int 10h
  jmp do;
  done  : ;

 ; OK We have displayed the message
 ; ================================

  jmp $    ;

 ; Hang the system
 ; ===============

 ; Make the file 512 bytes long
 ; ============================
 org 510    
 
 ; Add the boot signature
 ; ======================

 dw 0AA55h  

  _Text ENDS
  END