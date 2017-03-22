#ifndef __RTC_H
#define __RTC_H

#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

/**
 * @brief Simple Time Struct
 */
typedef struct 
{
	unsigned short min;
	unsigned short hours;
} hour_min;

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define BIT(n) (0x01<<(n))
#define RTC_MIN 2
#define RTC_HOUR 4
#define RTC_REG_A 0x10
#define RTC_REG_B 0x11
#define RTC_UIP BIT(7)


/**
 * @brief Get Date from RTC.
 */
void rtc_get_date(void);

/**
 * @brief Wait for Valid RTC.
 */
void wait_valid_rtc(void);

/**
 * @brief Read Register of RTC.
 * @param resgister Register
 * @return Return value upon success and 1 otherwise.
 */
unsigned short read_register(int resgister);

/**
 * @brief Get Hour from RTC.
 */
int rtc_get_hour();

#endif /* __RTC_H */
