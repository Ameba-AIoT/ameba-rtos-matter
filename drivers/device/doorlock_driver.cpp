#include <doorlock_driver.h>
#include <support/logging/CHIPLogging.h>

void MatterDoorLock::Init(PinName pin)
{
    mPwm_obj                        = (pwmout_t *) pvPortMalloc(sizeof(pwmout_t));
    mPwm_obj->pwm_idx               = 1;
    pwmout_init(mPwm_obj, pin);

    // Get lock state by reading sensor or something
    locked = false;
}

void MatterDoorLock::Lock(void)
{
    if (locked)
    {
        ChipLogProgress(DeviceLayer, "[MatterDoorLock] Door is already locked!");
    }
    else
    {
        ChipLogProgress(DeviceLayer, "[MatterDoorLock] Locking door...");
        locked = true;
    }
}

void MatterDoorLock::Unlock(void)
{
    if(locked)
    {
        ChipLogProgress(DeviceLayer, "[MatterDoorLock] Unlocking door...");
        locked = false;
    }
    else
    {
        ChipLogProgress(DeviceLayer, "[MatterDoorLock] Door is already unlocked!");
    }

}

bool MatterDoorLock::GetLockStatus(void)
{
    return locked;
}
