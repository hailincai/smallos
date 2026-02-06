#ifndef CMOS_H
#define CMOS_H
// CMOS 暫存器索引
#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

// RTC 暫存器位置
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS   0x04
#define RTC_DAY     0x07
#define RTC_MONTH   0x08
#define RTC_YEAR    0x09
#define RTC_STATUS_A 0x0A
#define RTC_STATUS_B 0x0B

int is_updating();
u8 read_rtc_register(int reg);
u8 bcd_to_bin(u8 bcd);
void get_rtc_time_string(char *buffer);
#endif