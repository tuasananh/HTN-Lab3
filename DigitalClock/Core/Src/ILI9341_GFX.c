//	MIT License
//
//	Copyright (c) 2017 Matej Artnak
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//
//
//
//-----------------------------------
//	ILI9341 GFX library for STM32
//-----------------------------------
//
//	Very simple GFX library built upon ILI9342_STM32_Driver library.
//	Adds basic shapes, image and font drawing capabilities to ILI9341
//
//	Library is written for STM32 HAL library and supports STM32CUBEMX. To use the library with Cube software
//	you need to tick the box that generates peripheral initialization code in their own respective .c and .h file
//
//
//-----------------------------------
//	How to use this library
//-----------------------------------
//
//	-If using MCUs other than STM32F7 you will have to change the #include "stm32f7xx_hal.h" in the ILI9341_GFX.h to your respective .h file
//
//	If using "ILI9341_STM32_Driver" then all other prequisites to use the library have allready been met
//	Simply include the library and it is ready to be used
//
//---------------------------------------------------------------------------//
//	This file was modified by Cong Thuan Do (HUST) for STM32F429-DISC1 board //
//---------------------------------------------------------------------------//

#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"

void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{
	int x = radius-1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		ILI9341_DrawPixel(X + x, Y + y, color);
		ILI9341_DrawPixel(X + y, Y + x, color);
		ILI9341_DrawPixel(X - y, Y + x, color);
		ILI9341_DrawPixel(X - x, Y + y, color);
		ILI9341_DrawPixel(X - x, Y - y, color);
		ILI9341_DrawPixel(X - y, Y - x, color);
		ILI9341_DrawPixel(X + y, Y - x, color);
		ILI9341_DrawPixel(X + x, Y - y, color);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += (-radius << 1) + dx;
		}
	}
}

