#include <dishwasher_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterDishwasher::Init(PinName pin)
{
    mPwm_obj                        = (pwmout_t*) pvPortMalloc(sizeof(pwmout_t));

    pwmout_init(mPwm_obj, pin);
#if defined(CONFIG_PLATFORM_8710C)
    pwmout_period_us(mPwm_obj, 20000); //pwm period = 20ms
    pwmout_start(mPwm_obj);
#endif
}

void MatterDishwasher::deInit(void)
{
    vPortFree(mPwm_obj);
}

uint16_t MatterDishwasher::GetMode(void)
{
    return mode;
}

void MatterDishwasher::SetMode(uint16_t newMode)
{
    mode = newMode;
}

int16_t MatterDishwasher::GetMaxTemperature(void)
{
    return maxTemperature;
}

int16_t MatterDishwasher::GetMinTemperature(void)
{
    return minTemperature;
}

int16_t MatterDishwasher::GetTemperature(void)
{
    return localTemperature;
}

void MatterDishwasher::SetTemperature(int16_t temp)
{
    if ((temp >= minTemperature) && (temp <= maxTemperature))
    {
        localTemperature = temp;
    }
    else
    {
        ChipLogProgress(DeviceLayer, "Temperature must be set between %i and %i", minTemperature, maxTemperature);
    }
}

void MatterDishwasher::SetAlarm(void)
{
    // depends on customer implementation
}
