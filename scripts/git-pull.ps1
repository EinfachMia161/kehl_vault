Set-Location "$PSScriptRoot\.."

Write-Host "=== Git Pull ===" -ForegroundColor Cyan

git pull --ff-only

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Git Pull fehlgeschlagen!" -ForegroundColor Red
    exit 1
}

Write-Host "Git ist aktuell." -ForegroundColor Green