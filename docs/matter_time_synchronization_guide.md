# Testing Time Synchronization Features

## Prerequisites

Build with the Time Synchronization feature and SNTP enabled:

- `CONFIG_MATTER_TIME_SYNC_EN` (menuconfig) → defines `CONFIG_ENABLE_AMEBA_TIME_SYNC=1`, which registers `AmebaTimeSyncDelegate` in `core/matter_core.cpp`.
- `CONFIG_ENABLE_AMEBA_SNTP=1` (set for the CMake / ameba-rtos v1.2 build in `project/cmake/flags/public_definitions_matter.cmake`). Priority 1 and priority 3 both rely on the SNTP client; without it those paths compile out.

## How the priorities map to the code

The Time Synchronization cluster tries time sources in order; each maps to a path in `drivers/matter_drivers/time_synchronization/ameba_time_sync_delegate.cpp`:

| Priority | Source | Code path | Reported `TimeSource` |
|----------|--------|-----------|-----------------------|
| 1 | Platform (on-device RTC / SNTP via `matter_timers`) | `UpdateTimeFromPlatformSource()` | set by delegate |
| 2 | Trusted Node (another node's Time Sync cluster) | handled by the cluster server | `3` (kNodeTimeCluster) |
| 3 | DefaultNTP / SNTP server | `UpdateTimeUsingNTPFallback()` | `4` (kNonMatterSNTP) |

Periodic background resync (ICD builds only) runs through `TryPeriodicResync()`, invoked from `AmebaObserver::OnEnterActiveMode()` in `core/matter_fabric_observer.h`.

## Important reminder — test hooks

For the priority 2 / priority 3 tests below to run, priority 1 must be forced unavailable. This is controlled by two **compile-time** hooks set in `core/matter_core.cpp`:

    sAmebaTimeSyncDelegate.SetSimulatePlatformSourceUnavailable(true);
    sAmebaTimeSyncDelegate.SetTimeResyncQuickTest(true);

These are not runtime-toggleable — changing them requires editing `matter_core.cpp` and rebuilding.

- `SetSimulatePlatformSourceUnavailable(true)` forces `UpdateTimeFromPlatformSource` to report unavailable, so the cluster falls through to priority 2 / 3. If left `false`, priority 1 (UtcTime from `matter_timers`) usually succeeds first and priorities 2/3 never run.
- `SetTimeResyncQuickTest(true)` shortens the periodic resync interval to 10 minutes (from 6 hours) so resync can be observed quickly.

> **Note:** both hooks are for testing only. For production firmware they must be `false` (priority 1 enabled, 6-hour resync interval).

Assumptions:

- Node 1 = Ameba DUT
- Node 2 = Linux all-clusters-app trusted time source
- Controller/admin node id = 112233
- Time Sync endpoint = 0
- Passcode = 20202021
- Discriminator = 3840
- Replace <WIFI_SSID> / <WIFI_PASS> as needed.

## Timezone (TZ)

### Method 1

1. Commission Ameba:

        ./chip-tool pairing ble-wifi 1 <WIFI_SSID> <WIFI_PASS> 20202021 3840
  
2. Try to get LocalTime

        ./chip-tool timesynchronization read local-time 1 0

    Expected:

        LocalTime: null

3. Set both valid UTC time and TimeZone/DSTOffset data.

        ./chip-tool timesynchronization set-time-zone '[{"offset": 28800, "validAt": 0, "name": "Asia/Singapore"}]' 1 0
        ./chip-tool timesynchronization set-dstoffset '[{"offset": 0, "validStarting": 0, "validUntil": null}]' 1 0
        ./chip-tool timesynchronization read local-time 1 0

    Expected:

        LocalTime: non-null

### Method 2

1. Commission Ameba with the `--time-zone` and `--dst-offset` options:

        ./chip-tool pairing ble-wifi 1 <WIFI_SSID> <WIFI_PASS> 20202021 3840 --time-zone '[{"offset": 28800, "validAt": 0, "name": "Asia/Singapore"}]' --dst-offset '[{"offset": 0, "validStarting": 0, "validUntil": null}]'

2. Try to read the LocalTime

        ./chip-tool timesynchronization read local-time 1 0

    Expected:

        LocalTime: non-null

## NTP CLient (NTPC)

1. Commission Ameba:

        ./chip-tool pairing ble-wifi 1 <WIFI_SSID> <WIFI_PASS> 20202021 3840
  
    After commissioning, time-source will usually be 2 (kAdmin) because chip-tool pushes UTC time during commissioning. That is expected.

2. Clear TrustedTimeSource on Node 1

        ./chip-tool timesynchronization set-trusted-time-source null 1 0

3. Set DefaultNTP on Node 1

        ./chip-tool timesynchronization set-default-ntp '"0.pool.ntp.org"' 1 0

4. Reboot / power-cycle Node 1. Do not re-run pairing. Just reset the Ameba board.

5. Verify priority 3 result

        ./chip-tool timesynchronization read trusted-time-source 1 0
        ./chip-tool timesynchronization read default-ntp 1 0
        ./chip-tool timesynchronization read time-source 1 0
        ./chip-tool timesynchronization read granularity 1 0
        ./chip-tool timesynchronization read utctime 1 0

    Expected:

        TrustedTimeSource: null
        DefaultNTP: "0.pool.ntp.org"
        TimeSource: 4
        Granularity: non-zero
        UTCTime: non-null
        TimeSource: 4 = kNonMatterSNTP, meaning priority 3 worked.

## TimeSyncClient (TSC)

1. Start Linux all-clusters-app for Node 2 first, then:


        ./chip-tool pairing onnetwork 2 20202021


    Commission Ameba:

        ./chip-tool pairing ble-wifi 1 <WIFI_SSID> <WIFI_PASS> 20202021 3840
  
    After commissioning, time-source will usually be 2 (kAdmin) because chip-tool pushes UTC time during commissioning. That is expected.

2. Grant Node 1 access to read Time Sync from Node 2

        ./chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [112233], "targets": null}, {"fabricIndex": 1, "privilege": 3, "authMode": 2, "subjects": [1], "targets": null}]' 2 0

3. Configure Node 1 to trust Node 2 as time source

        ./chip-tool timesynchronization set-trusted-time-source '{"nodeID": 2, "endpoint": 0}' 1 0

4. Reboot / power-cycle Node 1. Do not re-run pairing. Just reset the Ameba board.

5. Verify priority 2 result

        ./chip-tool timesynchronization read trusted-time-source 1 0
        ./chip-tool timesynchronization read time-source 1 0
        ./chip-tool timesynchronization read granularity 1 0
        ./chip-tool timesynchronization read utctime 1 0

    Expected:

        TrustedTimeSource: { nodeID: 2, endpoint: 0, fabricIndex: 1 }
        TimeSource: 3
        Granularity: non-zero
        UTCTime: non-null
        TimeSource: 3 = kNodeTimeCluster, meaning priority 2 worked.
