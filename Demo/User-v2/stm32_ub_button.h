#ifndef __STM32_UB_BUTTON_H
#define __STM32_UB_BUTTON_H

typedef enum
{
	BTN_USER = 0,
}BUTTON_NAME_t;

typedef enum
{
	BTN_UP =	0x0010,
	BTN_DOWN =	0x0100,
	BTN_EN =	0x0200,
	BTN_BACK =	0x0400

}BUTTON_STATUS_t;

typedef struct
{
	BUTTON_NAME_t	BUTTON_NAME;
	GPIO_TypeDef*	BUTTON_PORT;
	const u16		BUTTON_PIN;
	const u32		BUTTON_CLK;
	u8				BUTTON_STATUS;
	u16				BUTTON_MASK;
}BUTTON_t;

#endif

