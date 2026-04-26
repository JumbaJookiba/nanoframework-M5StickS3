# M5StickS3 nanoFramework Build Instructions

This document provides detailed instructions for building the M5StickS3 nanoFramework firmware both locally and via GitHub Actions.

## Prerequisites

### For Local Builds

1. **Operating System**: Linux (Ubuntu/Debian recommended) or WSL2 on Windows
2. **Python**: Version 3.11 or later
3. **CMake**: Version 3.20 or later
4. **Git**: Latest version
5. **ESP-IDF v5.5.4**: Espressif IoT Development Framework

### Required Tools

```bash
sudo apt-get update
sudo apt-get install -y \
  git wget flex bison gperf python3 python3-pip python3-venv \
  cmake ninja-build ccache libffi-dev libssl-dev dfu-util \
  libusb-1.0-0
```

## Local Build Instructions

### Step 1: Clone the Repository

```bash
git clone https://github.com/JumbaJookiba/nanoframework-M5StickS3.git
cd nanoframework-M5StickS3
```

### Step 2: Clone nf-interpreter

```bash
git clone --depth 1 https://github.com/nanoframework/nf-interpreter.git
```

### Step 3: Copy Target Files

```bash
# Copy M5StickS3 target directory
cp -r targets/ESP32/M5StickS3 nf-interpreter/targets/ESP32/M5StickS3

# Copy defconfig
cp targets/ESP32/defconfig/M5StickS3_defconfig \
   nf-interpreter/targets/ESP32/defconfig/M5StickS3_defconfig

# Copy sdkconfig
cp targets/ESP32/_IDF/sdkconfig.default_octal.esp32s3 \
   nf-interpreter/targets/ESP32/_IDF/sdkconfig.default_octal.esp32s3

# Copy partition table (if needed)
if [ ! -f nf-interpreter/targets/ESP32/_IDF/esp32s3/partitions_nanoclr_8mb.csv ]; then
  cp targets/ESP32/_IDF/esp32s3/partitions_nanoclr_8mb.csv \
	 nf-interpreter/targets/ESP32/_IDF/esp32s3/partitions_nanoclr_8mb.csv
fi
```

### Step 4: Install ESP-IDF

```bash
# Create ESP-IDF directory
mkdir -p ~/.espressif/frameworks

# Clone ESP-IDF v5.5.4
git clone --recursive --depth 1 --branch v5.5.4 \
  https://github.com/espressif/esp-idf.git \
  ~/.espressif/frameworks/esp-idf-v5.5.4

# Install ESP32-S3 toolchain
cd ~/.espressif/frameworks/esp-idf-v5.5.4
./install.sh esp32s3

# Return to project directory
cd -
```

### Step 5: Create User Configuration Files

This is the crucial step that fixes the "File not found: user-tools-repos.json" error:

```bash
# Create config directory
mkdir -p nf-interpreter/config

# Create user-tools-repos.json
cat > nf-interpreter/config/user-tools-repos.json << 'EOF'
{
  "toolsRepos": {
	"ESP32_TOOLS_PATH": "$env{HOME}/.espressif/tools",
	"ESP32_IDF_PATH": "$env{HOME}/.espressif/frameworks/esp-idf-v5.5.4",
	"ESP32_LIBS_PATH": "$env{HOME}/.espressif/frameworks/esp-idf-v5.5.4"
  }
}
EOF

# Create user-prefs.json
cat > nf-interpreter/config/user-prefs.json << 'EOF'
{
  "configurePresets": [
	{
	  "name": "user-prefs",
	  "hidden": true,
	  "cacheVariables": {
		"CMAKE_BUILD_TYPE": "Release"
	  }
	}
  ]
}
EOF
```

### Step 6: Add M5StickS3 Preset to CMakePresets.json

```bash
cd nf-interpreter/targets/ESP32

python3 << 'EOF'
import json

presets_file = "CMakePresets.json"
with open(presets_file, "r") as f:
	data = json.load(f)

# Check if M5StickS3 preset already exists
existing = [p for p in data.get("configurePresets", []) if p.get("name") == "M5StickS3"]
if not existing:
	m5sticks3_preset = {
		"name": "M5StickS3",
		"inherits": [
			"xtensa-esp32s3-preset",
			"user-tools-repos",
			"user-prefs"
		],
		"hidden": False,
		"cacheVariables": {
			"TARGET_NAME": "${presetName}",
			"NF_TARGET_DEFCONFIG": "targets/ESP32/defconfig/M5StickS3_defconfig"
		}
	}
	data["configurePresets"].append(m5sticks3_preset)
	with open(presets_file, "w") as f:
		json.dump(data, f, indent=2)
	print("✅ Added M5StickS3 preset to CMakePresets.json")
else:
	print("ℹ️  M5StickS3 preset already exists")
EOF

cd ../../..
```

### Step 7: Configure the Build

