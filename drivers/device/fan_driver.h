/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <platform_stdlib.h>
#include <pwmout_api.h>
#include <app/util/attribute-table.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/clusters/fan-control-server/fan-control-server.h>

class MatterFan
{
public:
    void Init(PinName pin);
    void deInit(void);
    void setFanMode(uint8_t mode);
    void setFanSpeedPercent(uint8_t speed);
    chip::app::Clusters::FanControl::FanModeEnum mapPercentToMode(uint8_t percent);
    uint8_t mapModeToPercent(uint8_t mode);

private:
    pwmout_t *mPwm_obj = NULL;
    uint8_t mMode;
    uint8_t mPercent;
    uint8_t mSpeed;
    uint8_t mMaxSpeed = 10;
};
