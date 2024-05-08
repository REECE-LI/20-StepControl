
#ifndef _SELFMAIN_H
#define _SELFMAIN_H

#include "main.h"

#include "stdint-gcc.h"

#include "tim.h"


#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------- C Scope ---------------------------*/



void SelfMain();
void OnCanCmd(uint8_t _cmd, uint8_t *_data, uint32_t _len);


#ifdef __cplusplus
}
/*---------------------------- C++ Scope ---------------------------*/
#include "motor.h"
extern Motor motor;

#endif


#endif //C8SCREEN_SELFMAIN_H