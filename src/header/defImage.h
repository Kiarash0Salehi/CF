#pragma once

#ifndef _DEF_IMAGE_H_
#define _DEF_IMAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct
{
	int width;
	int height;
	unsigned int* memory;
} Image;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _DEF_IMAGE_H_