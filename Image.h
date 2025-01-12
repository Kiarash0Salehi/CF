#pragma once
#include "defImage.h"

#ifndef _IMAGE_H_
#define _IMAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

Image readBMPFromFile(const char* filePath);

#ifdef __cplusplus
}
#endif

#endif // _IMAGE_H_



