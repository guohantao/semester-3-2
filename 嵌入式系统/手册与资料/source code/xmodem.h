/*
 * xmodem.h
 *
 */
#ifndef __XMODEM_H__
#define __XMODEM_H__

//#define USE_FIFO

#ifdef USE_FIFO
	#define SERIAL_CHAR_READY()     ((rUFSTAT0 & 0x0f))
	#define SERIAL_READ_CHAR()      (rURXH0)
	#define SERIAL_READ_STATUS()    (rUERSTAT0 & 0x0f)

	#define SERIAL_WRITE_READY()	(!(rUFSTAT0 & 0x200))
	#define SERIAL_WRITE_CHAR(c)	(rUTXH0 = (c))
#else
	#define SERIAL_CHAR_READY()     (rUTRSTAT0 & (1 << 0))
	#define SERIAL_READ_CHAR()      rURXH0
	#define SERIAL_READ_STATUS()    (rUERSTAT0 & 0x0f)

	#define SERIAL_WRITE_READY()	(rUTRSTAT0 & (1 << 2))
	#define SERIAL_WRITE_CHAR(c)	WrUTXH0(c)
#endif

#define __u8 unsigned char
#define __u32 unsigned long
typedef void (*vfuncp)(void);

#define DEBUGBUF_SIZE 4096
extern char debugbuf[DEBUGBUF_SIZE];

/* returns the number of bytes transferred */
__u32 xmodem_receive(char *dest, size_t len);

void u_putc(char c);

#endif //__XMODEM_H__