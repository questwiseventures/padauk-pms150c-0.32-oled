// Logo pixels are encoded as Mini-C immediate assignments, like the font.
// Only one eight-byte tile is present in RAM. IMAGE_TILE is row-major 0..15.
static void Cube_LoadTile (void)
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
			CELL_DATA[6] = 0xF0;
			CELL_DATA[7] = 0xF8;
			break;
		case 1 :
			CELL_DATA[0] = 0xF0;
			CELL_DATA[1] = 0x70;
			CELL_DATA[2] = 0x18;
			CELL_DATA[3] = 0x18;
			CELL_DATA[4] = 0x0C;
			CELL_DATA[5] = 0x0E;
			CELL_DATA[6] = 0x06;
			CELL_DATA[7] = 0x06;
			break;
		case 2 :
			CELL_DATA[0] = 0x06;
			CELL_DATA[1] = 0x0C;
			CELL_DATA[2] = 0x0C;
			CELL_DATA[3] = 0x18;
			CELL_DATA[4] = 0x18;
			CELL_DATA[5] = 0x30;
			CELL_DATA[6] = 0x70;
			CELL_DATA[7] = 0x60;
			break;
		case 3 :
			CELL_DATA[0] = 0xC0;
			CELL_DATA[1] = 0xC0;
			CELL_DATA[2] = 0x80;
			break;
		case 4 :
			CELL_DATA[2] = 0xFC;
			CELL_DATA[3] = 0xFE;
			CELL_DATA[4] = 0x06;
			CELL_DATA[5] = 0x06;
			CELL_DATA[6] = 0x0C;
			CELL_DATA[7] = 0x1C;
			break;
		case 5 :
			CELL_DATA[0] = 0x18;
			CELL_DATA[1] = 0x30;
			CELL_DATA[2] = 0x30;
			CELL_DATA[3] = 0x60;
			CELL_DATA[4] = 0x60;
			CELL_DATA[5] = 0xC0;
			CELL_DATA[6] = 0xC0;
			CELL_DATA[7] = 0x80;
			break;
		case 6 :
			CELL_DATA[0] = 0xC0;
			CELL_DATA[1] = 0xC0;
			CELL_DATA[2] = 0x60;
			CELL_DATA[3] = 0x60;
			CELL_DATA[4] = 0x30;
			CELL_DATA[5] = 0x38;
			CELL_DATA[6] = 0x18;
			CELL_DATA[7] = 0x0C;
			break;
		case 7 :
			CELL_DATA[0] = 0x0C;
			CELL_DATA[1] = 0x07;
			CELL_DATA[2] = 0xE7;
			CELL_DATA[3] = 0xF0;
			CELL_DATA[4] = 0xF0;
			CELL_DATA[5] = 0xE0;
			break;
		case 8 :
			CELL_DATA[2] = 0xFF;
			CELL_DATA[3] = 0xFF;
			CELL_DATA[4] = 0x80;
			break;
		case 9 :
			CELL_DATA[5] = 0x80;
			CELL_DATA[6] = 0xFF;
			CELL_DATA[7] = 0xFF;
			break;
		case 10 :
			CELL_DATA[0] = 0x81;
			break;
		case 11 :
			CELL_DATA[2] = 0x80;
			CELL_DATA[3] = 0xFF;
			CELL_DATA[4] = 0x7F;
			break;
		case 12 :
			CELL_DATA[3] = 0x01;
			CELL_DATA[4] = 0x01;
			CELL_DATA[5] = 0x03;
			CELL_DATA[6] = 0x07;
			CELL_DATA[7] = 0x06;
			break;
		case 13 :
			CELL_DATA[0] = 0x0C;
			CELL_DATA[1] = 0x0C;
			CELL_DATA[2] = 0x18;
			CELL_DATA[3] = 0x18;
			CELL_DATA[4] = 0x30;
			CELL_DATA[5] = 0x73;
			CELL_DATA[6] = 0x67;
			CELL_DATA[7] = 0x67;
			break;
		case 14 :
			CELL_DATA[0] = 0x67;
			CELL_DATA[1] = 0x31;
			CELL_DATA[2] = 0x30;
			CELL_DATA[3] = 0x18;
			CELL_DATA[4] = 0x1C;
			CELL_DATA[5] = 0x0C;
			CELL_DATA[6] = 0x0E;
			CELL_DATA[7] = 0x06;
			break;
		case 15 :
			CELL_DATA[0] = 0x03;
			CELL_DATA[1] = 0x03;
			CELL_DATA[2] = 0x01;
			CELL_DATA[3] = 0x01;
			break;
	}
}
