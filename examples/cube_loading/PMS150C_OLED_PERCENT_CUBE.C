
//#include	"extern.h"
#include "feature_config.h"
#IF OLED_NEEDS_CHAR
#include "font_config.h"
#ENDIF

/*
	Note :	At switch I2C_SDA

	$ I2C_SDA	Low, Out	:	Error

	$ I2C_SDA	Out, Low	:	OK
*/

// Application template: select APIs in feature_config.h.
// Original board constants/timing/pins below are preserved.
// Library routines/state are included once from oled_library.h.
// Application drawing is clearly marked inside FPPA0.

// SECTION 1: Board configuration and original timing
System_Clock	=>	4000000;	//	User defined, ref. CLKMD, maybe 4/8/16-000000 or other ...
FPPA_Duty		=>	16;			//	User defined, ref. PMODE, maybe /2, /4, /8, /16

//	T_xxx		=>	xxx nS
T_High			=>	4700;
T_Low			=>	4700;
T_Start			=>	4700;
T_Stop			=>	4700;
T_Buf			=>	4700;
Delay_High		=>	T_High ?  (((System_Clock / FPPA_Duty) / (1000000000 / T_High)) + 1) : 0;
Delay_Low		=>	T_Low ?   (((System_Clock / FPPA_Duty) / (1000000000 / T_Low)) + 1) : 0;
Delay_Start		=>	T_Start ? (((System_Clock / FPPA_Duty) / (1000000000 / T_Start)) + 1) : 0;
Delay_Stop		=>	T_Stop ?  (((System_Clock / FPPA_Duty) / (1000000000 / T_Stop)) + 1) : 0;
Delay_Buf		=>	T_Buf ?   (((System_Clock / FPPA_Duty) / (1000000000 / T_Buf)) + 1) : 0;

//#if	(Delay_High >= 256) || (Delay_Low >= 256)
//	.ERROR	I2C_Delay is %Delay_High, %Delay_Low, and >= 256
//	.ENDP
//#endif

Easy_Delay	macro	val, cmp
	#IF	val > cmp
		.delay	val - cmp;
	#ENDIF
	endm


I2C_SDA		BIT	PA.3;
I2C_SCL		BIT	PA.4;




#include "oled_library.h"

#include "app_config.h"
// Application logo loader: one shared tile index, no image framebuffer.
BYTE IMAGE_TILE;
#include "cube_logo.h"

void drawCubeLogo (void)
{
	IMAGE_TILE = 0;
	CURSOR_PAGE = 0;
	while (CURSOR_PAGE < 4)
	{
		CURSOR_COLUMN = 0;
		while (CURSOR_COLUMN < 4)
		{
			Cube_LoadTile();
			setCursor();
			BITMAP_POINT = CELL_DATA;
			drawBitmap();
			IMAGE_TILE = IMAGE_TILE + 1;
			CURSOR_COLUMN = CURSOR_COLUMN + 1;
		}
		CURSOR_PAGE = CURSOR_PAGE + 1;
	}
}

void FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4		//	SYSCLK=IHRC/4

	//	Insert Initial Code
	$ I2C_SDA	Low, In;	//	Pull be skip
	$ I2C_SCL	High, Out;





	//PAC.6 = 1; //  set PA.6 as toggle output
	//PA.6 = 0;

	OLED_Init();
	A = APP_ROTATION;
	setRotation();
	ClearScreen();

	// BEGIN APPLICATION DRAWING
	drawCubeLogo(); // Static image; not redrawn or cleared during loading.
	BAR_PERCENT = 0;
	while (1)
	{
#IF APP_PORTRAIT
		CURSOR_COLUMN = 0;
		CURSOR_PAGE = 4;
#ELSE
		CURSOR_COLUMN = 4;
		CURSOR_PAGE = 1;
#ENDIF
		setCursor();
		BAR_LENGTH = 3;
		drawPercentBar();
		// Redraw the complete three-digit field below the loading bar.
#IF APP_PORTRAIT
		CURSOR_COLUMN = 0;
		CURSOR_PAGE = 6;
#ELSE
		CURSOR_COLUMN = 4;
		CURSOR_PAGE = 3;
#ENDIF
		setCursor();
		// Bar draw is complete: reuse its private arithmetic scratch sequentially.
		// This application is tied to the frozen v1.0.0 scratch contract.
		BAR_BITS = BAR_PERCENT;
		BAR_STEP = 0;
		if (BAR_BITS == 100)
		{
			BAR_BITS = 0;
		}
		while (BAR_BITS >= 10)
		{
			BAR_BITS = BAR_BITS - 10;
			BAR_STEP = BAR_STEP + 1;
		}
		BAR_RIGHT = 0x20;
		if (BAR_PERCENT == 100) BAR_RIGHT = 0x31;
		A = BAR_RIGHT;
		drawChar();
		CHAR_COLUMN = CHAR_COLUMN + 1;
		BAR_RIGHT = 0x20;
		if ((BAR_PERCENT == 100) || (BAR_STEP != 0)) BAR_RIGHT = BAR_STEP + 0x30;
		A = BAR_RIGHT;
		drawChar();
		CHAR_COLUMN = CHAR_COLUMN + 1;
		A = BAR_BITS + 0x30;
		drawChar();

		.delay APP_HOLD_CYCLES; // Application cadence, including full/empty states.
		BAR_PERCENT = BAR_PERCENT + 5;
		if (BAR_PERCENT > 100) BAR_PERCENT = 0;
	}
	// END APPLICATION DRAWING
}
