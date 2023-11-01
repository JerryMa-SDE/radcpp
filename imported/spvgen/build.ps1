Push-Location $PSScriptRoot
Write-Host "Current working directory: $(Get-Location)"

Write-Host "Build spvgen:"
$spvgenSourceDir = $PSScriptRoot
$spvgenExternalDir = Join-Path -Path $spvgenSourceDir -ChildPath "external/"
$spvgenExternalScript = Join-Path -Path $spvgenExternalDir -ChildPath "fetch_external_sources.py"
$spvgenBuildDir = Join-Path -Path $spvgenSourceDir -ChildPath "build/"
Push-Location $spvgenExternalDir
Write-Host "Current working directory: $(Get-Location)"
Write-Host "spvgen: fetch external sources:"
Invoke-Expression "python `"$spvgenExternalScript`""
Pop-Location
Invoke-Expression "cmake -S `"$spvgenSourceDir`" -B `"$spvgenBuildDir`""
if (Test-Path -Path $spvgenBuildDir -PathType Container) {
    Invoke-Expression "cmake --build `"$spvgenBuildDir`" --target spvgen --config Release"
}

Pop-Location
