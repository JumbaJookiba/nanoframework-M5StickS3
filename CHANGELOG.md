# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Comprehensive build instructions in BUILD_INSTRUCTIONS.md
- Quick fix guide for common errors in QUICKFIX.md
- Automatic creation of required CMake configuration files in GitHub Actions workflow
- Firmware package creation with flash instructions and README
- Release creation workflow for firmware distribution
- Version tracking in firmware packages

### Fixed
- **Issue #10**: Fixed "user-tools-repos.json not found" error during CMake configuration
  - GitHub Actions workflow now automatically creates `config/user-tools-repos.json`
  - GitHub Actions workflow now automatically creates `config/user-prefs.json`
  - Updated documentation to guide users through creating these files for local builds
- Improved CMake configure step to auto-detect xtensa toolchain path
- Enhanced firmware packaging with proper flash addresses and instructions

### Changed
- Updated README.md with links to comprehensive documentation
- Improved GitHub Actions workflow with better error handling
- Enhanced build validation with checkmarks for better visibility
- Updated firmware artifact naming and organization

### Documentation
- Added BUILD_INSTRUCTIONS.md - Complete step-by-step build guide
- Added QUICKFIX.md - Fast reference for common CMake preset errors
- Added ISSUE_10_FIX.md - Technical summary of the user-tools-repos.json fix
- Updated README.md with quick links and improved build instructions
- Added inline documentation in workflow file

## [0.1.0] - Initial Release

### Added
- M5StickS3 target support for nanoFramework
- ESP32-S3 configuration with OPI PSRAM support
- ST7789P3 display driver integration
- I2C configuration for BMI270 IMU and AXP2101 PMU
- USB CDC support for debugging and deployment
- LittleFS support for file system operations
- Custom 8MB partition table
- GitHub Actions CI/CD workflow
- Configuration validation workflow

### Hardware Support
- ESP32-S3-PICO-1-N8R8 (8MB Flash, 8MB PSRAM)
- ST7789P3 1.14" TFT display (135x240)
- BMI270 6-axis IMU
- AXP2101 power management
- Native USB CDC
- WiFi 802.11 b/g/n

### Build System
- CMake preset configuration
- ESP-IDF v5.5.4 integration
- Automated firmware building
- Binary artifact upload
