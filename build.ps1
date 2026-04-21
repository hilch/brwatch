# PowerShell build script for brwatch with MSYS2 mingw32
# This script compiles brwatch using mingw32-make
# Prerequisites: MSYS2 with mingw32 toolchain installed

param(
    [switch]$Clean = $false,
    [switch]$Verbose = $false,
    [string]$Msys2Path = "C:\msys64"
)

$ErrorActionPreference = "Stop"

# Color output functions
function Write-Success {
    param([string]$Message)
    Write-Host $Message -ForegroundColor Green
}

function Write-Error-Custom {
    param([string]$Message)
    Write-Host $Message -ForegroundColor Red
}

function Write-Info {
    param([string]$Message)
    Write-Host $Message -ForegroundColor Cyan
}

# Prepend MSYS2 mingw32 bin to PATH so we use the 32-bit toolchain
$mingw32Bin = "$Msys2Path\mingw32\bin"
if (Test-Path $mingw32Bin) {
    $env:PATH = "$mingw32Bin;$env:PATH"
}
else {
    Write-Host "Warning: MSYS2 mingw32 not found at '$mingw32Bin'. Falling back to system PATH." -ForegroundColor Yellow
    Write-Host "Use -Msys2Path to specify your MSYS2 installation folder, e.g.: .\build.ps1 -Msys2Path C:\msys64" -ForegroundColor Yellow
}

# Check for required tools
function Test-Requirement {
    param([string]$ToolName, [string]$Command = $ToolName)
    
    try {
        & $Command --version > $null 2>&1
        Write-Success "[OK] $ToolName found"
        return $true
    }
    catch {
        Write-Error-Custom "[FAIL] $ToolName not found. Please install MSYS2 mingw32 toolchain."
        return $false
    }
}

# Main script
Write-Info "========================================="
Write-Info "brwatch Build Script (MSYS2 mingw32)"
Write-Info "========================================="
Write-Info ""

# Verify requirements
Write-Info "Checking prerequisites..."
$gcc_ok = Test-Requirement "GCC" "gcc"
$make_ok = Test-Requirement "make" "mingw32-make"
$windres_ok = Test-Requirement "windres" "windres"

if (-not ($gcc_ok -and $make_ok -and $windres_ok)) {
    Write-Error-Custom ""
    Write-Error-Custom "Missing required tools. Please install MSYS2 mingw32:"
    Write-Error-Custom "  https://www.msys2.org/"
    Write-Error-Custom ""
    Write-Error-Custom "After installation, run in MSYS2 MinGW 32-bit terminal:"
    Write-Error-Custom "  pacman -S mingw-w64-i686-toolchain mingw-w64-i686-binutils"
    exit 1
}

Write-Info ""

# Create build directory if it doesn't exist
if (-not (Test-Path "build")) {
    Write-Info "Creating build directory..."
    New-Item -ItemType Directory -Path "build" -Force | Out-Null
    Write-Success "Build directory created"
}

Write-Info ""

# Clean build if requested
if ($Clean) {
    Write-Info "Cleaning previous build..."
    & mingw32-make -f Makefile clean
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Clean failed"
        exit 1
    }
    Write-Success "Clean completed"
    Write-Info ""
}

# Compile
Write-Info "Starting compilation..."
Write-Info ""

if ($Verbose) {
    & mingw32-make -f Makefile all
}
else {
    # Run via cmd to avoid PS treating gcc warnings (stderr) as errors
    cmd /c "mingw32-make -f Makefile all 2>&1" | Tee-Object -FilePath build/build.log
}

if ($LASTEXITCODE -ne 0) {
    Write-Error-Custom ""
    Write-Error-Custom "Build failed! Check the errors above."
    Write-Info "Build log saved to: build/build.log"
    exit 1
}

Write-Info ""
Write-Success "========================================="
Write-Success "[OK] Build completed successfully!"
Write-Success "========================================="
Write-Info ""
Write-Info "Output: build/brwatch.exe"
Write-Info ""
