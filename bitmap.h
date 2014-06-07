/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Copyright (C) 2014 Marzo Sette Torres Junior <marzojr@dcc.ufmg.br>
 *
 * TP is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TP is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _BITMAP_H_
#define _BITMAP_H_

// Estruturas para manipulação de bitmaps. Fonte: documentação de bitmaps:
// http://msdn.microsoft.com/en-us/library/windows/desktop/dd183377(v=vs.85).aspx
// Nada muito interessante aqui...

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef unsigned int   FXPT2DOT30;

#pragma pack(push,1)
struct RGBQUAD {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
};

struct BITMAPFILEHEADER {
	WORD  bfType;			// 'BM'
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
};

struct CIEXYZ {
	FXPT2DOT30 ciexyzX;
	FXPT2DOT30 ciexyzY;
	FXPT2DOT30 ciexyzZ;
};

struct CIEXYZTRIPLE {
	CIEXYZ ciexyzRed;
	CIEXYZ ciexyzGreen;
	CIEXYZ ciexyzBlue;
};

#  define BI_RGB       0

struct BITMAPINFOHEADER {
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
	DWORD bV5RedMask;
	DWORD bV5GreenMask;
	DWORD bV5BlueMask;
	DWORD bV5AlphaMask;
	DWORD bV5CSType;
	CIEXYZTRIPLE bV5Endpoints;
	DWORD bV5GammaRed;
	DWORD bV5GammaGreen;
	DWORD bV5GammaBlue;
	DWORD bV5Intent;
	DWORD bV5ProfileData;
	DWORD bV5ProfileSize;
	DWORD bV5Reserved;
};

#pragma pack(pop)

#endif // _BITMAP_H_
