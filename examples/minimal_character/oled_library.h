// SECTION 2: Original I2C transport (accumulator A input)
static	void	I2C_Write_Acc (void)
{
	BYTE	data, count;
	data	=	A ^ 0xFF; // Explicit 8-bit complement.
	count	=	8;

	do
	{
		$ I2C_SCL	Low;			//	1
		sl		data;				//	2
		#if	_SYS(OP:SWAPC IO.N)
			swapc	_PXC(I2C_SDA);		//	3
			$ I2C_SDA	Low;			//	4
		Easy_Delay (Delay_Low, 4);
		#else
			if (!CF)	$ I2C_SDA	In;		//	34
			if (CF)		$ I2C_SDA	Out;	//	56
			$ I2C_SDA	Low;				//	7
		Easy_Delay (Delay_Low, 7);
		#endif

		$ I2C_SCL	High;			//	1
		Easy_Delay (Delay_High, 3);
	} while (--count);				//	2, 3
	nop;							//	3

	$ I2C_SCL	Low;				//	1
	$ I2C_SDA	In;					//	2
	Easy_Delay (Delay_Low, 2);

	$ I2C_SCL	High;				//	1
//	if (I2C_SDA) trap;				//	Pleass add you code, when no Ack ..
	Easy_Delay (Delay_High, 2);
}		



static	void	I2C_Stop (void)
{
	$ I2C_SCL	Low;				//	1
//	if (! I2C_SDA) trap;
	$ I2C_SDA	Out, Low;			//	2/3	SDA	:	Output Low
	Easy_Delay (Delay_Low, 3);

	$ I2C_SCL	High;				//	1
	Easy_Delay (Delay_Stop, 1);

	$ I2C_SDA	In;					//	1	SDA	:	Pull High	:	Stop
	Easy_Delay (Delay_Buf, 2);
}	

// SECTION 3: Shared OLED command and address state
WORD	I2C_Address;				//	Original WORD storage; command routines use its low byte.
									//	Load the command into I2C_Address before I2C_Write_Command2.

BYTE COL_START_ADDRESS;
BYTE COL_END_ADDRESS;
BYTE PAGE_START_ADDRESS;
BYTE PAGE_END_ADDRESS;

// drawChar inputs: logical cell column/row within TEXT_COLUMNS/TEXT_ROWS.
// Set both to valid CELL indexes, then load A with an enabled font code.
// No clipping, wrapping, or automatic cursor advance.
BYTE CHAR_COLUMN;
BYTE CHAR_PAGE;

// Rotation text coordinates are CELL indexes, not raw controller columns.
BYTE ROTATION;     // 0,1,2,3 = clockwise 0,90,180,270 degrees
BYTE TEXT_COLUMNS; // 7 landscape, 4 portrait
BYTE TEXT_ROWS;    // 4 landscape, 7 portrait

void	setRotation (void)
{
	ROTATION = A;
	if (ROTATION > 3) ROTATION = 0;
	TEXT_COLUMNS = 7;
	TEXT_ROWS = 4;
	if (ROTATION.0)
	{
		TEXT_COLUMNS = 4;
		TEXT_ROWS = 7;
	}
	CHAR_COLUMN = 0;
	CHAR_PAGE = 0;
}



enum	{	I2C_WR_CMD = 0x78, I2C_RD_CMD = 0x79 };


// SECTION 4: OLED command transactions (original ACK retry behavior)
// Shared START/address/ACK retry. A and flags are clobbered.
// Original pin order and retry loop retained; call adds stack depth.
static	void	I2C_BeginWrite (void)
{
	while (1)
	{

		// 1/2	SDA	:	Output Low	:	Start
		$ I2C_SDA	Out, Low;	

		//Slave Address
		A = I2C_WR_CMD;
		I2C_Write_Acc();

		//Wait For ACK
		if (I2C_SDA)
		{
			I2C_Stop();
			continue;
		}

		break;
	}

}

void	I2C_Write_Command1 (void)
{

	BYTE	command =	A;

	I2C_BeginWrite();

	//Control Byte
	A = 0x00;
	I2C_Write_Acc();

	//Command Byte
	A = command;
	I2C_Write_Acc();

	//I2C Stop
	I2C_Stop();


}

void	I2C_Write_Command2 (void)
{
	BYTE	I2C_Data	=	A;

	I2C_BeginWrite();

	//Control Byte
	A = 0x00;
	I2C_Write_Acc();

	//Register Address Byte
	A	=	I2C_Address$0; // Mini-C low-byte command extraction.
	I2C_Write_Acc();

	//Data Byte
	A	=	I2C_Data;
	I2C_Write_Acc();


	//I2C Stop
	I2C_Stop();



}

// SECTION 5: OLED column and page addressing
void	Set_Column_Address (void)
{

	I2C_BeginWrite();

	//Control Byte
	A = 0x00;
	I2C_Write_Acc();

	//Register Address Byte
	A	=	0x21; //I2C register address		
	I2C_Write_Acc();

	//Col Start Address
	A	=	COL_START_ADDRESS; 	
	I2C_Write_Acc();

	//Col End Address
	A	=	COL_END_ADDRESS; 	
	I2C_Write_Acc();


	//I2C Stop
	I2C_Stop();



}

void	Set_Page_Address (void)
{

	I2C_BeginWrite();

	//Control Byte
	A = 0x00;
	I2C_Write_Acc();

	//Register Address Byte
	A	=	0x22; //I2C register address		
	I2C_Write_Acc();

	//Page Start Address
	A	=	PAGE_START_ADDRESS; 	
	I2C_Write_Acc();

	//Page End Address
	A	=	PAGE_END_ADDRESS; 	
	I2C_Write_Acc();


	//I2C Stop
	I2C_Stop();



}

// SECTION 6: Clear original display window (columns 34-93, pages 0-3)
void	ClearScreen (void)
{

	BYTE x;
	BYTE y;
	x = 0;
	y = 0;

	//Set Column Address
	COL_START_ADDRESS = 34;
	COL_END_ADDRESS = 93;
	Set_Column_Address();

	//Set Page Address
	PAGE_START_ADDRESS = 0; 
	PAGE_END_ADDRESS = 3;
	Set_Page_Address();	

	I2C_BeginWrite();

	//Control Byte
	A = 0x40;
	I2C_Write_Acc();

	//Write Data
	while (y < 4){

		x = 0;
		while (x < 60){

			A = 0b00000000;
			//A = 0b11111111;
			I2C_Write_Acc();

			//Increment Col
			x = x + 1;
		}

		//Increment Row
		y = y + 1;

	}

	//I2C Stop
	I2C_Stop();
}


