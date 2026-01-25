; kernel/user_bin.s
[section .data]
[global user_program_start]
[global user_program_end]

user_program_start:
    incbin "init.bin"      ; NASM 會在編譯時將 init.bin 的內容直接塞入此處
user_program_end: