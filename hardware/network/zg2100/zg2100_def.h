/***************************************************************************
 *            zg2100_def.h
 *
 *  Tue Sep  3 19:14:22 2013
 *  Copyright  2013  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef _ZG2100_DEF_H
	#define ZG2100_DEF_H

	#define ZG2100_HEAD_BUFFER_SIZE				64
	#define ZG2100_FIFO_BUFFER_SIZE				2048

	#define ZG2100_MAX_TX_PENDING				7

	#define ZG2100_SCAN_PROBE_DELAY				10
	#define ZG2100_SCAN_MIN_CHAN_TIME			400
	#define ZG2100_SCAN_MAX_CHAN_TIME			800

	#define	ZG2100_SSID_LEN						32
	#define	ZG2100_MAX_SUPPORTED_RATES			8
	#define	ZG2100_SCAN_MAX_CHANNELS			11
	#define	ZG2100_MAX_CHANNELS					14
	#define	ZG2100_MACADDR_LEN					6
	#define	ZG2100_SNAP_LEN						6

	#define ZG2100_WEP_KEYS_COUNT				4
	#define ZG2100_WEP_KEY_LEN					13
	#define ZG2100_WPA_PASS_LEN					64
	#define ZG2100_PMK_LEN						32

	/* Region */

	#define ZG2100_REGION						2

	#define ZG2100_REG_LEN( reg )				( ( reg ) >= 0x25 ? 2:1 )

	/*ZG2100 commands */

	#define ZG2100_CMD_REG						0x00
	#define ZG2100_CMD_REG_READ					(ZG2100_CMD_REG | 0x40 )
	#define ZG2100_CMD_REG_WRITE				(ZG2100_CMD_REG | 0x00 )

	#define ZG2100_CMD_FIFO						0x80
	#define ZG2100_CMD_FIFO_RD_DATA				( ZG2100_CMD_FIFO | 0x00 )
	#define ZG2100_CMD_FIFO_RD_MGMT				( ZG2100_CMD_FIFO | 0x00 )
	#define ZG2100_CMD_FIFO_RD_DONE				( ZG2100_CMD_FIFO | 0x50 )
	#define ZG2100_CMD_FIFO_WR_DATA				( ZG2100_CMD_FIFO | 0x20 )
	#define ZG2100_CMD_FIFO_WR_MGMT				( ZG2100_CMD_FIFO | 0x30 )
	#define ZG2100_CMD_FIFO_WR_DONE				( ZG2100_CMD_FIFO | 0x40 )

	/* Register */

	#define ZG2100_REG_INTF						0x01
	#define ZG2100_REG_INTE						0x02
	#define ZG2100_REG_SYSINFO					0x21
	#define ZG2100_REG_SYSINFO_INDEX			0x2b
	#define ZG2100_REG_INTF2					0x2d
	#define ZG2100_REG_INTE2					0x2e

	#define ZG2100_REG_F0_ROOM					0x2f
	#define ZG2100
	#define ZG2100_REG_F0_LEN					0x33
	#define ZG2100_REG_F1_LEN					0x35
	#define ZG2100_REG_IREG_ADDR				0x3e
	#define ZG2100_REG_IREG_DATA				0x3f

	#define ZG2100_IREG_HW_STATUS				0x2a
	#define ZG2100_IREG_HW_RST					0x2b

	#define ZG2100_HW_STATUS_RESET				0x1000

	/* Mask for ZG2100_REG_INT* */

	#define ZG2100_INT_MASK_F0					0x40
	#define ZG2100_INT_MASK_F1					0x80
	#define ZG2100_REG_F_LEN(n)					((n==0)?ZG2100_REG_F0_LEN:ZG2100_REG_F1_LEN)
	#define ZG2100_INT_MASK_F(n)				((n==0)?ZG2100_INT_MASK_F0:ZG2100_INT_MASK_F1)

	/* FIFO */

	#define ZG2100_FIFO_DATA					0
	#define ZG2100_FIFO_MGMT					1
	#define ZG2100_FIFO_ANY						0

	/* Types */

	#define ZG2100_FIFO_WR_TXD_REQ				1
	#define ZG2100_FIFO_RD_TXD_ACK				1
	#define ZG2100_FIFO_RD_RXD_AVL				3
	#define ZG2100_FIFO_WR_MGMT_REQ				2
	#define ZG2100_FIFO_RD_MGMT_AVL				2
	#define ZG2100_FIFO_RD_MGMT_EVT				4

	/* Subtypes */

	#define ZG2100_FIFO_TXD_STD					1
	#define ZG2100_FIFO_RXD_STD					1

	/* Managment */

	/* Managment Events */

	#define ZG2100_FIFO_MGMT_DISASSOC			1
	#define ZG2100_FIFO_MGMT_DEAUTH				2

	/* Managment Commands */

	#define ZG2100_FIFO_MGMT_CONNECT 		19
	#define ZG2100_FIFO_MGMT_CONNECT_MANAGE 20

	#define ZG2100_FIFO_MGMT_SCAN				1
	#define ZG2100_FIFO_MGMT_SET_WEP_KEY		10
	#define ZG2100_FIFO_MGMT_PSK_CALC			12
	#define ZG2100_FIFO_MGMT_PMK_KEY			8
	#define ZG2100_FIFO_MGMT_SCAN_GET_RESULT	32
	#define ZG2100_FIFO_MGMT_PARM_SET			15
	#define ZG2100_FIFO_MGMT_PARM_GET			16

	/* Managment Params */
	#define ZG2100_FIFO_MGMT_PARM_MACAD			1
	#define ZG2100_FIFO_MGMT_PARM_REGION		2
	#define ZG2100_FIFO_MGMT_PARM_SYSV			26

	#define	ZG2100_PROBE_ACTIVE					1
	#define ZG2100_PROBE_PASSIVE				2

	#define ZG2100_BSS_INFRA					1
	#define ZG2100_BSS_ADHOC					2
	#define ZG2100_BSS_ANY						3

	typedef struct __ZG2100_SCAN_REQ
	{
		int probe_delay;
		int min_chan_time;
		int max_chan_time;
		char bssid_mask[ ZG2100_MACADDR_LEN ];
		char bss_type;
		char probe_req;
		char ssid_len;
		char channel_count;
		char ssid[ ZG2100_SSID_LEN ];
		char channels[ ZG2100_MAX_CHANNELS ];
	} ZG2100_SCAN_REQ;

	typedef struct __ZG2100_SCAN_RES
	{
		char type;
		char subtype;
		char result;
		char macstate;
	} ZG2100_SCAN_RES;

	typedef struct __ZG2100_SCAN_ITEM
	{
		char bssid[ZG2100_MACADDR_LEN];
		char ssid[ZG2100_SSID_LEN];
		char apConfig;
		char reserved;
		int beacon_period;
		int atim_wdw;
		char supported_rates[ZG2100_MAX_SUPPORTED_RATES];
		char rssi;
		char supported_rates_count;
		char dtim_period;
		char bss_type;
		char channel;
		char ssid_len;
	} ZG2100_SCAN_ITEM;

	typedef struct __ZG2100_CONNECT_REQ
	{
		char security;
		char ssid_len;
		char ssid[ZG2100_SSID_LEN];
		int sleep_duration;
		char bss_type;
		char pad;
	} ZG2100_CONNECT_REQ;

	typedef struct __ZG2100_WEP_REQ
	{
		char slot;
		char key_size;
		char default_key;
		char ssid_len;
		char ssid[ZG2100_SSID_LEN];
		char keys[ZG2100_WEP_KEYS_COUNT][ZG2100_WEP_KEY_LEN];
	} ZG2100_WEP_REQ;

	typedef struct __ZG2100_PSK_CALC_REQ
	{
		char config;
		char phrase_len;
		char ssid_len;
		char _pad;
		char ssid[ZG2100_SSID_LEN];
		char pass_phrase[ZG2100_WPA_PASS_LEN];
	} ZG2100_PSK_CALC_REQ;

	typedef struct _ZG2100_PSK_CALC_RES
	{
		char result;
		char state;
		char key_returned;
		char _pad;
		char key[ZG2100_PMK_LEN];
	} ZG2100_PSK_CALC_RES;

	typedef struct __ZG2100_PMK_REQ
	{
		char slot;
		char ssid_len;
		char ssid[ZG2100_SSID_LEN];
		char key[ZG2100_PMK_LEN];
	} ZG2100_PMK_REQ;

	typedef struct __ZG2100_RXD_HDR
	{
		char rssi;
		char dest[6];
		char src[6];
		long dtime;
		int len;
		char snap[ZG2100_SNAP_LEN];
		int eth_type;
	} ZG2100_RXD_HDR;

#endif /* ZG2100_DEF_h */