// SECTION 7: Positioned alphanumeric character output (A, CHAR_COLUMN, CHAR_PAGE)
// Internal shared 8x8-cell source. Set immediately before OLED_WriteCell.
// Shared glyph/tile workspace; overwritten by every drawChar call.
#IF OLED_NEEDS_TILE_BUFFER
BYTE CELL_DATA[8];
#ENDIF
#IF OLED_NEEDS_CELL_RENDER
WORD CELL_SOURCE;
#ENDIF
#IF OLED_NEEDS_BITMAP
WORD BITMAP_POINT; // Public RAM address of eight column bytes for drawBitmap.
#ENDIF

#IF OLED_NEEDS_CELL_RENDER
static	void	OLED_WriteCell (void)
{
	BYTE x, i, j, bits, mask;
	WORD Point;

	// Map logical cell coordinates to the fixed physical 7x4 cell grid.
	switch (ROTATION)
	{
		case 0 :
			COL_START_ADDRESS = CHAR_COLUMN;
			PAGE_START_ADDRESS = CHAR_PAGE;
			break;
		case 1 :
			COL_START_ADDRESS = 6 - CHAR_PAGE;
			PAGE_START_ADDRESS = CHAR_COLUMN;
			break;
		case 2 :
			COL_START_ADDRESS = 6 - CHAR_COLUMN;
			PAGE_START_ADDRESS = 3 - CHAR_PAGE;
			break;
		default :
			COL_START_ADDRESS = CHAR_PAGE;
			PAGE_START_ADDRESS = 3 - CHAR_COLUMN;
			break;
	}
	sl COL_START_ADDRESS;
	sl COL_START_ADDRESS;
	sl COL_START_ADDRESS;
	COL_START_ADDRESS = COL_START_ADDRESS + 34;
	COL_END_ADDRESS = COL_START_ADDRESS + 7;
	PAGE_END_ADDRESS = PAGE_START_ADDRESS;
	Set_Column_Address();
	Set_Page_Address();

	I2C_BeginWrite();
	A = 0x40;
	I2C_Write_Acc();

	// Produce one rotated column at a time, without a second bitmap buffer.
	x = 0;
	mask = 0x80;
	if (ROTATION == 3) mask = 1;
	while (x < 8)
	{
		if (ROTATION.0)
		{
			bits = 0;
			j = 0;
			do
			{
				sr bits;
				Point = CELL_SOURCE + j;
				A = *Point;
				A = A & mask;
				if (A != 0) bits = bits | 0x80;
				j = j + 1;
			} while (j < 8);
		}
		else
		{
			j = x;
			if (ROTATION == 2) j = 7 - x;
			Point = CELL_SOURCE + j;
			bits = *Point;
		}

		// 180/270 degrees also reverse the bits in each output column.
		if (ROTATION.1)
		{
			j = 0;
			i = 8;
			do
			{
				sl j;
				if (bits.0) j = j | 1;
				sr bits;
			} while (--i);
			bits = j;
		}
		A = bits;
		I2C_Write_Acc();
		if (ROTATION == 1) sr mask;
		else sl mask;
		x = x + 1;
	}
	I2C_Stop();
}
#ENDIF

