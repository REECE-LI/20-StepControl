//
// Created by X on 2023/5/14.
//

#include "can_base.h"
#include "string.h"

#include "configurations.h"

#include "encoder_calibrator_stm32.h"

#include "selfmain.h"



extern EncoderCalibrator encoderCalibrator;
// 这里使用了一个很巧妙的方法：将StdId对127整除得到ID号，将StdId对127取余得到命令
CAN_TxHeaderTypeDef txHeader = {
        .StdId = CAN_ID_SELF,
//        .ExtId = 0x00,
        .IDE = CAN_ID_STD,
        .RTR = CAN_RTR_DATA,
        .DLC = 8
};

void CANInit(CAN_HandleTypeDef* hcan) {
    CAN_FilterTypeDef  can_filter;

  can_filter.FilterBank = 0;                       // filter 0
  can_filter.FilterMode =  CAN_FILTERMODE_IDMASK;  // mask mode
  can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
  can_filter.FilterIdHigh = 0;
  can_filter.FilterIdLow  = 0;
  can_filter.FilterMaskIdHigh = 0;
  can_filter.FilterMaskIdLow  = 0;                // set mask 0 to receive all can id
  can_filter.FilterFIFOAssignment = CAN_RX_FIFO0; // assign to fifo0
  can_filter.FilterActivation = ENABLE;           // enable can filter
  can_filter.SlaveStartFilterBank  = 14;          // only meaningful in dual can mode

  HAL_CAN_ConfigFilter(hcan, &can_filter);        // init can filter
  HAL_CAN_Start(hcan);                          // start can1
  HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING); // enable can1 rx interrupt
}

uint32_t TxMailbox;

void CAN_Send(CAN_TxHeaderTypeDef* pHeader, uint8_t* _data) {

    if(HAL_CAN_AddTxMessage(&hcan, pHeader, _data, (uint32_t*)&TxMailbox) != HAL_OK)
    {
      Error_Handler();
    }
}



void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    uint8_t RxData[8];
    if (hcan->Instance == CAN) {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, RxData); //receive can data
    }
    uint8_t id = (rx_header.StdId >> 7); // 4Bits ID & 7Bits Msg
    uint8_t cmd = rx_header.StdId & 0x7F; // 4Bits ID & 7Bits Msg
    if (id == 0 || id == boardConfig.canNodeId) {
        OnCanCmd(cmd, RxData, rx_header.DLC);
    }
}


