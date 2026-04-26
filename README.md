# nanoframework-M5StickS3

nanoFramework custom firmware support for the [M5StickS3](https://docs.m5stack.com/en/core/M5StickS3) device.

## Hardware Overview

The M5StickS3 is a compact development kit based on the ESP32-S3-PICO-1-N8R8 module with the following hardware:

| Component | Details |
|-----------|---------|
| MCU | ESP32-S3-PICO-1-N8R8 (dual-core LX7 @ 240 MHz) |
| Flash | 8 MB OPI Flash (QIO) |
| PSRAM | 8 MB OPI PSRAM (Octal SPI) |
| Display | 1.14" TFT ST7789P3 (135×240 pixels) via SPI |
| IMU | BMI270 6-axis accelerometer/gyroscope via I2C |
| PMU | AXP2101 power management via I2C |
| USB | USB-C (native USB, CDC) |
| Buttons | M5 button (GPIO35), Side button (GPIO39) |
| IR LED | GPIO2  |
| Red LED | GPIO19 (shared with USB D-, unavailable when USB CDC is active) |
| Battery | Built-in Li-ion with AXP2101 management |

## Pin Configuration

### SPI — ST7789P3 Display

| Signal | GPIO |
|--------|------|
| CLK    | 13   |
| MOSI   | 15   |
| DC     | 14   |
| CS     | 37   |
| RST    | 12   |
| BLK (Backlight) | 27 |

### I2C — BMI270 IMU + AXP2101 PMU

| Signal | GPIO |
|--------|------|
| SDA    | 21   |
| SCL    | 22   |

| Device  | I2C Address |
|---------|-------------|
| BMI270  | 0x68        |
| AXP2101 | 0x34        |
| BM8563 RTC | 0x51     |

### GPIO — Buttons

| Button      | GPIO | Logic  |
|-------------|------|--------|
| M5 (power)  | 35   | Active LOW |
| Side button | 39   | Active LOW |

### USB CDC

| Signal | GPIO |
|--------|------|
| D-     | 19   |
| D+     | 20   |

## Firmware Features

- **PSRAM**: 8 MB OPI PSRAM enabled via `CONFIG_SPIRAM_MODE_OCT=y`
- **USB CDC**: Native USB for debugging and firmware deployment
- **SPI**: Configured for ST7789P3 display (SPI2_HOST / FSPI)
- **I2C**: Bus 0 (GPIO21/GPIO22) shared by BMI270, AXP2101, and BM8563
- **GPIO**: Buttons on GPIO35 and GPIO39
- **Graphics**: nanoFramework.Graphics with ST7789P3 driver
- **WiFi**: 802.11 b/g/n with full networking stack
- **File System**: LittleFS on internal flash (2 MB config partition)
- **RTC**: Real-time clock support

## Repository Structure

```
targets/
└── ESP32/
    ├── M5StickS3/                      # Target-specific files
    │   ├── CMakeLists.txt
    │   ├── target_common.h.in
    │   ├── target_common.c
    │   ├── target_BlockStorage.c/h
    │   ├── target_FileSystem.cpp
    │   ├── target_system_device_spi_config.cpp
    │   ├── target_system_device_i2c_config.cpp
    │   ├── target_system_io_ports_config.cpp
    │   ├── common/
    │   │   └── CMakeLists.txt
    │   └── nanoCLR/
    │       ├── target_board.h.in
    │       ├── nanoHAL.cpp
    │       └── nanoFramework.Graphics/
    │           ├── Graphics_Memory.cpp
    │           └── Spi_To_TouchPanel.cpp
    ├── defconfig/
    │   └── M5StickS3_defconfig         # nanoFramework build configuration
    └── _IDF/
        ├── sdkconfig.default_octal.esp32s3  # ESP-IDF SDK config (OPI PSRAM)
        └── esp32s3/
            └── partitions_nanoclr_8mb.csv   # Flash partition table (8 MB)
```

## Building the Firmware

### Quick Links

- **📖 [Complete Build Instructions](BUILD_INSTRUCTIONS.md)** - Detailed step-by-step guide for local builds
- **🔧 [Quick Fix for Common Errors](QUICKFIX.md)** - Fix "user-tools-repos.json not found" error
- **☁️ Use GitHub Actions** - Build firmware automatically without local setup (see below)

### Building Locally

For complete local build instructions, see **[BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)**.

**Quick summary:**

1. Clone nf-interpreter
2. Copy M5StickS3 target files
3. **Create required config files** (see [QUICKFIX.md](QUICKFIX.md))
4. Install ESP-IDF v5.5.4
5. Configure and build

### Building with GitHub Actions (Recommended)

The easiest way to get firmware binaries is to use the automated GitHub Actions workflow:

1. **Fork this repository** (or use it directly if you have write access)
2. Go to **Actions** tab
3. Select **"Build M5StickS3 Firmware"** workflow
4. Click **"Run workflow"**
5. Select branch and optionally create a release
6. **Download** the firmware artifacts after the build completes

The workflow automatically:
- ✅ Creates all required configuration files
- ✅ Builds bootloader, partition table, and nanoCLR
- ✅ Packages firmware for easy flashing
- ✅ Provides detailed flash instructions

### Flashing the Firmware

After obtaining the firmware (via Actions or local build):

**Using nanoff (recommended):**
```bash
dotnet tool install -g nanoff
nanoff --target M5StickS3 --serialport COMx --update --binfile nanoCLR.bin
```

**Using esptool (manual):**
```bash
esptool.py --chip esp32s3 --port COMx --baud 921600 write_flash -z \
  0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0x10000 nanoCLR.bin
```

Replace `COMx` with your device port (`COM3`, `/dev/ttyUSB0`, etc.)

## CI/CD

The repository includes a GitHub Actions workflow (`.github/workflows/build.yml`) that:

- Validates all configuration files
- Clones nf-interpreter and applies the M5StickS3 target
- Builds the firmware
- Uploads build artifacts

## Contributing

Contributions are welcome. Please open an issue or submit a pull request.

## License

MIT — see [LICENSE](LICENSE).