#IF OLED_NEEDS_CHAR
void	drawChar (void)
{

	BYTE x;
	x = A; // Preserve character before shared buffer initialization.
	CELL_DATA[0] = 0x00;
	CELL_DATA[6] = 0x00;
	// Common bitmap columns; cases override only differing values.
	CELL_DATA[1] = 0x7F;
	CELL_DATA[2] = 0x41;
	CELL_DATA[3] = 0x49;
	CELL_DATA[4] = 0x40;
	CELL_DATA[5] = 0x00;
	CELL_DATA[7] = 0x00; // Full opaque 8x8 cell padding.

    switch  (x)
    {
#IF FONT_20
		case 0x20 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x00;
			CELL_DATA[3] = 0x00;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_21
		case 0x21 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x00;
			CELL_DATA[3] = 0x5F;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_22
		case 0x22 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x07;
			CELL_DATA[3] = 0x00;
			CELL_DATA[4] = 0x07;
			break;
#ENDIF

#IF FONT_23
		case 0x23 :
			CELL_DATA[1] = 0x14;
			CELL_DATA[2] = 0x7F;
			CELL_DATA[3] = 0x14;
			CELL_DATA[4] = 0x7F;
			CELL_DATA[5] = 0x14;
			break;
#ENDIF

#IF FONT_24
		case 0x24 :
			CELL_DATA[1] = 0x24;
			CELL_DATA[2] = 0x2A;
			CELL_DATA[3] = 0x7F;
			CELL_DATA[4] = 0x2A;
			CELL_DATA[5] = 0x12;
			break;
#ENDIF

#IF FONT_25
		case 0x25 :
			CELL_DATA[1] = 0x23;
			CELL_DATA[2] = 0x13;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x64;
			CELL_DATA[5] = 0x62;
			break;
#ENDIF

#IF FONT_26
		case 0x26 :
			CELL_DATA[1] = 0x36;
			CELL_DATA[2] = 0x49;
			CELL_DATA[3] = 0x55;
			CELL_DATA[4] = 0x22;
			CELL_DATA[5] = 0x50;
			break;
#ENDIF

#IF FONT_27
		case 0x27 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x05;
			CELL_DATA[3] = 0x03;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_28
		case 0x28 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x1C;
			CELL_DATA[3] = 0x22;
			CELL_DATA[4] = 0x41;
			break;
#ENDIF

#IF FONT_29
		case 0x29 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[3] = 0x22;
			CELL_DATA[4] = 0x1C;
			break;
#ENDIF

#IF FONT_2A
		case 0x2A :
			CELL_DATA[1] = 0x14;
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x3E;
			CELL_DATA[4] = 0x08;
			CELL_DATA[5] = 0x14;
			break;
#ENDIF

#IF FONT_2B
		case 0x2B :
			CELL_DATA[1] = 0x08;
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x3E;
			CELL_DATA[4] = 0x08;
			CELL_DATA[5] = 0x08;
			break;
#ENDIF

#IF FONT_2C
		case 0x2C :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x50;
			CELL_DATA[3] = 0x30;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_2D
		case 0x2D :
			CELL_DATA[1] = 0x08;
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x08;
			CELL_DATA[5] = 0x08;
			break;
#ENDIF

#IF FONT_2E
		case 0x2E :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x60;
			CELL_DATA[3] = 0x60;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_2F
		case 0x2F :
			CELL_DATA[1] = 0x20;
			CELL_DATA[2] = 0x10;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x04;
			CELL_DATA[5] = 0x02;
			break;
#ENDIF

#IF FONT_30
		case 0x30 :
			CELL_DATA[1] = 0x3E;
			CELL_DATA[2] = 0x51;
			CELL_DATA[4] = 0x45;
			CELL_DATA[5] = 0x3E;
			break;
#ENDIF

#IF FONT_31
		case 0x31 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x42;
			CELL_DATA[3] = 0x7F;
			break;
#ENDIF

#IF FONT_32
		case 0x32 :
			CELL_DATA[1] = 0x42;
			CELL_DATA[2] = 0x61;
			CELL_DATA[3] = 0x51;
			CELL_DATA[4] = 0x49;
			CELL_DATA[5] = 0x46;
			break;
#ENDIF

#IF FONT_33
		case 0x33 :
			CELL_DATA[1] = 0x21;
			CELL_DATA[3] = 0x45;
			CELL_DATA[4] = 0x4B;
			CELL_DATA[5] = 0x31;
			break;
#ENDIF

#IF FONT_34
		case 0x34 :
			CELL_DATA[1] = 0x18;
			CELL_DATA[2] = 0x14;
			CELL_DATA[3] = 0x12;
			CELL_DATA[4] = 0x7F;
			CELL_DATA[5] = 0x10;
			break;
#ENDIF

#IF FONT_35
		case 0x35 :
			CELL_DATA[1] = 0x27;
			CELL_DATA[2] = 0x45;
			CELL_DATA[3] = 0x45;
			CELL_DATA[4] = 0x45;
			CELL_DATA[5] = 0x39;
			break;
#ENDIF

#IF FONT_36
		case 0x36 :
			CELL_DATA[1] = 0x3C;
			CELL_DATA[2] = 0x4A;
			CELL_DATA[4] = 0x49;
			CELL_DATA[5] = 0x30;
			break;
#ENDIF

#IF FONT_37
		case 0x37 :
			CELL_DATA[1] = 0x01;
			CELL_DATA[2] = 0x71;
			CELL_DATA[3] = 0x09;
			CELL_DATA[4] = 0x05;
			CELL_DATA[5] = 0x03;
			break;
#ENDIF

#IF FONT_38
		case 0x38 :
			CELL_DATA[1] = 0x36;
			CELL_DATA[2] = 0x49;
			CELL_DATA[4] = 0x49;
			CELL_DATA[5] = 0x36;
			break;
#ENDIF

#IF FONT_39
		case 0x39 :
			CELL_DATA[1] = 0x06;
			CELL_DATA[2] = 0x49;
			CELL_DATA[4] = 0x29;
			CELL_DATA[5] = 0x1E;
			break;
#ENDIF

#IF FONT_3A
		case 0x3A :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x36;
			CELL_DATA[3] = 0x36;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_3B
		case 0x3B :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x56;
			CELL_DATA[3] = 0x36;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_3C
		case 0x3C :
			CELL_DATA[1] = 0x08;
			CELL_DATA[2] = 0x14;
			CELL_DATA[3] = 0x22;
			CELL_DATA[4] = 0x41;
			break;
#ENDIF

#IF FONT_3D
		case 0x3D :
			CELL_DATA[1] = 0x14;
			CELL_DATA[2] = 0x14;
			CELL_DATA[3] = 0x14;
			CELL_DATA[4] = 0x14;
			CELL_DATA[5] = 0x14;
			break;
#ENDIF

#IF FONT_3E
		case 0x3E :
			CELL_DATA[1] = 0x00;
			CELL_DATA[3] = 0x22;
			CELL_DATA[4] = 0x14;
			CELL_DATA[5] = 0x08;
			break;
#ENDIF

#IF FONT_3F
		case 0x3F :
			CELL_DATA[1] = 0x02;
			CELL_DATA[2] = 0x01;
			CELL_DATA[3] = 0x51;
			CELL_DATA[4] = 0x09;
			CELL_DATA[5] = 0x06;
			break;
#ENDIF

#IF FONT_40
		case 0x40 :
			CELL_DATA[1] = 0x32;
			CELL_DATA[2] = 0x49;
			CELL_DATA[3] = 0x79;
			CELL_DATA[4] = 0x41;
			CELL_DATA[5] = 0x3E;
			break;
#ENDIF

#IF FONT_41
		case 0x41 :
			CELL_DATA[1] = 0x7E;
			CELL_DATA[2] = 0x11;
			CELL_DATA[3] = 0x11;
			CELL_DATA[4] = 0x11;
			CELL_DATA[5] = 0x7E;
			break;
#ENDIF

#IF FONT_42
		case 0x42 :
			CELL_DATA[2] = 0x49;
			CELL_DATA[4] = 0x49;
			CELL_DATA[5] = 0x36;
			break;
#ENDIF

#IF FONT_43
		case 0x43 :
			CELL_DATA[1] = 0x3E;
			CELL_DATA[3] = 0x41;
			CELL_DATA[4] = 0x41;
			CELL_DATA[5] = 0x22;
			break;
#ENDIF

#IF FONT_44
		case 0x44 :
			CELL_DATA[3] = 0x41;
			CELL_DATA[4] = 0x22;
			CELL_DATA[5] = 0x1C;
			break;
#ENDIF

#IF FONT_45
		case 0x45 :
			CELL_DATA[2] = 0x49;
			CELL_DATA[4] = 0x49;
			CELL_DATA[5] = 0x41;
			break;
#ENDIF

#IF FONT_46
		case 0x46 :
			CELL_DATA[2] = 0x09;
			CELL_DATA[3] = 0x09;
			CELL_DATA[4] = 0x09;
			CELL_DATA[5] = 0x01;
			break;
#ENDIF

#IF FONT_47
		case 0x47 :
			CELL_DATA[1] = 0x3E;
			CELL_DATA[4] = 0x49;
			CELL_DATA[5] = 0x7A;
			break;
#ENDIF

#IF FONT_48
		case 0x48 :
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x08;
			CELL_DATA[5] = 0x7F;
			break;
#ENDIF

#IF FONT_49
		case 0x49 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[3] = 0x7F;
			CELL_DATA[4] = 0x41;
			break;
#ENDIF

#IF FONT_4A
		case 0x4A :
			CELL_DATA[1] = 0x20;
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x41;
			CELL_DATA[4] = 0x3F;
			CELL_DATA[5] = 0x01;
			break;
#ENDIF

#IF FONT_4B
		case 0x4B :
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x14;
			CELL_DATA[4] = 0x22;
			CELL_DATA[5] = 0x41;
			break;
#ENDIF

#IF FONT_4C
		case 0x4C :
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x40;
			CELL_DATA[5] = 0x40;
			break;
#ENDIF

#IF FONT_4D
		case 0x4D :
			CELL_DATA[2] = 0x02;
			CELL_DATA[3] = 0x0C;
			CELL_DATA[4] = 0x02;
			CELL_DATA[5] = 0x7F;
			break;
#ENDIF

#IF FONT_4E
		case 0x4E :
			CELL_DATA[2] = 0x04;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x10;
			CELL_DATA[5] = 0x7F;
			break;
#ENDIF

#IF FONT_4F
		case 0x4F :
			CELL_DATA[1] = 0x3E;
			CELL_DATA[3] = 0x41;
			CELL_DATA[4] = 0x41;
			CELL_DATA[5] = 0x3E;
			break;
#ENDIF

#IF FONT_50
		case 0x50 :
			CELL_DATA[2] = 0x09;
			CELL_DATA[3] = 0x09;
			CELL_DATA[4] = 0x09;
			CELL_DATA[5] = 0x06;
			break;
#ENDIF

#IF FONT_51
		case 0x51 :
			CELL_DATA[1] = 0x3E;
			CELL_DATA[3] = 0x51;
			CELL_DATA[4] = 0x21;
			CELL_DATA[5] = 0x5E;
			break;
#ENDIF

#IF FONT_52
		case 0x52 :
			CELL_DATA[2] = 0x09;
			CELL_DATA[3] = 0x19;
			CELL_DATA[4] = 0x29;
			CELL_DATA[5] = 0x46;
			break;
#ENDIF

#IF FONT_53
		case 0x53 :
			CELL_DATA[1] = 0x46;
			CELL_DATA[2] = 0x49;
			CELL_DATA[4] = 0x49;
			CELL_DATA[5] = 0x31;
			break;
#ENDIF

#IF FONT_54
		case 0x54 :
			CELL_DATA[1] = 0x01;
			CELL_DATA[2] = 0x01;
			CELL_DATA[3] = 0x7F;
			CELL_DATA[4] = 0x01;
			CELL_DATA[5] = 0x01;
			break;
#ENDIF

#IF FONT_55
		case 0x55 :
			CELL_DATA[1] = 0x3F;
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x40;
			CELL_DATA[5] = 0x3F;
			break;
#ENDIF

#IF FONT_56
		case 0x56 :
			CELL_DATA[1] = 0x1F;
			CELL_DATA[2] = 0x20;
			CELL_DATA[3] = 0x40;
			CELL_DATA[4] = 0x20;
			CELL_DATA[5] = 0x1F;
			break;
#ENDIF

#IF FONT_57
		case 0x57 :
			CELL_DATA[1] = 0x3F;
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x38;
			CELL_DATA[5] = 0x3F;
			break;
#ENDIF

#IF FONT_58
		case 0x58 :
			CELL_DATA[1] = 0x63;
			CELL_DATA[2] = 0x14;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x14;
			CELL_DATA[5] = 0x63;
			break;
#ENDIF

#IF FONT_59
		case 0x59 :
			CELL_DATA[1] = 0x07;
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x70;
			CELL_DATA[4] = 0x08;
			CELL_DATA[5] = 0x07;
			break;
#ENDIF

#IF FONT_5A
		case 0x5A :
			CELL_DATA[1] = 0x61;
			CELL_DATA[2] = 0x51;
			CELL_DATA[4] = 0x45;
			CELL_DATA[5] = 0x43;
			break;
#ENDIF

#IF FONT_5B
		case 0x5B :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x7F;
			CELL_DATA[3] = 0x41;
			CELL_DATA[4] = 0x41;
			break;
#ENDIF

#IF FONT_5C
		case 0x5C :
			CELL_DATA[1] = 0x02;
			CELL_DATA[2] = 0x04;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x10;
			CELL_DATA[5] = 0x20;
			break;
#ENDIF

#IF FONT_5D
		case 0x5D :
			CELL_DATA[1] = 0x00;
			CELL_DATA[3] = 0x41;
			CELL_DATA[4] = 0x7F;
			break;
#ENDIF

#IF FONT_5E
		case 0x5E :
			CELL_DATA[1] = 0x04;
			CELL_DATA[2] = 0x02;
			CELL_DATA[3] = 0x01;
			CELL_DATA[4] = 0x02;
			CELL_DATA[5] = 0x04;
			break;
#ENDIF

#IF FONT_5F
		case 0x5F :
			CELL_DATA[1] = 0x40;
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x40;
			CELL_DATA[5] = 0x40;
			break;
#ENDIF

#IF FONT_60
		case 0x60 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x01;
			CELL_DATA[3] = 0x02;
			CELL_DATA[4] = 0x04;
			break;
#ENDIF

#IF FONT_61
		case 0x61 :
			CELL_DATA[1] = 0x20;
			CELL_DATA[2] = 0x54;
			CELL_DATA[3] = 0x54;
			CELL_DATA[4] = 0x54;
			CELL_DATA[5] = 0x78;
			break;
#ENDIF

#IF FONT_62
		case 0x62 :
			CELL_DATA[2] = 0x48;
			CELL_DATA[3] = 0x44;
			CELL_DATA[4] = 0x44;
			CELL_DATA[5] = 0x38;
			break;
#ENDIF

#IF FONT_63
		case 0x63 :
			CELL_DATA[1] = 0x38;
			CELL_DATA[2] = 0x44;
			CELL_DATA[3] = 0x44;
			CELL_DATA[4] = 0x44;
			CELL_DATA[5] = 0x20;
			break;
#ENDIF

#IF FONT_64
		case 0x64 :
			CELL_DATA[1] = 0x38;
			CELL_DATA[2] = 0x44;
			CELL_DATA[3] = 0x44;
			CELL_DATA[4] = 0x48;
			CELL_DATA[5] = 0x7F;
			break;
#ENDIF

#IF FONT_65
		case 0x65 :
			CELL_DATA[1] = 0x38;
			CELL_DATA[2] = 0x54;
			CELL_DATA[3] = 0x54;
			CELL_DATA[4] = 0x54;
			CELL_DATA[5] = 0x18;
			break;
#ENDIF

#IF FONT_66
		case 0x66 :
			CELL_DATA[1] = 0x08;
			CELL_DATA[2] = 0x7E;
			CELL_DATA[3] = 0x09;
			CELL_DATA[4] = 0x01;
			CELL_DATA[5] = 0x02;
			break;
#ENDIF

#IF FONT_67
		case 0x67 :
			CELL_DATA[1] = 0x0C;
			CELL_DATA[2] = 0x52;
			CELL_DATA[3] = 0x52;
			CELL_DATA[4] = 0x52;
			CELL_DATA[5] = 0x3E;
			break;
#ENDIF

#IF FONT_68
		case 0x68 :
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x04;
			CELL_DATA[4] = 0x04;
			CELL_DATA[5] = 0x78;
			break;
#ENDIF

#IF FONT_69
		case 0x69 :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x44;
			CELL_DATA[3] = 0x7D;
			break;
#ENDIF

#IF FONT_6A
		case 0x6A :
			CELL_DATA[1] = 0x20;
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x44;
			CELL_DATA[4] = 0x3D;
			break;
#ENDIF

#IF FONT_6B
		case 0x6B :
			CELL_DATA[2] = 0x10;
			CELL_DATA[3] = 0x28;
			CELL_DATA[4] = 0x44;
			break;
#ENDIF

#IF FONT_6C
		case 0x6C :
			CELL_DATA[1] = 0x00;
			CELL_DATA[3] = 0x7F;
			break;
#ENDIF

#IF FONT_6D
		case 0x6D :
			CELL_DATA[1] = 0x7C;
			CELL_DATA[2] = 0x04;
			CELL_DATA[3] = 0x18;
			CELL_DATA[4] = 0x04;
			CELL_DATA[5] = 0x78;
			break;
#ENDIF

#IF FONT_6E
		case 0x6E :
			CELL_DATA[1] = 0x7C;
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x04;
			CELL_DATA[4] = 0x04;
			CELL_DATA[5] = 0x78;
			break;
#ENDIF

#IF FONT_6F
		case 0x6F :
			CELL_DATA[1] = 0x38;
			CELL_DATA[2] = 0x44;
			CELL_DATA[3] = 0x44;
			CELL_DATA[4] = 0x44;
			CELL_DATA[5] = 0x38;
			break;
#ENDIF

#IF FONT_70
		case 0x70 :
			CELL_DATA[1] = 0x7C;
			CELL_DATA[2] = 0x14;
			CELL_DATA[3] = 0x14;
			CELL_DATA[4] = 0x14;
			CELL_DATA[5] = 0x08;
			break;
#ENDIF

#IF FONT_71
		case 0x71 :
			CELL_DATA[1] = 0x08;
			CELL_DATA[2] = 0x14;
			CELL_DATA[3] = 0x14;
			CELL_DATA[4] = 0x18;
			CELL_DATA[5] = 0x7C;
			break;
#ENDIF

#IF FONT_72
		case 0x72 :
			CELL_DATA[1] = 0x7C;
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x04;
			CELL_DATA[4] = 0x04;
			CELL_DATA[5] = 0x08;
			break;
#ENDIF

#IF FONT_73
		case 0x73 :
			CELL_DATA[1] = 0x48;
			CELL_DATA[2] = 0x54;
			CELL_DATA[3] = 0x54;
			CELL_DATA[4] = 0x54;
			CELL_DATA[5] = 0x20;
			break;
#ENDIF

#IF FONT_74
		case 0x74 :
			CELL_DATA[1] = 0x04;
			CELL_DATA[2] = 0x3F;
			CELL_DATA[3] = 0x44;
			CELL_DATA[5] = 0x20;
			break;
#ENDIF

#IF FONT_75
		case 0x75 :
			CELL_DATA[1] = 0x3C;
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x40;
			CELL_DATA[4] = 0x20;
			CELL_DATA[5] = 0x7C;
			break;
#ENDIF

#IF FONT_76
		case 0x76 :
			CELL_DATA[1] = 0x1C;
			CELL_DATA[2] = 0x20;
			CELL_DATA[3] = 0x40;
			CELL_DATA[4] = 0x20;
			CELL_DATA[5] = 0x1C;
			break;
#ENDIF

#IF FONT_77
		case 0x77 :
			CELL_DATA[1] = 0x3C;
			CELL_DATA[2] = 0x40;
			CELL_DATA[3] = 0x30;
			CELL_DATA[5] = 0x3C;
			break;
#ENDIF

#IF FONT_78
		case 0x78 :
			CELL_DATA[1] = 0x44;
			CELL_DATA[2] = 0x28;
			CELL_DATA[3] = 0x10;
			CELL_DATA[4] = 0x28;
			CELL_DATA[5] = 0x44;
			break;
#ENDIF

#IF FONT_79
		case 0x79 :
			CELL_DATA[1] = 0x0C;
			CELL_DATA[2] = 0x50;
			CELL_DATA[3] = 0x50;
			CELL_DATA[4] = 0x50;
			CELL_DATA[5] = 0x3C;
			break;
#ENDIF

#IF FONT_7A
		case 0x7A :
			CELL_DATA[1] = 0x44;
			CELL_DATA[2] = 0x64;
			CELL_DATA[3] = 0x54;
			CELL_DATA[4] = 0x4C;
			CELL_DATA[5] = 0x44;
			break;
#ENDIF

#IF FONT_7B
		case 0x7B :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x08;
			CELL_DATA[3] = 0x36;
			CELL_DATA[4] = 0x41;
			break;
#ENDIF

#IF FONT_7C
		case 0x7C :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x00;
			CELL_DATA[3] = 0x7F;
			CELL_DATA[4] = 0x00;
			break;
#ENDIF

#IF FONT_7D
		case 0x7D :
			CELL_DATA[1] = 0x00;
			CELL_DATA[3] = 0x36;
			CELL_DATA[4] = 0x08;
			break;
#ENDIF

#IF FONT_7E
		case 0x7E :
			CELL_DATA[1] = 0x08;
			CELL_DATA[2] = 0x04;
			CELL_DATA[3] = 0x08;
			CELL_DATA[4] = 0x10;
			CELL_DATA[5] = 0x08;
			break;
#ENDIF

#IF FONT_80
		case 0x80 :
			CELL_DATA[1] = 0x06;
			CELL_DATA[2] = 0x09;
			CELL_DATA[3] = 0x09;
			CELL_DATA[4] = 0x06;
			break;
#ENDIF
		default :
			CELL_DATA[1] = 0x00;
			CELL_DATA[2] = 0x00;
			CELL_DATA[3] = 0x00;
			CELL_DATA[4] = 0x00;
			CELL_DATA[5] = 0x00;
			break;
	}

	CELL_SOURCE = CELL_DATA;
	OLED_WriteCell();
}
#ENDIF

