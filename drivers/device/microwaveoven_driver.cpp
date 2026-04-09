#include <microwaveoven_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterMicrowaveOven::Init(PinName pin)
{
    mPwm_obj               = (pwmout_t *) pvPortMalloc(sizeof(pwmout_t));
#if defined(CONFIG_AMEBAGREEN2)
    mPwm_obj->pwmtimer_idx = 4;
#endif
    mPwm_obj->pwm_idx      = 0;
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
