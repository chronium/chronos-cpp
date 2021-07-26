#include "cmos.h"

#include <arch/x86_io.h>

using namespace platform;

int century_register = 0x00; // Set by ACPI table parsing code if possible

uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
uint8_t month;
uint32_t year;

bool get_update_in_progress_flag() {
  io_outb(CMOS::ADDRESS, 0x0A);
  return io_inb(CMOS::DATA) & 0x80;
}

uint8_t get_RTC_register(int reg) {
  io_outb(CMOS::ADDRESS, reg);
  return io_inb(CMOS::DATA);
}

void CMOS::Read() {
  uint8_t century = 21;
  uint8_t last_second;
  uint8_t last_minute;
  uint8_t last_hour;
  uint8_t last_day;
  uint8_t last_month;
  uint8_t last_year;
  uint8_t last_century;
  uint8_t registerB;

  // century_register = 0x00;

  while (get_update_in_progress_flag())
    ; // Make sure an update isn't in progress
  second = get_RTC_register(0x00);
  minute = get_RTC_register(0x02);
  hour = get_RTC_register(0x04);
  day = get_RTC_register(0x07);
  month = get_RTC_register(0x08);
  year = get_RTC_register(0x09);
  // if (century_register != 0) {
  century = get_RTC_register(century_register);
  //}

  do {
    last_second = second;
    last_minute = minute;
    last_hour = hour;
    last_day = day;
    last_month = month;
    last_year = year;
    last_century = century;

    while (get_update_in_progress_flag())
      ; // Make sure an update isn't in progress
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);
    if (century_register != 0) {
      century = get_RTC_register(century_register);
    }
  } while ((last_second != second) || (last_minute != minute) ||
           (last_hour != hour) || (last_day != day) || (last_month != month) ||
           (last_year != year) || (last_century != century));

  registerB = get_RTC_register(0x0B);

  // Convert BCD to binary values if necessary

  if (!(registerB & 0x04)) {
    second = (second & 0x0F) + ((second / 16) * 10);
    minute = (minute & 0x0F) + ((minute / 16) * 10);
    hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
    day = (day & 0x0F) + ((day / 16) * 10);
    month = (month & 0x0F) + ((month / 16) * 10);
    year = (year & 0x0F) + ((year / 16) * 10);
    if (century_register != 0) {
      century = (century & 0x0F) + ((century / 16) * 10);
    }
  }

  // Convert 12 hour clock to 24 hour clock if necessary

  if (!(registerB & 0x02) && (hour & 0x80)) {
    hour = ((hour & 0x7F) + 12) % 24;
  }

  // Calculate the full (4-digit) year

  if (century_register != 0) {
    year += century * 100;
  } else {
    year += (CURRENT_YEAR / 100) * 100;
    if (year < CURRENT_YEAR)
      year += 100;
  }
}

uint8_t CMOS::Second() { return second; }

uint8_t CMOS::Minute() { return minute; }

uint8_t CMOS::Hour() { return hour; }

uint64_t platform_timer_now() {
  CMOS::Read();
  return (CMOS::Hour() << 16) | (CMOS::Minute() << 8) | (CMOS::Second());
}