#IF OLED_NEEDS_BITMAP
// Draw one opaque 8x8 tile at the current logical cell; no cursor advance.
void	drawBitmap (void)
{
	if ((CHAR_COLUMN < TEXT_COLUMNS) && (CHAR_PAGE < TEXT_ROWS))
	{
		CELL_SOURCE = BITMAP_POINT;
		OLED_WriteCell();
	}
}

#ENDIF
// 8x8 cell text API. Cursor inputs use logical cell indexes.
#IF OLED_USE_CURSOR
BYTE CURSOR_COLUMN; // 0 .. TEXT_COLUMNS-1
BYTE CURSOR_PAGE;   // 0 .. TEXT_ROWS-1
#ENDIF
#IF OLED_USE_DRAWSTRING
WORD STRING_POINT; // RAM BYTE-array address, not a ROM/string-literal pointer
BYTE STRING_LENGTH; // Bytes remaining; caller must provide a valid buffer
#ENDIF

#IF OLED_USE_CURSOR
void	setCursor (void)
{
	CHAR_COLUMN = CURSOR_COLUMN;
	CHAR_PAGE = CURSOR_PAGE;
}
#ENDIF

#IF OLED_USE_DRAWSTRING
void	drawString (void)
{
	while (STRING_LENGTH != 0)
	{
		// Stop before reading outside the drawable row. No automatic wrap.
		if (CHAR_COLUMN >= TEXT_COLUMNS) break;
		if (CHAR_PAGE >= TEXT_ROWS) break;
		A = *STRING_POINT;
		drawChar();
		STRING_POINT ++;
		STRING_LENGTH = STRING_LENGTH - 1;
		CHAR_COLUMN = CHAR_COLUMN + 1;
	}
}