void OnCanCmd(uint8_t _cmd, uint8_t *_data, uint32_t _len) {
#if 1
    float tmpF;
    int32_t tmpI;


    switch (_cmd) {
        // 0x00~0x0F No Memory CMDs
        case 0x01:  // Enable Motor
            motor.controller->requestMode = (*((uint8_t *) (_data) )== 1) ?
                                            Motor::MODE_COMMAND_VELOCITY : Motor::MODE_STOP;
            break;
        case 0x02:  // Do Calibration
            encoderCalibrator.isTriggered = true;
            break;
        case 0x03:  // Set Current SetPoint
            if (motor.controller->modeRunning != Motor::MODE_COMMAND_CURRENT)
                motor.controller->SetCtrlMode(Motor::MODE_COMMAND_CURRENT);
            motor.controller->SetCurrentSetPoint((int32_t) (*(float *) _data * 1000));

            break;
        case 0x04:  // Set Velocity SetPoint
            if (motor.controller->modeRunning != Motor::MODE_COMMAND_VELOCITY) {
                motor.config.motionParams.ratedVelocity = boardConfig.velocityLimit;
                motor.controller->SetCtrlMode(Motor::MODE_COMMAND_VELOCITY);
            }

            motor.controller->SetVelocitySetPoint(
                    (int32_t) (*(float *) _data *
                               (float) motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS));
            break;
        case 0x05:  // Set Position SetPoint
            if (motor.controller->modeRunning != Motor::MODE_COMMAND_POSITION) {
                motor.config.motionParams.ratedVelocity = boardConfig.velocityLimit;
                motor.controller->SetCtrlMode(Motor::MODE_COMMAND_POSITION);
            }
            motor.controller->SetPositionSetPoint(
                    (int32_t) (*(float *) _data * (float) motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS));
            if (_data[4]) // Need Position & Finished ACK
            {
                tmpF = motor.controller->GetPosition();
                auto *b = (unsigned char *) &tmpF;
                for (int i = 0; i < 4; i++)
                    _data[i] = *(b + i);
                _data[4] = motor.controller->state == Motor::STATE_FINISH ? 1 : 0;
                txHeader.StdId = (boardConfig.canNodeId << 7) | 0x23;
                CAN_Send(&txHeader, _data);
            }
            break;
        case 0x06:  // Set Position with Time
            if (motor.controller->modeRunning != Motor::MODE_COMMAND_POSITION)
                motor.controller->SetCtrlMode(Motor::MODE_COMMAND_POSITION);
            motor.controller->SetPositionSetPointWithTime(
                    (int32_t) (*(float *) _data * (float) motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS),
                    *(float *) (_data + 4));
            if (_data[4]) // Need Position & Finished ACK
            {
                tmpF = motor.controller->GetPosition();
                auto *b = (unsigned char *) &tmpF;
                for (int i = 0; i < 4; i++)
                    _data[i] = *(b + i);
                _data[4] = motor.controller->state == Motor::STATE_FINISH ? 1 : 0;
                txHeader.StdId = (boardConfig.canNodeId << 7) | 0x23;
                CAN_Send(&txHeader, _data);
            }
            break;
        case 0x07:  // Set Position with Velocity-Limit
        {
            if (motor.controller->modeRunning != Motor::MODE_COMMAND_POSITION) {
                motor.config.motionParams.ratedVelocity = boardConfig.velocityLimit;
                motor.controller->SetCtrlMode(Motor::MODE_COMMAND_POSITION);
            }
            motor.config.motionParams.ratedVelocity =
                    (int32_t) (*(float *) (_data + 4) * (float) motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS);
            motor.controller->SetPositionSetPoint(
                    (int32_t) (*(float *) _data * (float) motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS));
            // Always Need Position & Finished ACK
            tmpF = motor.controller->GetPosition();
            auto *b = (unsigned char *) &tmpF;
            for (int i = 0; i < 4; i++)
                _data[i] = *(b + i);
            _data[4] = motor.controller->state == Motor::STATE_FINISH ? 1 : 0;
            txHeader.StdId = (boardConfig.canNodeId << 7) | 0x23;
            CAN_Send(&txHeader, _data);
        }
            break;

            // 0x10~0x1F CMDs with Memory
        case 0x11:  // Set Node-ID and Store to EEPROM
            boardConfig.canNodeId = *(uint32_t *) (_data);
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x12:  // Set Current-Limit and Store to EEPROM
            motor.config.motionParams.ratedCurrent = (int32_t) (*(float *) _data * 1000);
            boardConfig.currentLimit = motor.config.motionParams.ratedCurrent;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x13:  // Set Velocity-Limit and Store to EEPROM
            motor.config.motionParams.ratedVelocity =
                    (int32_t) (*(float *) _data *
                               (float) motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS);
            boardConfig.velocityLimit = motor.config.motionParams.ratedVelocity;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x14:  // Set Acceleration （and Store to EEPROM）
            tmpF = *(float *) _data * (float) motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS;

            motor.config.motionParams.ratedVelocityAcc = (int32_t) tmpF;
            motor.motionPlanner.velocityTracker.SetVelocityAcc((int32_t) tmpF);
            motor.motionPlanner.positionTracker.SetVelocityAcc((int32_t) tmpF);
            boardConfig.velocityAcc = motor.config.motionParams.ratedVelocityAcc;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x15:  // Apply Home-Position and Store to EEPROM
            motor.controller->ApplyPosAsHomeOffset();
            boardConfig.encoderHomeOffset = motor.config.motionParams.encoderHomeOffset %
                                            motor.MOTOR_ONE_CIRCLE_SUBDIVIDE_STEPS;
            boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x16:  // Set Auto-Enable and Store to EEPROM
            boardConfig.enableMotorOnBoot = (*(uint32_t *) (_data) == 1);
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x17:  // Set DCE Kp
            motor.config.ctrlParams.dce.kp = *(int32_t *) (_data);
            boardConfig.dce_kp = motor.config.ctrlParams.dce.kp;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x18:  // Set DCE Kv
            motor.config.ctrlParams.dce.kv = *(int32_t *) (_data);
            boardConfig.dce_kv = motor.config.ctrlParams.dce.kv;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x19:  // Set DCE Ki
            motor.config.ctrlParams.dce.ki = *(int32_t *) (_data);
            boardConfig.dce_ki = motor.config.ctrlParams.dce.ki;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x1A:  // Set DCE Kd
            motor.config.ctrlParams.dce.kd = *(int32_t *) (_data);
            boardConfig.dce_kd = motor.config.ctrlParams.dce.kd;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x1B:  // Set Enable Stall-Protect
            motor.config.ctrlParams.stallProtectSwitch = (*(uint32_t *) (_data) == 1);
            boardConfig.enableStallProtect = motor.config.ctrlParams.stallProtectSwitch;
            if (_data[4])
                boardConfig.configStatus = CONFIG_COMMIT;
            break;


            // 0x20~0x2F Inquiry CMDs
        case 0x21: // Get Current
        {
            tmpF = motor.controller->GetFocCurrent();
            auto *b = (unsigned char *) &tmpF;
            for (int i = 0; i < 4; i++)
                _data[i] = *(b + i);
            _data[4] = (motor.controller->state == Motor::STATE_FINISH ? 1 : 0);

            txHeader.StdId = (boardConfig.canNodeId << 7) | 0x21;
            CAN_Send(&txHeader, _data);
        }
            break;
        case 0x22: // Get Velocity
        {
            tmpF = motor.controller->GetVelocity();
            auto *b = (unsigned char *) &tmpF;
            for (int i = 0; i < 4; i++)
                _data[i] = *(b + i);
            _data[4] = (motor.controller->state == Motor::STATE_FINISH ? 1 : 0);

            txHeader.StdId = (boardConfig.canNodeId << 7) | 0x22;
            CAN_Send(&txHeader, _data);
        }
            break;
        case 0x23: // Get Position
        {
            tmpF = motor.controller->GetPosition();
            auto *b = (unsigned char *) &tmpF;
            for (int i = 0; i < 4; i++)
                _data[i] = *(b + i);
            // Finished ACK
            _data[4] = motor.controller->state == Motor::STATE_FINISH ? 1 : 0;
            txHeader.StdId = (boardConfig.canNodeId << 7) | 0x23;
            CAN_Send(&txHeader, _data);
        }
            break;
        case 0x24: // Get Offset
        {
            tmpI = motor.config.motionParams.encoderHomeOffset;
            auto *b = (unsigned char *) &tmpI;
            for (int i = 0; i < 4; i++)
                _data[i] = *(b + i);
            txHeader.StdId = (boardConfig.canNodeId << 7) | 0x24;
            CAN_Send(&txHeader, _data);
        }
            break;

        case 0x7e:  // Erase Configs
            boardConfig.configStatus = CONFIG_RESTORE;
            break;
        case 0x7f:  // Reboot
            HAL_NVIC_SystemReset();
            break;
        default:
            break;
    }
#endif
}