#include <washer_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterWasher::Init(PinName pin)
{
    mPwm_obj               = (pwmout_t *) pvPortMalloc(sizeof(pwmout_t));
#if defined(CONFIG_AMEBAGREEN2)
    mPwm_obj->pwmtimer_idx = 4;
#endif
    mPwm_obj->pwm_idx      = 0;
    pwmout_init(mPwm_obj, pin);
}

void MatterWasher::deInit(void)
{
    vPortFree(mPwm_obj);
}

void MatterWasher::Do(void)
{
    // implement action here
    ChipLogProgress(DeviceLayer, "Washer action");
}