#ENDIF
#IF OLED_USE_DRAWNUMBER
// Unsigned BYTE decimal output at the current page-aligned cursor.
// NUMBER_VALUE is retained; CHAR_COLUMN advances one cell per drawn digit.
BYTE NUMBER_VALUE;

void	drawNumber (void)
{
	BYTE remainder, hundreds, tens, digits;
	remainder = NUMBER_VALUE;
	hundreds = 0;
	tens = 0;

	while (remainder >= 100)
	{
		remainder = remainder - 100;
		hundreds = hundreds + 1;
	}
	while (remainder >= 10)
	{
		remainder = remainder - 10;
		tens = tens + 1;
	}

	// Select decimal width without leading zeroes; zero still has one digit.
	digits = 1;
	if (tens != 0) digits = 2;
	if (hundreds != 0) digits = 3;

	while (digits != 0)
	{
		// Match drawString: stop at row edge before starting a glyph.
		if (CHAR_COLUMN >= TEXT_COLUMNS) break;
		if (CHAR_PAGE >= TEXT_ROWS) break;
		switch (digits)
		{
			case 3 :
				A = hundreds + 0x30;
				break;
			case 2 :
				A = tens + 0x30;
				break;
			default :
				A = remainder + 0x30;
				break;
		}
		drawChar();
		CHAR_COLUMN = CHAR_COLUMN + 1;
		digits = digits - 1;
	}
}

