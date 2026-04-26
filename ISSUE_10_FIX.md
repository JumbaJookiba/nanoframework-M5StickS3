# Issue #10 Fix Summary

## Problem

The build workflow and manual builds were failing with the error:
```
CMake Error: Could not read presets from D:/nf-interpreter:
File not found: D:/nf-interpreter/targets/ThreadX/SiliconLabs/SL_STK3701A/../../../../config/user-tools-repos.json
```

## Root Cause

The nanoFramework CMake preset system requires two configuration files that must be created manually:

1. `config/user-tools-repos.json` - Defines paths to ESP-IDF and toolchains
2. `config/user-prefs.json` - Defines build preferences

The M5StickS3 preset inherits from `"user-tools-repos"` and `"user-prefs"` presets, which require these files. When the files don't exist, CMake fails during the preset loading phase.

**Critical Discovery**: When running `cmake --preset M5StickS3` from the root `nf-interpreter` directory, CMake loads ALL preset files from all subdirectories (ESP32, ThreadX, STM32, etc.). Many of these other platform presets ALSO inherit from `user-tools-repos` and `user-prefs`. This causes CMake to fail even if you're only trying to build for ESP32.

**Solution**: Run CMake from the `targets/ESP32` directory instead of the root, which limits preset loading to ESP32-specific presets only.

## Solution Implemented

### 1. Updated GitHub Actions Workflow

Added a new step in `.github/workflows/build.yml` to automatically create the required configuration files:

```yaml
- name: Create user configuration files
  run: |
	mkdir -p nf-interpreter/config

	cat > nf-interpreter/config/user-tools-repos.json << 'EOF'
	{
	  "toolsRepos": {
		"ESP32_TOOLS_PATH": "$env{HOME}/.espressif/tools",
		"ESP32_IDF_PATH": "$env{HOME}/.espressif/frameworks/esp-idf-v5.5.4",
		"ESP32_LIBS_PATH": "$env{HOME}/.espressif/frameworks/esp-idf-v5.5.4"
	  }
	}
	EOF

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

This step runs **before** the CMake configuration step, ensuring the files exist when CMake tries to load the presets.

### 2. Improved CMake Configuration

Updated the configure step to:
- Set proper environment variables
- Auto-detect the xtensa toolchain path (instead of hardcoding)
- **Run from `targets/ESP32` directory** (critical fix)
- Pass all required paths explicitly

```yaml
- name: Configure M5StickS3 build
  run: |
	source ~/.espressif/frameworks/esp-idf-v5.5.4/export.sh

	export ESP32_IDF_PATH=$HOME/.espressif/frameworks/esp-idf-v5.5.4
	export ESP32_TOOLS_PATH=$HOME/.espressif/tools
	export ESP32_LIBS_PATH=$HOME/.espressif/frameworks/esp-idf-v5.5.4

	XTENSA_TOOLCHAIN=$(find $HOME/.espressif/tools/xtensa-esp-elf -type d -name "xtensa-esp-elf" | grep -E "esp-[0-9]" | head -1)

	# Run from ESP32 directory to avoid loading other platform presets
	cd nf-interpreter/targets/ESP32

	cmake --preset M5StickS3 \
	  -DTOOLCHAIN_PREFIX=$XTENSA_TOOLCHAIN \
	  -DESP32_IDF_PATH=$ESP32_IDF_PATH \
	  -DESP32_TOOLS_PATH=$ESP32_TOOLS_PATH
```

### 3. Created Documentation

Created three comprehensive documentation files:

#### BUILD_INSTRUCTIONS.md
Complete step-by-step guide for:
- Local builds on Linux/WSL
- Installing prerequisites
- Creating config files (the key fix)
- Building and flashing
- Troubleshooting common errors

#### QUICKFIX.md
Fast reference for the specific error with:
- Copy-paste commands for Linux/macOS/WSL
- Copy-paste commands for Windows PowerShell
- Verification steps

#### Updated README.md
Added quick links to:
- Complete build instructions
- Quick fix guide
- GitHub Actions build option (recommended for users)

## Testing

The fix should now allow:

1. **GitHub Actions builds** to complete successfully without manual intervention
2. **Local builds** to work if users follow BUILD_INSTRUCTIONS.md
3. **Manual build users** to quickly fix the error using QUICKFIX.md

## Files Changed

1. `.github/workflows/build.yml` - Added config file creation step and improved CMake configuration
2. `BUILD_INSTRUCTIONS.md` - New comprehensive build guide
3. `QUICKFIX.md` - New quick fix reference
4. `README.md` - Updated with links to documentation
5. `ISSUE_10_FIX.md` - This summary document

## Verification Steps

To verify the fix works:

1. **Test GitHub Actions**:
   - Push changes to trigger workflow
   - Verify "Create user configuration files" step succeeds
   - Verify "Configure M5StickS3 build" step succeeds
   - Verify firmware binaries are created and uploaded

2. **Test Local Build**:
   - Clone fresh nf-interpreter
   - Follow BUILD_INSTRUCTIONS.md step-by-step
   - Verify config files are created
   - Verify CMake configure succeeds
   - Verify build completes

3. **Test Quick Fix**:
   - Intentionally skip config file creation
   - Encounter the error
   - Follow QUICKFIX.md
   - Verify error is resolved

## Related Issues

- Issue #8 - Build workflow completion (original request)
- Issue #10 - user-tools-repos.json not found error (this fix)

## Next Steps

After this fix is merged:

1. Test the GitHub Actions workflow completes successfully
2. Create a release with firmware binaries
3. Test flashing the firmware to actual M5StickS3 hardware
4. Update documentation based on real-world testing feedback
