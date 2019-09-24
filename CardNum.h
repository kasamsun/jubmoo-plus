#ifndef _CARDNUM_H_
#define _CARDNUM_H_

#define		NO_LEADED		0

#define		CLUB			0
#define		DIAMOND			1
#define		SPADE			2
#define		HEART			3

#define		GET_FACE(_cardnum_)		(((_cardnum_) & 0xf0)>>4)
#define		GET_NUM(_cardnum_)		((_cardnum_) & 0x0f)

#define		CARD_2			1
#define		CARD_3			2
#define		CARD_4			3
#define		CARD_5			4
#define		CARD_6			5
#define		CARD_7			6
#define		CARD_8			7
#define		CARD_9			8
#define		CARD_10			9
#define		CARD_J			10
#define		CARD_Q			11
#define		CARD_K			12
#define		CARD_A			13

#define		CLUB_2			0x01
#define		CLUB_3			0x02
#define		CLUB_4			0x03
#define		CLUB_5			0x04
#define		CLUB_6	 		0x05
#define		CLUB_7			0x06
#define		CLUB_8			0x07
#define		CLUB_9			0x08
#define		CLUB_10			0x09
#define		CLUB_J			0x0a
#define		CLUB_Q			0x0b
#define		CLUB_K			0x0c
#define		CLUB_A			0x0d

#define		DIAMOND_2		0x11
#define		DIAMOND_3 		0x12
#define		DIAMOND_4 		0x13
#define		DIAMOND_5 		0x14
#define		DIAMOND_6  		0x15
#define		DIAMOND_7 		0x16
#define		DIAMOND_8 		0x17
#define		DIAMOND_9 		0x18
#define		DIAMOND_10		0x19
#define		DIAMOND_J		0x1a
#define		DIAMOND_Q		0x1b
#define		DIAMOND_K		0x1c
#define		DIAMOND_A		0x1d

#define		SPADE_2			0x21
#define		SPADE_3			0x22
#define		SPADE_4			0x23
#define		SPADE_5			0x24
#define		SPADE_6			0x25
#define		SPADE_7			0x26
#define		SPADE_8			0x27
#define		SPADE_9			0x28
#define		SPADE_10		0x29
#define		SPADE_J			0x2a
#define		SPADE_Q			0x2b
#define		SPADE_K			0x2c
#define		SPADE_A			0x2d

#define		HEART_2			0x31
#define		HEART_3			0x32
#define		HEART_4			0x33
#define		HEART_5			0x34
#define		HEART_6			0x35
#define		HEART_7			0x36
#define		HEART_8			0x37
#define		HEART_9			0x38
#define		HEART_10		0x39
#define		HEART_J			0x3a
#define		HEART_Q			0x3b
#define		HEART_K			0x3c
#define		HEART_A			0x3d

#define HEART_2_VAL     -2
#define HEART_3_VAL     -3
#define HEART_4_VAL     -4
#define HEART_5_VAL     -5
#define HEART_6_VAL     -6
#define HEART_7_VAL     -7
#define HEART_8_VAL     -8
#define HEART_9_VAL     -9
#define HEART_10_VAL    -10
#define HEART_J_VAL     -20
#define HEART_Q_VAL     -30
#define HEART_K_VAL     -40
#define HEART_A_VAL     -50
#define CLUB_10_VAL     50
#define DIAMOND_J_VAL   100
#define SPADE_Q_VAL     -100

#define BIG_CHUAN_VAL	1000
#define NO_CARD_VAL     -50


#endif