#ENDIF
// SECTION 8: OLED initialization and original post-init delay
// Uses A and I2C_Address through the existing command routines.
// MCU clock and initial pin setup remain in FPPA0.
void	OLED_Init (void)
{
	//---Init OLED---

	//Set Display Off
	A	=	0xAE;
	I2C_Write_Command1();

	//Set Osc Frequency
	I2C_Address	=	0xD5;
	A	=	0x80;
	I2C_Write_Command2();


	//Set MUX Ratio
	I2C_Address	=	0xA8;
	A	=	0x1F;
	I2C_Write_Command2();


	//Set Display Offset
	I2C_Address	=	0xD3;
	A	=	0x00;
	I2C_Write_Command2();

	//Set Start Line
	A	=	0x40;
	I2C_Write_Command1();

	//Enable Charge Pump Regulator
	I2C_Address	=	0x8D;
	A	=	0x14;
	I2C_Write_Command2();

	//Segment Re-map
	A	=	0xA0;
	I2C_Write_Command1();

	//Scan Direction
	A	=	0xC0;
	I2C_Write_Command1();	

	//Set COM Pins Hardware Configuration
	I2C_Address	=	0xDA;
	A	=	0x12;
	I2C_Write_Command2();

	//Set Contrast
	I2C_Address	=	0x81;
	A	=	0x18;
	I2C_Write_Command2();

	//Set Pre-charge Period
	I2C_Address	=	0xD9;
	A	=	0xF1;
	I2C_Write_Command2();

	//Set VCOMH Deselect Level
	I2C_Address	=	0xDB;
	A	=	0x40;
	I2C_Write_Command2();


	//Disable Entire Display On
	A	=	0xA4; //Output RAM contents	
	I2C_Write_Command1();

	//Set Vertical Addressing Mode
	I2C_Address	=	0x20;
	A	=	0x01;
	I2C_Write_Command2();


	//Display On
	A	=	0xAF;
	I2C_Write_Command1();


	.delay 400000; //This is to solve bug

}

// SECTION 9: Independent optional shape routines in the existing RAM tile.
#IF OLED_USE_TILE_SHAPES
BYTE SHAPE_X0, SHAPE_Y0, SHAPE_X1, SHAPE_Y1; // Inclusive local pixels 0..7
BYTE TILE_X, TILE_Y; // Internal scratch
#ENDIF

#IF OLED_NEEDS_CLEARTILE
void clearTile (void)
{
	CELL_DATA[0] = 0;
	CELL_DATA[1] = 0;
	CELL_DATA[2] = 0;
	CELL_DATA[3] = 0;
	CELL_DATA[4] = 0;
	CELL_DATA[5] = 0;
	CELL_DATA[6] = 0;
	CELL_DATA[7] = 0;
}
#ENDIF

#IF OLED_USE_TILE_SHAPES
static void Tile_SetPixel (void)
{
	BYTE count, mask;
	WORD Point;
	mask = 1;
	count = TILE_Y;
	while (count != 0)
	{
		sl mask;
		count = count - 1;
	}
	Point = CELL_DATA + TILE_X;
	A = *Point;
	A = A | mask;
	*Point = A;
}
#ENDIF

#IF OLED_USE_DRAWLINE
void drawLine (void)
{
	BYTE dx, dy, error, remaining;
	if ((SHAPE_X0 < 8) && (SHAPE_Y0 < 8) && (SHAPE_X1 < 8) && (SHAPE_Y1 < 8))
	{
		TILE_X = SHAPE_X0;
		TILE_Y = SHAPE_Y0;
		if (SHAPE_X1 >= SHAPE_X0) dx = SHAPE_X1 - SHAPE_X0;
		else dx = SHAPE_X0 - SHAPE_X1;
		if (SHAPE_Y1 >= SHAPE_Y0) dy = SHAPE_Y1 - SHAPE_Y0;
		else dy = SHAPE_Y0 - SHAPE_Y1;
		if (dx >= dy)
		{
			error = dx;
			sr error;
			remaining = dx + 1;
			while (remaining != 0)
			{
				Tile_SetPixel();
				remaining = remaining - 1;
				if (remaining == 0) break;
				if (TILE_X < SHAPE_X1) TILE_X = TILE_X + 1;
				else TILE_X = TILE_X - 1;
				error = error + dy;
				if (error >= dx)
				{
					error = error - dx;
					if (TILE_Y < SHAPE_Y1) TILE_Y = TILE_Y + 1;
					else TILE_Y = TILE_Y - 1;
				}
			}
		}
		else
		{
			error = dy;
			sr error;
			remaining = dy + 1;
			while (remaining != 0)
			{
				Tile_SetPixel();
				remaining = remaining - 1;
				if (remaining == 0) break;
				if (TILE_Y < SHAPE_Y1) TILE_Y = TILE_Y + 1;
				else TILE_Y = TILE_Y - 1;
				error = error + dx;
				if (error >= dy)
				{
					error = error - dy;
					if (TILE_X < SHAPE_X1) TILE_X = TILE_X + 1;
					else TILE_X = TILE_X - 1;
				}
			}
		}
	}
}
#ENDIF

