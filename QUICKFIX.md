# Quick Fix for "user-tools-repos.json not found" Error

If you encounter the error:
```
CMake Error: Could not read presets from D:/nf-interpreter:
File not found: D:/nf-interpreter/targets/ThreadX/SiliconLabs/SL_STK3701A/../../../../config/user-tools-repos.json
```

## The Problem

The nanoFramework CMake presets system requires two configuration files that are not included in the repository by default:
- `config/user-tools-repos.json` - Defines paths to toolchains and SDKs
- `config/user-prefs.json` - Defines user build preferences

Additionally, CMake must be run from the `targets/ESP32` directory (not the root) to avoid loading preset files from other platforms that also require these config files.

## The Solution

Run these commands in your `nf-interpreter` directory:

### On Linux/macOS/WSL:

```bash
# Navigate to nf-interpreter
cd nf-interpreter

# Create config directory
mkdir -p config

# Create user-tools-repos.json
cat > config/user-tools-repos.json << 'EOF'
{
  "toolsRepos": {
	"ESP32_TOOLS_PATH": "$env{HOME}/.espressif/tools",
	"ESP32_IDF_PATH": "$env{HOME}/.espressif/frameworks/esp-idf-v5.5.4",
	"ESP32_LIBS_PATH": "$env{HOME}/.espressif/frameworks/esp-idf-v5.5.4"
  }
}
EOF

# Create user-prefs.json
cat > config/user-prefs.json << 'EOF'
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

echo "✅ Configuration files created successfully!"
```

### On Windows (PowerShell):

```powershell
# Navigate to nf-interpreter
cd nf-interpreter

# Create config directory
New-Item -ItemType Directory -Force -Path config

# Create user-tools-repos.json
@"
{
  "toolsRepos": {
	"ESP32_TOOLS_PATH": "`$env{USERPROFILE}/.espressif/tools",
	"ESP32_IDF_PATH": "`$env{USERPROFILE}/.espressif/frameworks/esp-idf-v5.5.4",
	"ESP32_LIBS_PATH": "`$env{USERPROFILE}/.espressif/frameworks/esp-idf-v5.5.4"
  }
}
"@ | Out-File -FilePath config/user-tools-repos.json -Encoding UTF8

# Create user-prefs.json
@"
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
"@ | Out-File -FilePath config/user-prefs.json -Encoding UTF8

Write-Host "✅ Configuration files created successfully!"
```

## Verify the Files

Check that the files were created:

```bash
ls -la config/
# Should show:
# user-tools-repos.json
# user-prefs.json
```

Now try the CMake configure command again (from ESP32 directory):

```bash
cd nf-interpreter/targets/ESP32
source ~/.espressif/frameworks/esp-idf-v5.5.4/export.sh
cmake --preset M5StickS3
```

## Alternative: Use GitHub Actions

If local builds continue to fail, you can use the automated GitHub Actions workflow:

1. Fork the repository
2. Go to Actions tab
3. Run "Build M5StickS3 Firmware" workflow
4. Download the firmware artifacts

The GitHub Actions workflow automatically creates these configuration files.

## For More Help

See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) for complete step-by-step build instructions.
