[bits 32]
[extern main] ; 聲明 main 是一個外部符號（在 C 檔案裡定義）
call main ; 呼叫 C 語言的 main 函式
jmp $ ; 無限循環，防止回傳