#IF OLED_USE_DRAWRECTANGLE
void drawRectangle (void)
{
	if ((SHAPE_X0 < 8) && (SHAPE_Y0 < 8) && (SHAPE_X1 < 8) && (SHAPE_Y1 < 8))
	{
		if ((SHAPE_X0 <= SHAPE_X1) && (SHAPE_Y0 <= SHAPE_Y1))
		{
			TILE_X = SHAPE_X0;
			while (TILE_X <= SHAPE_X1)
			{
				TILE_Y = SHAPE_Y0;
				Tile_SetPixel();
				TILE_Y = SHAPE_Y1;
				Tile_SetPixel();
				TILE_X = TILE_X + 1;
			}
			TILE_Y = SHAPE_Y0;
			while (TILE_Y <= SHAPE_Y1)
			{
				TILE_X = SHAPE_X0;
				Tile_SetPixel();
				TILE_X = SHAPE_X1;
				Tile_SetPixel();
				TILE_Y = TILE_Y + 1;
			}
		}
	}
}
#ENDIF

#IF OLED_USE_FILLRECTANGLE
void fillRectangle (void)
{
	if ((SHAPE_X0 < 8) && (SHAPE_Y0 < 8) && (SHAPE_X1 < 8) && (SHAPE_Y1 < 8))
	{
		if ((SHAPE_X0 <= SHAPE_X1) && (SHAPE_Y0 <= SHAPE_Y1))
		{
			TILE_X = SHAPE_X0;
			while (TILE_X <= SHAPE_X1)
			{
				TILE_Y = SHAPE_Y0;
				while (TILE_Y <= SHAPE_Y1)
				{
					Tile_SetPixel();
					TILE_Y = TILE_Y + 1;
				}
				TILE_X = TILE_X + 1;
			}
		}
	}
}
#ENDIF

// SECTION 10: Direct bar APIs with one explicit shared scratch set.
#IF OLED_USE_LEVELBAR
BYTE BAR_LEVEL; // 0..4 retained.
#ENDIF
#IF OLED_USE_BAR_APIS
BYTE BAR_LENGTH; // Logical cell count, retained.
#ENDIF
#IF OLED_USE_PERCENTBAR
BYTE BAR_PERCENT; // 0..100 retained.
#ENDIF
// Private scratch: shared by both direct routines, not application inputs.
// Sequential use only; no extra pixel buffer and no mode selector.
#IF OLED_USE_BAR_APIS
BYTE BAR_RIGHT, BAR_FILL, BAR_STEP, BAR_X, BAR_COLUMN, BAR_BITS;
WORD BAR_POINT;
#ENDIF
#IF OLED_USE_LEVELBAR
void drawLevelBar (void)
{
	// Validate before any tile/pointer/display/cursor writes. Avoid byte overflow.
	if ((BAR_LEVEL < 5) && (BAR_LENGTH != 0))
	{
		if ((CHAR_COLUMN < TEXT_COLUMNS) && (CHAR_PAGE < TEXT_ROWS))
		{
			BAR_COLUMN = TEXT_COLUMNS - CHAR_COLUMN;
			if (BAR_LENGTH <= BAR_COLUMN)
			{
				BAR_RIGHT = BAR_LENGTH;
				sl BAR_RIGHT;
				sl BAR_RIGHT;
				sl BAR_RIGHT;
				BAR_RIGHT = BAR_RIGHT - 2;
				// Interior width = 8*length-4; one quarter = 2*length-1.
				BAR_STEP = BAR_LENGTH + BAR_LENGTH;
				BAR_STEP = BAR_STEP - 1;
				BAR_FILL = 2;
				BAR_BITS = BAR_LEVEL;
				while (BAR_BITS != 0)
				{
					BAR_FILL = BAR_FILL + BAR_STEP;
					BAR_BITS = BAR_BITS - 1;
				}
				BAR_X = 0;
				// Each tile is generated, written, then reused; no second buffer.
				do
				{
					clearTile();
					BAR_POINT = CELL_DATA;
					BAR_COLUMN = 0;
					do
					{
						BAR_BITS = 0;
						if ((BAR_X != 0) && (BAR_X <= BAR_RIGHT))
						{
							BAR_BITS = 0x81;
							if ((BAR_X == 1) || (BAR_X == BAR_RIGHT) || (BAR_X < BAR_FILL)) BAR_BITS = 0xFF;
						}
						if (BAR_BITS != 0)
						{
							A = BAR_BITS;
							*BAR_POINT = A;
						}
						BAR_POINT++;
						BAR_X = BAR_X + 1;
						BAR_COLUMN = BAR_COLUMN + 1;
					} while (BAR_COLUMN < 8);
					BITMAP_POINT = CELL_DATA;
					drawBitmap();
					CHAR_COLUMN = CHAR_COLUMN + 1;
				} while (BAR_X <= BAR_RIGHT);
				CHAR_COLUMN = CHAR_COLUMN - BAR_LENGTH;
			}
		}
	}
}

#ENDIF