void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{

	int x = radius;
	int y = 0;
	int xChange = 1 - (radius << 1);
	int yChange = 0;
	int radiusError = 0;

	while (x >= y)
	{
		for (int i = X - x; i <= X + x; i++)
		{
			ILI9341_DrawPixel(i, Y + y,color);
			ILI9341_DrawPixel(i, Y - y,color);
		}

		for (int i = X - y; i <= X + y; i++)
		{
			ILI9341_DrawPixel(i, Y + x,color);
			ILI9341_DrawPixel(i, Y - x,color);
		}

		y++;
		radiusError += yChange;
		yChange += 2;

		if (((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
	uint16_t xLen = 0;
	uint16_t yLen = 0;
	uint8_t negX = 0;
	uint8_t negY = 0;
	float negCalc = 0;

	negCalc = X1 - X0;
	if(negCalc < 0) negX = 1;
	negCalc = 0;

	negCalc = Y1 - Y0;
	if(negCalc < 0) negY = 1;

	//DRAW HORIZONTAL!
	if(!negX)
	{
		xLen = X1 - X0;
	}
	else
	{
		xLen = X0 - X1;
	}
	ILI9341_DrawHLine(X0, Y0, xLen, color);
	ILI9341_DrawHLine(X0, Y1, xLen, color);

	//DRAW VERTICAL!
	if(!negY)
	{
		yLen = Y1 - Y0;
	}
	else
	{
		yLen = Y0 - Y1;
	}

	ILI9341_DrawVLine(X0, Y0, yLen, color);
	ILI9341_DrawVLine(X1, Y0, yLen, color);

	if((xLen > 0)||(yLen > 0))
	{
		ILI9341_DrawPixel(X1, Y1, color);
	}
}

void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
	uint16_t xLen = 0;
	uint16_t yLen = 0;
	uint8_t negX = 0;
	uint8_t negY = 0;
	int32_t negCalc = 0;
	uint16_t X0True = 0;
	uint16_t Y0True = 0;

	negCalc = X1 - X0;
	if(negCalc < 0) negX = 1;
	negCalc = 0;

	negCalc = Y1 - Y0;
	if(negCalc < 0) negY = 1;

	if(!negX)
	{
		xLen = X1 - X0;
		X0True = X0;
	}
	else
	{
		xLen = X0 - X1;
		X0True = X1;
	}

	if(!negY)
	{
		yLen = Y1 - Y0;
		Y0True = Y0;
	}
	else
	{
		yLen = Y0 - Y1;
		Y0True = Y1;
	}

	ILI9341_DrawRectangle(X0True, Y0True, xLen, yLen, color);
}

static uint8_t ILI9341_FontCharCode(char ch)
{
	uint8_t code = (uint8_t)ch;
	return ((code >= 0x20) && (code <= 0x7F)) ? code : (uint8_t)'?';
}

static uint8_t ILI9341_CharAdvance(char ch, const uint8_t font[])
{
	uint8_t code = ILI9341_FontCharCode(ch);
	uint8_t width = font[4 + (code - 0x20) * font[0]];
	if((code >= '0') && (code <= '9'))
	{
		for(uint8_t digit = '0'; digit <= '9'; digit++)
		{
			uint8_t digitWidth = font[4 + (digit - 0x20) * font[0]];
			if(digitWidth > width) width = digitWidth;
		}
	}
	return (width + 2 < font[1]) ? width + 2 : font[1];
}

uint16_t ILI9341_GetCharAdvance(char ch, const uint8_t font[], uint8_t scale)
{
	return (uint16_t)ILI9341_CharAdvance(ch, font) * scale;
}

void ILI9341_DrawCharScaled(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor, uint8_t scale)
{
	if((scale == 0) || (X >= LCD_WIDTH) || (Y >= LCD_HEIGHT)) return;

	uint8_t fOffset = font[0];
	uint8_t fBPL = font[3];
	uint8_t code = ILI9341_FontCharCode(ch);
	const uint8_t *tempChar = &font[(code - 0x20) * fOffset + 4];
	uint8_t cellWidth = font[1];
	uint8_t glyphWidth = font[1];
	uint8_t leftPadding = 0;
	if((code >= '0') && (code <= '9'))
	{
		cellWidth = ILI9341_CharAdvance(ch, font);
		glyphWidth = tempChar[0];
		if(cellWidth > glyphWidth) leftPadding = (cellWidth - glyphWidth) / 2U;
	}

	uint16_t drawWidth = (uint16_t)cellWidth * scale;
	uint16_t drawHeight = (uint16_t)font[2] * scale;
	if((drawWidth == 0) || (drawHeight == 0)) return;

	if(drawWidth > LCD_WIDTH - X) drawWidth = LCD_WIDTH - X;
	if(drawHeight > LCD_HEIGHT - Y) drawHeight = LCD_HEIGHT - Y;

	uint8_t buffer[BURST_MAX_SIZE];
	uint16_t used = 0;

	ILI9341_SetAddress(X, Y, X + drawWidth - 1, Y + drawHeight - 1);
	for(uint16_t j = 0; j < drawHeight; j++)
	{
		uint16_t row = j / scale;
		uint8_t bit = 1U << (row & 0x07);
		for(uint16_t i = 0; i < drawWidth; i++)
		{
			uint16_t column = i / scale;
			uint16_t pixel = bgcolor;
			if((column >= leftPadding) && (column - leftPadding < glyphWidth))
			{
				uint8_t bits = tempChar[fBPL * (column - leftPadding) + (row >> 3) + 1];
				if(bits & bit) pixel = color;
			}
			buffer[used++] = pixel >> 8;
			buffer[used++] = pixel;
			if(used == sizeof(buffer))
			{
				ILI9341_WriteBuffer(buffer, used);
				used = 0;
			}
		}
	}

	if(used > 0) ILI9341_WriteBuffer(buffer, used);
}

void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	ILI9341_DrawCharScaled(ch, font, X, Y, color, bgcolor, 1);
}

void ILI9341_DrawTextScaled(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor, uint8_t scale)
{
	if(scale == 0) return;

	while (*str && (X < LCD_WIDTH))
	{
		uint8_t code = ILI9341_FontCharCode(*str);
		ILI9341_DrawCharScaled((char)code, font, X, Y, color, bgcolor, scale);

		uint32_t nextX = (uint32_t)X + ILI9341_GetCharAdvance((char)code, font, scale);
		if(nextX >= LCD_WIDTH) break;
		X = (uint16_t)nextX;

		str++;
	}
}

void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	ILI9341_DrawTextScaled(str, font, X, Y, color, bgcolor, 1);
}

void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation)
{
	if(orientation == SCREEN_HORIZONTAL_1)
	{
		ILI9341_SetRotation(SCREEN_HORIZONTAL_1);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_WIDTH,ILI9341_SCREEN_HEIGHT);
	}
	else if(orientation == SCREEN_HORIZONTAL_2)
	{
		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_WIDTH,ILI9341_SCREEN_HEIGHT);
	}
	else if(orientation == SCREEN_VERTICAL_2)
	{
		ILI9341_SetRotation(SCREEN_VERTICAL_2);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_HEIGHT,ILI9341_SCREEN_WIDTH);
	}
	else if(orientation == SCREEN_VERTICAL_1)
	{
		ILI9341_SetRotation(SCREEN_VERTICAL_1);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_HEIGHT,ILI9341_SCREEN_WIDTH);
	}

	uint32_t counter = 0;
	for(uint32_t i = 0; i < ILI9341_SCREEN_WIDTH*ILI9341_SCREEN_HEIGHT*2/BURST_MAX_SIZE; i++)
	{
		ILI9341_WriteBuffer((uint8_t*)(image + counter), BURST_MAX_SIZE);
		counter += BURST_MAX_SIZE;
	}
}
