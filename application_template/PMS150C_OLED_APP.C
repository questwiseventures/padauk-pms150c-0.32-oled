
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
	// This accepted example uses the shared glyph tile sequentially.
	CURSOR_COLUMN = 0;
	CURSOR_PAGE = 0;
	setCursor();
	NUMBER_VALUE = 37;
	drawNumber();
	A = 0x25;
	drawChar();
	// END APPLICATION DRAWING

	while (1)
	{
		// Add application-controlled updates here when needed.
	}
}
