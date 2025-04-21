#include <microwaveoven_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterMicrowaveOven::Init(PinName pin)
{
    mPwm_obj                        = (pwmout_t *) pvPortMalloc(sizeof(pwmout_t));
    mPwm_obj->pwm_idx               = 1;
    pwmout_init(mPwm_obj, pin);
}

void MatterMicrowaveOven::deInit(void)
{
    vPortFree(mPwm_obj);
}

void MatterMicrowaveOven::setOpState(uint8_t state)
{
    if (mOpState == state) {
        return;
    }

    mOpState = state;
}
