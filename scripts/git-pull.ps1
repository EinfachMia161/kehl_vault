Set-Location "$PSScriptRoot\.."

Write-Host "Git Pull..."

git pull --ff-only

if ($LASTEXITCODE -ne 0) {
    Write-Host "Git Pull failed."
    exit 1
}

Write-Host "Git Pull complete."