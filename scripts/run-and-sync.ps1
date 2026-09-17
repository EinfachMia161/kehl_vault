Set-Location "$PSScriptRoot\.."

Write-Host "Starting kehl_vault..."

# --------------------------------------------------
# Find the compiler used by CMake
# --------------------------------------------------

$cmakeCache = Join-Path (Get-Location) "cmake-build-debug\CMakeCache.txt"

if (!(Test-Path $cmakeCache)) {
    Write-Host "ERROR: CMakeCache.txt not found."
    Write-Host "Run the project once in CLion first."
    exit 1
}

$compilerLine = Select-String -Path $cmakeCache -Pattern "^CMAKE_CXX_COMPILER:FILEPATH=" |
        Select-Object -First 1

if ($null -eq $compilerLine) {
    Write-Host "ERROR: C++ compiler not found in CMakeCache.txt."
    exit 1
}

$compilerPath = ($compilerLine.Line -split "=", 2)[1]

if (!(Test-Path $compilerPath)) {
    Write-Host "ERROR: Compiler does not exist:"
    Write-Host $compilerPath
    exit 1
}

$compilerDir = Split-Path $compilerPath -Parent

Write-Host "Compiler:"
Write-Host $compilerPath

# Add compiler directory to PATH
$env:Path = "$compilerDir;$env:Path"

# --------------------------------------------------
# Find executable
# --------------------------------------------------

$exe = Join-Path (Get-Location) "cmake-build-debug\kehl_vault.exe"

if (!(Test-Path $exe)) {
    Write-Host "ERROR: kehl_vault.exe not found."
    Write-Host "Build the project in CLion first."
    exit 1
}

Write-Host "Executable:"
Write-Host $exe

# --------------------------------------------------
# Run program
# --------------------------------------------------

Write-Host "Running program..."

& $exe

$exitCode = $LASTEXITCODE

Write-Host "Program exit code: $exitCode"

if ($exitCode -ne 0) {
    Write-Host "Program failed. No Git sync."
    exit $exitCode
}

Write-Host "Program finished successfully."

# --------------------------------------------------
# Git
# --------------------------------------------------

Write-Host "Running Git..."

git add .

$changes = git status --porcelain

if ([string]::IsNullOrWhiteSpace($changes)) {
    Write-Host "No changes to commit."
    exit 0
}

$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

git commit -m "Auto-save: $timestamp"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Git commit failed."
    exit 1
}

git push

if ($LASTEXITCODE -ne 0) {
    Write-Host "Git push failed."
    exit 1
}

Write-Host "Git sync complete."