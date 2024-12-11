#include "defImage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

Image readBMPFromFile(const char* filePath)
{
	Image image;
	long lSize;
	uint8_t* buffer;

	memset(&image, 0, sizeof(Image));
	FILE* f = fopen(filePath, "rb");
	if (f == 0) 
	{
		puts("failed to open file");
		exit(0x00000f);
	}

	fseek(f, 0, SEEK_END);
	lSize = ftell(f);
	rewind(f);

	buffer = (uint8_t*)malloc((size_t)lSize);
	if (buffer == 0)
	{
		puts("failed to alloc image buffer");
		exit(0x0000f0);
	}

	if (fread(buffer, 1, (size_t)lSize, f))
	{
		puts("failed to read image file");
	}

	if (buffer[0] != 'B' && buffer[1] != 'M')
	{
		puts("wrong format");
		exit(0);
	}

	uint64_t filesize = (uint64_t)buffer[2];
	uint32_t offset = (uint32_t)buffer[10];
	uint32_t width = (uint32_t)buffer[18];
	uint32_t height = (uint32_t)buffer[22];
	uint16_t bits = (uint16_t)buffer[28];

	image.height = height;
	image.width = width;
	image.memory = (uint8_t*)malloc(width * height * 4);
	
	return image;
	fclose(f);
}