```bash
# Navigate to ESP32 targets directory (important!)
cd nf-interpreter/targets/ESP32

# Source ESP-IDF environment
source ~/.espressif/frameworks/esp-idf-v5.5.4/export.sh

# Set environment variables
export ESP32_IDF_PATH=$HOME/.espressif/frameworks/esp-idf-v5.5.4
export ESP32_TOOLS_PATH=$HOME/.espressif/tools
export ESP32_LIBS_PATH=$HOME/.espressif/frameworks/esp-idf-v5.5.4

# Find xtensa toolchain
XTENSA_TOOLCHAIN=$(find $HOME/.espressif/tools/xtensa-esp-elf -type d -name "xtensa-esp-elf" | grep -E "esp-[0-9]" | head -1)
echo "Using xtensa toolchain at: $XTENSA_TOOLCHAIN"

# Configure CMake from ESP32 directory
cmake --preset M5StickS3 \
  -DTOOLCHAIN_PREFIX=$XTENSA_TOOLCHAIN \
  -DESP32_IDF_PATH=$ESP32_IDF_PATH \
  -DESP32_TOOLS_PATH=$ESP32_TOOLS_PATH
```

**Important**: Run `cmake --preset` from the `nf-interpreter/targets/ESP32` directory, not from the root `nf-interpreter` directory. This ensures CMake only loads the ESP32 preset files and not presets from other platforms (ThreadX, STM32, etc.) that also require the same config files.

### Step 8: Build the Firmware

```bash
# Build all targets
cmake --build --preset M5StickS3 --target all
```

### Step 9: Locate the Built Binaries

After a successful build, you'll find the firmware binaries at:

```
nf-interpreter/build/M5StickS3/bootloader/bootloader.bin
nf-interpreter/build/M5StickS3/partition_table/partition-table.bin
nf-interpreter/build/M5StickS3/nanoCLR.bin
```

## Flashing the Firmware

### Using nanoff (Recommended)

```bash
# Install nanoff
dotnet tool install -g nanoff

# Flash the firmware
nanoff --target M5StickS3 --serialport /dev/ttyUSB0 --update --binfile nf-interpreter/build/M5StickS3/nanoCLR.bin
```

### Using esptool (Manual)

```bash
# Install esptool
pip install esptool

# Flash all binaries
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 write_flash -z \
  0x0 nf-interpreter/build/M5StickS3/bootloader/bootloader.bin \
  0x8000 nf-interpreter/build/M5StickS3/partition_table/partition-table.bin \
  0x10000 nf-interpreter/build/M5StickS3/nanoCLR.bin
```

## Building via GitHub Actions

The repository includes a GitHub Actions workflow that automatically builds the firmware.

### Automatic Builds

The workflow runs automatically on:
- Push to `main` or `develop` branches
- Pull requests to `main`

### Manual Build with Release

1. Go to the repository on GitHub
2. Click on "Actions" tab
3. Select "Build M5StickS3 Firmware" workflow
4. Click "Run workflow"
5. Select branch (usually `main`)
6. Set "Create a release" to `true` for a release
7. Click "Run workflow"

The artifacts will be available for download after the build completes.

## Troubleshooting

### Error: "File not found: user-tools-repos.json"

**Solution**: Make sure you created the config files in Step 5.

```bash
# Verify the files exist
ls -la nf-interpreter/config/
```

### Error: "xtensa-esp-elf not found"

**Solution**: Ensure ESP-IDF is properly installed and environment is sourced.

```bash
source ~/.espressif/frameworks/esp-idf-v5.5.4/export.sh
which xtensa-esp32s3-elf-gcc
```

### Error: "Could not find preset M5StickS3"

**Solution**: Verify the preset was added to CMakePresets.json.

```bash
grep -A 5 "M5StickS3" nf-interpreter/targets/ESP32/CMakePresets.json
```

### CMake Configuration Fails

**Solution**: Clean the build directory and try again.

```bash
rm -rf nf-interpreter/build
cmake --preset M5StickS3 ...
```

### Build Fails with "ninja: error"

**Solution**: Ensure all dependencies are installed and ESP-IDF is properly set up.

```bash
# Re-source ESP-IDF
source ~/.espressif/frameworks/esp-idf-v5.5.4/export.sh

# Verify IDF is working
idf.py --version
```

## Windows Build Instructions (WSL2)

For Windows users, it's recommended to use WSL2 (Windows Subsystem for Linux):

1. Install WSL2 with Ubuntu 24.04
2. Follow the Linux build instructions above
3. Access Windows COM ports as `/dev/ttySx` (requires usbipd-win)

Alternatively, you can use the GitHub Actions workflow to build and download the firmware artifacts.

## Additional Resources

- [nanoFramework Documentation](https://docs.nanoframework.net/)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/v5.5.4/)
- [M5StickC Plus2 Documentation](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)

## Support

For issues and questions:
- Open an issue: https://github.com/JumbaJookiba/nanoframework-M5StickS3/issues
- nanoFramework Discord: https://discord.gg/gCyBu8T
