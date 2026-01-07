#include "io.h" 
#include "types.h"
#include "cmos.h"
#include "util.h"

// 檢查 CMOS 是否正在更新（若在更新中讀取會拿到錯誤數值）
int is_updating() {
    port_byte_out(CMOS_ADDR, RTC_STATUS_A);
    return (port_byte_in(CMOS_DATA) & 0x80);
}

u8 read_rtc_register(int reg) {
    port_byte_out(CMOS_ADDR, reg);
    return port_byte_in(CMOS_DATA);
}

// BCD 轉十進位
u8 bcd_to_bin(u8 bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void get_rtc_time_string(char *buffer) 
{
    u8 second, minute, hour, day, month, year;
    
    // 防震盪讀取邏輯
    while (is_updating());
    second = read_rtc_register(RTC_SECONDS);
    minute = read_rtc_register(RTC_MINUTES);
    hour = read_rtc_register(RTC_HOURS);
    day  = read_rtc_register(RTC_DAY); // RTC_DAY_OF_MONTH
    month = read_rtc_register(RTC_MONTH); // RTC_MONTH
    year  = read_rtc_register(RTC_YEAR); // RTC_YEAR    

    u8 regB = read_rtc_register(RTC_STATUS_B);

    // 格式轉換
    if (!(regB & 0x04)) {
        second = bcd_to_bin(second);
        minute = bcd_to_bin(minute);
        hour = bcd_to_bin(hour);
        day  = bcd_to_bin(day);
        month = bcd_to_bin(month);
        year = bcd_to_bin(year);        
    }

    // 簡單的手動格式化 (YYYY-MM-dd HH:MM:SS)
    // 假設 buffer 至少有 20 bytes 空間
    // 年份處理 (假設是 20xx 年, 我們至少還有74年是正確的)
    buffer[0] = '2';
    buffer[1] = '0';
    buffer[2] = (year / 10) + '0';
    buffer[3] = (year % 10) + '0';
    buffer[4] = '-';
    
    // 月
    buffer[5] = (month / 10) + '0';
    buffer[6] = (month % 10) + '0';
    buffer[7] = '-';
    
    // 日
    buffer[8] = (day / 10) + '0';
    buffer[9] = (day % 10) + '0';
    buffer[10] = ' ';
    
    // 時:分:秒
    buffer[11] = (hour / 10) + '0';
    buffer[12] = (hour % 10) + '0';
    buffer[13] = ':';
    buffer[14] = (minute / 10) + '0';
    buffer[15] = (minute % 10) + '0';
    buffer[16] = ':';
    buffer[17] = (second / 10) + '0';
    buffer[18] = (second % 10) + '0';
    buffer[19] = '\0';
}