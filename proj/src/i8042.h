#ifndef __I8042_H
#define __I8042_H

/* KEYBOARD from Lab3 */

#define BIT(n) (0x01<<(n))
#define DELAY_US 20000
#define STAT_REG    0x64
#define KBC_CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60
#define PAR_ERR BIT(7)
#define STAT_TIMEOUT BIT(6)
#define TO_ERR  BIT(5)
#define M_or_B BIT(7)
#define ACK 0xFA
#define Resend 0xFE
#define ERROR 0xFC
#define COMANDO_LED 0xED
#define IBFULL BIT(1)
#define OBFULL BIT(0)

/* ----------------- */

/* MOUSE from Lab4 */

#define IRQ_MOUSE 12
#define WRITE_MOUSE 0xD4 
#define ENABLE_STREAM_MODE 0xEA
#define ENABLE_DATA 0xF4
#define DISABLE_DATA 0xF5
#define AUX BIT(5)
#define FIRST_BYTE_PACKET BIT(3) 
#define ENABLE_MOUSE 0xA8
#define STATUS_REQUEST 0xE9	

#endif
