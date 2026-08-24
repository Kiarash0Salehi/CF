#pragma once

#ifndef _RGBCOLOR_H_
#define _RGBCOLOR_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	union
	{
		struct
		{
			unsigned char r;
			unsigned char b;
			unsigned char g;
			unsigned char a;
		} ui8;
		unsigned int ui32;
	} color;
} RGBAcolor;

// uint32_t* color = (uint32_t*)&rgbcolor.color.rgba;

// draw( ... , .. , . , *color);
#ifdef __cplusplus
}
#endif

#endif // _RGBCOLOR_H_