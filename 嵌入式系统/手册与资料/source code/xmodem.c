/*
 * vivi/deriver/serialxmodem.c:
 *     - an implementation of the xmodem protocol from the spec.
 *
 * Based on bootldr/xmodem.c (C) 2000 Compaq Computer Coporation.
 *
 * Copyright (C) 2001 MIZI Research, Inc.
 *
 *
 * 1999-01-xx: Edwin Foo <efoo@crl.dec.com>
 *    - Initial code
 *
 * 2001-10-04: Janghoon Lyu <nandy@mizi.com>
 *    - Modified a little bit.
 *
 * 2002-07-04: Janghoon Lyu <nandy@mizi.com>
 *    - get_param_value() ÀÎÅÍÆäÀÌ½º°¡ ¹Ù²î¾î¼­ ÀÌ¿¡ ¸Â°Ô ¼öÁ¤.
 */

#include <stdio.h>
#include <string.h>
#include "def.h"
#include "option.h"
#include "2410addr.h"
#include "2410lib.h"
#include "2410slib.h"
#include "xmodem.h"

/* XMODEM parameters */
#define BLOCK_SIZE_1	128	/* size of transmit blocks , SOH=0x01Ê±µÄblock´óÐ¡*/
#define BLOCK_SIZE_2	1024	/* size of transmit blocks , SOH=0x02Ê±µÄblock´óÐ¡*/
#define BLOCK_SIZE block_size
#define RETRIES		20	/* maximum number of RETRIES */

static int block_size = BLOCK_SIZE_1;

/* Line control codes */
#define SOH_1		0x01	/* start of header */
#define SOH_2		0x02	/* 1K xmodem start of header */
#define ACK		0x06	/* Acknowledge */
#define NAK		0x15	/* Negative acknowledge */
#define CAN		0x18	/* Cancel */
#define EOT		0x04	/* end of text */

#define GET_BYTE_TIMEOUT 1000000
//#define GET_BYTE_TIMEOUT (1000 * 5)	//Ãë


/* global error variable */
char *xmodem_errtxt = NULL;
int get_byte_err = 0;
__u8 volatile rbuffer[BLOCK_SIZE_2];

/* prototypes of helper functions */
int get_record(void);
__u8 get_byte(void);
void u_putc(char c);

enum
{
	SAC_SEND_NAK = 0,
	SAC_SENT_NAK = 1,
	SAC_PAST_START_NAK = 2
};

static volatile int seen_a_char = SAC_SEND_NAK;

static int one_nak = 0;
static unsigned long xmodem_timeout = GET_BYTE_TIMEOUT;

char debugbuf[DEBUGBUF_SIZE];
int db_idx = 0;

void bufputs(char *s)
{
	size_t len = strlen(s) + 1;

	if (len + db_idx > sizeof(debugbuf))
		len = sizeof(debugbuf) - db_idx;

	if (len) {
		memcpy(&debugbuf[db_idx], s, len);
		db_idx += len;
	}
}

void reset_debugbuf(void)
{
	memset(debugbuf, 0x0, sizeof(debugbuf));
	db_idx = 0;
}

__u32 xmodem_receive(char *dldaddr, size_t len)
{
	char ochr;
	int r = 0, rx_block_num = 0, error_count = 0;
	__u32 foffset = 0;
	int i;
	//int ret;

	xmodem_errtxt = NULL;
	seen_a_char = 0;

	//one_nak = get_param_value("xmodem_one_nak", &ret);
	one_nak = 0;

	//xmodem_timeout = get_param_value("xmodem_initial_timeout", &ret);
	xmodem_timeout = GET_BYTE_TIMEOUT/2;

	rx_block_num = 1;
	error_count = RETRIES;

	do {
		if ((r = get_record()) == (rx_block_num & 255)) {
			error_count = RETRIES;
			for (i = 0; i <BLOCK_SIZE; i++)
				*(__u8 *)(dldaddr+foffset+i) = rbuffer[i];
			xmodem_errtxt = "RX PACKET";
			rx_block_num++;
			ochr = ACK;
			foffset += BLOCK_SIZE;
		} else {
			switch (r) {
			case -1: /* TIMEOUT */
				xmodem_errtxt = "TIMEOUT";
				ochr = NAK;
				break;
			case -2: /* Bad block */
				xmodem_errtxt = "BAD BLOCK#";
				/* eat teh rest of the block */
				get_byte_err = 0;
				while (get_byte_err != -1) get_byte();
				ochr = NAK;
				break;
			case -3: /* Bad checksum */
				xmodem_errtxt = "BAD CHKSUM";
				ochr = NAK;
				break;
			case -4: /* End of file */
				xmodem_errtxt = "DONE";
				ochr = ACK;
				break;
			case -5: /* Cancel */
				xmodem_errtxt = "ABORTED";
				ochr = ACK;
				break;
			default: /* Block out of sequence */
				xmodem_errtxt = "WRONG BLK";
				ochr = NAK;
			}
			error_count--;
		}
		u_putc(ochr);
	} while ((r > -3) && error_count);

	if ((!error_count) || (r != -4)) {
		foffset = 0;	/* indicate failure to caller */
		/*printk("x-modem error: %s\n", xmodem_errtxt); */
	}

	return foffset;
}

