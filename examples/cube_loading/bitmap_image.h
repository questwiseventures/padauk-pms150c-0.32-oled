// Logo pixels are encoded as Mini-C immediate assignments, like the font.
// Only one eight-byte tile is present in RAM. IMAGE_TILE is row-major 0..15.
static void Image_LoadTile (void)
{
	CELL_DATA[0] = 0x00;
	CELL_DATA[1] = 0x00;
	CELL_DATA[2] = 0x00;
	CELL_DATA[3] = 0x00;
	CELL_DATA[4] = 0x00;
	CELL_DATA[5] = 0x00;
	CELL_DATA[6] = 0x00;
	CELL_DATA[7] = 0x00;
	switch (IMAGE_TILE)
	{
		case 0 :
			break;
		case 1 :
			CELL_DATA[5] = 0xC0;
			CELL_DATA[6] = 0xF8;
			CELL_DATA[7] = 0x9C;
			break;
		case 2 :
			CELL_DATA[0] = 0x8C;
			CELL_DATA[1] = 0x8C;
			CELL_DATA[2] = 0x1C;
			CELL_DATA[3] = 0x18;
			CELL_DATA[4] = 0x38;
			CELL_DATA[5] = 0x30;
			CELL_DATA[6] = 0x70;
			CELL_DATA[7] = 0xE0;
			break;
		case 3 :
			CELL_DATA[0] = 0xC0;
			CELL_DATA[1] = 0x80;
			break;
		case 4 :
			CELL_DATA[4] = 0x80;
			CELL_DATA[5] = 0x80;
			CELL_DATA[6] = 0xC0;
			CELL_DATA[7] = 0x70;
			break;
		case 5 :
			CELL_DATA[0] = 0x10;
			CELL_DATA[1] = 0x0C;
			CELL_DATA[2] = 0x06;
			CELL_DATA[3] = 0x02;
			CELL_DATA[4] = 0x03;
			CELL_DATA[5] = 0x19;
			CELL_DATA[6] = 0x18;
			CELL_DATA[7] = 0x10;
			break;
		case 6 :
			CELL_DATA[0] = 0x1C;
			CELL_DATA[1] = 0x18;
			CELL_DATA[2] = 0x49;
			CELL_DATA[3] = 0xE1;
			CELL_DATA[4] = 0xC2;
			CELL_DATA[5] = 0x62;
			CELL_DATA[6] = 0x44;
			CELL_DATA[7] = 0x1C;
			break;
		case 7 :
			CELL_DATA[0] = 0x71;
			CELL_DATA[1] = 0xE3;
			CELL_DATA[2] = 0xE7;
			CELL_DATA[3] = 0x7E;
			CELL_DATA[4] = 0xFC;
			CELL_DATA[5] = 0xF8;
			CELL_DATA[6] = 0xC0;
			break;
		case 8 :
			CELL_DATA[1] = 0x38;
			CELL_DATA[2] = 0xCC;
			CELL_DATA[3] = 0x07;
			CELL_DATA[4] = 0x81;
			CELL_DATA[5] = 0x40;
			CELL_DATA[6] = 0x20;
			CELL_DATA[7] = 0x10;
			break;
		case 9 :
			CELL_DATA[0] = 0x1A;
			CELL_DATA[1] = 0x1F;
			CELL_DATA[2] = 0x1B;
			CELL_DATA[3] = 0x19;
			CELL_DATA[4] = 0x78;
			CELL_DATA[5] = 0xC0;
			CELL_DATA[6] = 0x78;
			CELL_DATA[7] = 0x1C;
			break;
		case 10 :
			CELL_DATA[0] = 0x1C;
			CELL_DATA[1] = 0x0E;
			CELL_DATA[2] = 0x0E;
			CELL_DATA[3] = 0x0E;
			CELL_DATA[4] = 0x0E;
			CELL_DATA[5] = 0x0E;
			CELL_DATA[6] = 0x0E;
			CELL_DATA[7] = 0x08;
			break;
		case 11 :
			CELL_DATA[0] = 0x08;
			CELL_DATA[1] = 0x0D;
			CELL_DATA[2] = 0x04;
			CELL_DATA[3] = 0x06;
			CELL_DATA[4] = 0x06;
			CELL_DATA[5] = 0x07;
			CELL_DATA[6] = 0x07;
			break;
		case 12 :
			CELL_DATA[2] = 0x19;
			CELL_DATA[3] = 0x10;
			CELL_DATA[4] = 0x0B;
			break;
		case 13 :
			break;
		case 14 :
			break;
		case 15 :
			break;
	}
}
