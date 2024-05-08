//
// Created by X on 2023/5/14.
//

#ifndef XSTEP_CAN_BASE_H
#define XSTEP_CAN_BASE_H

#include "main.h"
#include "can.h"

#define CAN_ID_SELF 0x01


extern CAN_TxHeaderTypeDef txHeader;



#ifdef __cplusplus
extern "C" {
#endif

    void CANInit(CAN_HandleTypeDef* hcan);
    void CAN_Send(CAN_TxHeaderTypeDef* pHeader, uint8_t* _data);

#ifdef __cplusplus
}
#endif


#endif //XSTEP_CAN_BASE_H
