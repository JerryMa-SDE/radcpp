Push-Location $PSScriptRoot
Write-Host "radcpp: Current working directory: $(Get-Location)"

$vcpkgRoot = $Env:VCPKG_ROOT
if ($null -eq $vcpkgRoot -or !(Test-Path $vcpkgRoot -PathType Container))
{
    Write-Host "radcpp: You need to setup vcpkg and set `$Env:VCPKG_ROOT to its path."
    Write-Host "radcpp: Please refer to: https://vcpkg.io/en/getting-started.html"
    return
}

Set-Location $PSScriptRoot
$sourceDir = $PSScriptRoot
$buildDir = Join-Path -Path $sourceDir -ChildPath "build/"
$vcpkgToolchainFile = Join-Path -Path $vcpkgRoot -ChildPath "scripts/buildsystems/vcpkg.cmake"
if (Test-Path -Path $buildDir -PathType Container)
{
    Write-Host "radcpp: Build directory already existed: $buildDir"
    while ($true)
    {
        $choice = Read-Host -Prompt "radcpp: Remove exist build directory and make a clean build? Enter your choice (y/n for yes/no)"
        if ($choice -eq "y" -or $choice -eq "yes")
        {
            Remove-Item -Path $buildDir -Recurse -Force
            break
        }
        if ($choice -eq "n" -or $choice -eq "no")
        {
            break
        }
    }
}

Write-Host "radcpp: Build directory: $buildDir"
if (Test-Path -Path $vcpkgToolchainFile -PathType Leaf)
{
    Write-Host "radcpp: Found vcpkg toolchain file: $vcpkgToolchainFile"
}
else
{
    Write-Host "radcpp: Cannot found vcpkg toolchain file: $vcpkgToolchainFile"
    Write-Host "radcpp: You need to setup vcpkg and set `$Env:VCPKG_ROOT to its path."
    Write-Host "radcpp: Please refer to: https://vcpkg.io/en/getting-started.html"
    return
}
Write-Host "radcpp: Generating project files:"
$cmakeCommand = "cmake -S `"$sourceDir`" -B `"$buildDir`" -DCMAKE_TOOLCHAIN_FILE=`"$vcpkgToolchainFile`""
Write-Host $cmakeCommand
Invoke-Expression $cmakeCommand

Pop-Location
