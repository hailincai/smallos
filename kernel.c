void main() {
    // 指向 VGA 顯示記憶體起始位址
    char* video_memory = (char*) 0xb8000;

    // 記憶體佈局：[字元, 顏色屬性, 字元, 顏色屬性...]
    video_memory[0] = 'X';
    video_memory[1] = 0x0f; // 0x0f: 黑底白字
    video_memory[2] = 'Y';
    video_memory[3] = 0x0f;
    video_memory[4] = 'Z';
    video_memory[5] = 0x0f;

    while(1); // 讓 CPU 停在這裡，不要退出
}