/*
 * Read a record in the XMODEM protocol, return the block number
 * (0-255) if successful, or one of the following return codes:
 * 	-1 = Bad byte
 * 	-2 = Bad block number
 * 	-3 = Bad block checksum
 * 	-4 = End of file
 * 	-5 = Canceled by remote
 */
int get_record(void)
{
	int c, block_num = 0;
	int i;
	__u32 check_sum;

	/* clear the buffer */
	for (i = 0; i < BLOCK_SIZE; i++)
		rbuffer[i] = 0x00;

	check_sum = 0;
	i = -2;
	c = get_byte();
	if (get_byte_err)
		return -1;

	switch (c) {
	case SOH_1:	/* Receive packet */
		BLOCK_SIZE = BLOCK_SIZE_1;
		goto recv_soh;
	case SOH_2:	/* Receive packet */
		BLOCK_SIZE = BLOCK_SIZE_2;
recv_soh:
		for (;;) {
			c = get_byte();
			if (get_byte_err)
				return -1;

			switch (i) {
			case -2: 
				block_num = c;
				break;
			case -1:
#if 0
#ifdef CHECK_NEGATED_SECTNUM
				if (c != (-block_num -1))
					return -2;
#endif
#endif
				break;
			case BLOCK_SIZE_1:
			case BLOCK_SIZE_2:
				if(BLOCK_SIZE == i)
				{
					if ((check_sum & 0xff) != c)
						return -3;
					else
						return block_num;
					break;
				}
			default:
				rbuffer[i] = c;
				check_sum += c;
			}
			i++;
		}
	case EOT:	/* end of file encountered */
		return -4;
	case CAN:	/* cancel protocol */
		return -5;
	default:
		return -5;
	}
}

//
int getc_errno = 0;
__u8 do_getc(vfuncp idler, unsigned long timeout, int *statp)
{
	__u8	c, rxstat;
	int	do_timeout = timeout != 0;
	//unsigned long t = GetTickCount();

	getc_errno = 0;	/* reste errno */

	while(!SERIAL_CHAR_READY()) {

		if (do_timeout) {
/*			if (GetTickCount()-t > timeout)
			{
				timeout = 0;
				break;
			}*/
			if(timeout == 0)
				break;
			timeout--;
		}

		if (idler)
			idler();
	}

	if (do_timeout && timeout == 0) {
		c = 0;
		rxstat = (__u8)-1;
	} else {
		c = SERIAL_READ_CHAR();
		rxstat = SERIAL_READ_STATUS();
	}

	if (rxstat) {
		getc_errno = rxstat;
		/*printk("RXSTAT error. status = 0x%08lx", rxstat);*/
		if (statp)
			*statp = rxstat;
	}
	return (c);
}

/*
 * Reads and returns a character from the serial port
 *   - Times out after delay iterations checking for presence of character
 *   - Sets *error_p to UART error bits or - on timeout
 *   - On timeout, sets *error_p to -1 and returns 0
 */
char awaitkey(unsigned long delay, int* error_p)
{
	return (do_getc(NULL, delay, error_p));
}


void u_putc(char c)
{
	while (!SERIAL_WRITE_READY());
	SERIAL_WRITE_CHAR(c);
}


__u8 get_byte()
{
	int c;

again:
	c = 0;
	get_byte_err = 0; /* reset errno */
	c = awaitkey(xmodem_timeout, &get_byte_err);

		if (get_byte_err) {
		if (seen_a_char == SAC_SEND_NAK || !one_nak) {
			bufputs("timeout nak");
			u_putc(NAK);	/* make the sender go */
		}

		if (seen_a_char < SAC_PAST_START_NAK) {
			bufputs("goto again");
			seen_a_char = SAC_SENT_NAK;

			//xmodem_timeout = get_param_value("xmodem_timeout", &ret);
			xmodem_timeout = GET_BYTE_TIMEOUT;
			goto again;
		}
	}

	if (get_byte_err == 0)
		seen_a_char = SAC_PAST_START_NAK;

	return (c);
}