#IF OLED_USE_PERCENTBAR
void drawPercentBar (void)
{
	// Validate before any tile/pointer/display/cursor writes. Avoid byte overflow.
	if ((BAR_PERCENT < 101) && (BAR_LENGTH != 0))
	{
		if ((CHAR_COLUMN < TEXT_COLUMNS) && (CHAR_PAGE < TEXT_ROWS))
		{
			BAR_COLUMN = TEXT_COLUMNS - CHAR_COLUMN;
			if (BAR_LENGTH <= BAR_COLUMN)
			{
				BAR_RIGHT = BAR_LENGTH;
				sl BAR_RIGHT;
				sl BAR_RIGHT;
				sl BAR_RIGHT;
				BAR_RIGHT = BAR_RIGHT - 2;
				// floor(interior width * percent / 100), bounded BYTE additions.
				BAR_STEP = BAR_RIGHT - 2;
				BAR_FILL = 2;
				BAR_X = 0;
				BAR_BITS = BAR_PERCENT;
				while (BAR_BITS != 0)
				{
					BAR_X = BAR_X + BAR_STEP;
					if (BAR_X >= 100)
					{
						BAR_X = BAR_X - 100;
						BAR_FILL = BAR_FILL + 1;
					}
					BAR_BITS = BAR_BITS - 1;
				}
				BAR_X = 0;
				// Each tile is generated, written, then reused; no second buffer.
				do
				{
					clearTile();
					BAR_POINT = CELL_DATA;
					BAR_COLUMN = 0;
					do
					{
						BAR_BITS = 0;
						if ((BAR_X != 0) && (BAR_X <= BAR_RIGHT))
						{
							BAR_BITS = 0x81;
							if ((BAR_X == 1) || (BAR_X == BAR_RIGHT) || (BAR_X < BAR_FILL)) BAR_BITS = 0xFF;
						}
						if (BAR_BITS != 0)
						{
							A = BAR_BITS;
							*BAR_POINT = A;
						}
						BAR_POINT++;
						BAR_X = BAR_X + 1;
						BAR_COLUMN = BAR_COLUMN + 1;
					} while (BAR_COLUMN < 8);
					BITMAP_POINT = CELL_DATA;
					drawBitmap();
					CHAR_COLUMN = CHAR_COLUMN + 1;
				} while (BAR_X <= BAR_RIGHT);
				CHAR_COLUMN = CHAR_COLUMN - BAR_LENGTH;
			}
		}
	}
}

#ENDIF

// Optional integrated cell operations and multi-cell frame.
#IF OLED_USE_CLEARCELL
void clearCell (void)
{
	CELL_DATA[0] = 0x00;
	CELL_DATA[1] = 0x00;
	CELL_DATA[2] = 0x00;
	CELL_DATA[3] = 0x00;
	CELL_DATA[4] = 0x00;
	CELL_DATA[5] = 0x00;
	CELL_DATA[6] = 0x00;
	CELL_DATA[7] = 0x00;
	BITMAP_POINT = CELL_DATA;
	drawBitmap();
}
#ENDIF

#IF OLED_USE_FILLCELL
void fillCell (void)
{
	CELL_DATA[0] = 0xFF;
	CELL_DATA[1] = 0xFF;
	CELL_DATA[2] = 0xFF;
	CELL_DATA[3] = 0xFF;
	CELL_DATA[4] = 0xFF;
	CELL_DATA[5] = 0xFF;
	CELL_DATA[6] = 0xFF;
	CELL_DATA[7] = 0xFF;
	BITMAP_POINT = CELL_DATA;
	drawBitmap();
}
#ENDIF

#IF OLED_USE_INVERTTILE
void invertTile (void)
{
	A = CELL_DATA[0];
	A = A ^ 0xFF;
	CELL_DATA[0] = A;
	A = CELL_DATA[1];
	A = A ^ 0xFF;
	CELL_DATA[1] = A;
	A = CELL_DATA[2];
	A = A ^ 0xFF;
	CELL_DATA[2] = A;
	A = CELL_DATA[3];
	A = A ^ 0xFF;
	CELL_DATA[3] = A;
	A = CELL_DATA[4];
	A = A ^ 0xFF;
	CELL_DATA[4] = A;
	A = CELL_DATA[5];
	A = A ^ 0xFF;
	CELL_DATA[5] = A;
	A = CELL_DATA[6];
	A = A ^ 0xFF;
	CELL_DATA[6] = A;
	A = CELL_DATA[7];
	A = A ^ 0xFF;
	CELL_DATA[7] = A;
}
#ENDIF

#IF OLED_USE_DRAWFRAME
BYTE FRAME_WIDTH; // Logical cells, retained.
BYTE FRAME_HEIGHT; // Logical cells, retained.
void drawFrame (void)
{
	BYTE startX, startY, endX, endY, column, bits;
	WORD Point;
	if ((FRAME_WIDTH != 0) && (FRAME_HEIGHT != 0))
	{
		if ((CHAR_COLUMN < TEXT_COLUMNS) && (CHAR_PAGE < TEXT_ROWS))
		{
			endX = TEXT_COLUMNS - CHAR_COLUMN;
			endY = TEXT_ROWS - CHAR_PAGE;
			if ((FRAME_WIDTH <= endX) && (FRAME_HEIGHT <= endY))
			{
				startX = CHAR_COLUMN;
				startY = CHAR_PAGE;
				endX = startX + FRAME_WIDTH;
				endX = endX - 1;
				endY = startY + FRAME_HEIGHT;
				endY = endY - 1;
				do
				{
					CHAR_COLUMN = startX;
					do
					{
						if ((CHAR_COLUMN == startX) || (CHAR_COLUMN == endX) || (CHAR_PAGE == startY) || (CHAR_PAGE == endY))
						{
							bits = 0;
							if (CHAR_PAGE == startY) bits = bits | 0x01;
							if (CHAR_PAGE == endY) bits = bits | 0x80;
							Point = CELL_DATA;
							column = 0;
							do
							{
								A = bits;
								*Point = A;
								Point++;
								column = column + 1;
							} while (column < 8);
							if (CHAR_COLUMN == startX) CELL_DATA[0] = 0xFF;
							if (CHAR_COLUMN == endX) CELL_DATA[7] = 0xFF;
							BITMAP_POINT = CELL_DATA;
							drawBitmap();
						}
						CHAR_COLUMN = CHAR_COLUMN + 1;
					} while (CHAR_COLUMN <= endX);
					CHAR_PAGE = CHAR_PAGE + 1;
				} while (CHAR_PAGE <= endY);
				CHAR_COLUMN = startX;
				CHAR_PAGE = startY;
			}
		}
	}
}
#ENDIF

// Optional single converter-generated 32x32 image, one tile at a time.
#IF OLED_USE_DRAWIMAGE
BYTE IMAGE_TILE; // Row-major 0..15 input to the generated loader.
#include "bitmap_image.h"

// Full-scene draw: clears display first; resets active/staging cursor to 0/0.
void drawImage (void)
{
	ClearScreen();
	IMAGE_TILE = 0;
	CURSOR_PAGE = 0;
	while (CURSOR_PAGE < 4)
	{
		CURSOR_COLUMN = 0;
		while (CURSOR_COLUMN < 4)
		{
			Image_LoadTile();
			setCursor();
			if (ROTATION.0) CHAR_PAGE = CHAR_PAGE + 1;
			else CHAR_COLUMN = CHAR_COLUMN + 1;
			BITMAP_POINT = CELL_DATA;
			drawBitmap();
			IMAGE_TILE = IMAGE_TILE + 1;
			CURSOR_COLUMN = CURSOR_COLUMN + 1;
		}
		CURSOR_PAGE = CURSOR_PAGE + 1;
	}
	CURSOR_COLUMN = 0;
	CURSOR_PAGE = 0;
	setCursor();
}
#ENDIF

