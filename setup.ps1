# script for radcpp build: install required packages and generate project files with CMake.
Push-Location $PSScriptRoot

if ($PSVersionTable.PSVersion.Major -lt 7)
{
    Write-Host "radcpp: the setup script requires PowerShell 7 and later!"
    return
}

$psVersion = "$($PSVersionTable.PSVersion) $($PSVersionTable.PSEdition)"
Write-Host "radcpp: Execute: $PSCommandPath `($psVersion`)"
Write-Host "radcpp: Current working directory: $(Get-Location)"

$vcpkgRoot = $Env:VCPKG_ROOT
$vcpkgExecutableName = "vcpkg"
$vcpkgTriplet = $null

# repos need to clone for the build (may contain prebuilt binaries): 
$importedRepos = @(
    [PSCustomObject]@{
        Name = "spvgen"
        URL = "https://github.com/JerryMa-SDE/spvgen.git"
    },
    [PSCustomObject]@{
        Name = "Compressonator"
        URL = "https://github.com/JerryMa-SDE/Compressonator.git"
    }
)

if ($null -eq $vcpkgRoot)
{
    Write-Host "radcpp: You need to setup vcpkg and set `$Env:VCPKG_ROOT to its root."
    Write-Host "radcpp: Please refer to: https://vcpkg.io/en/getting-started.html"
    return
}

if (-not (Test-Path -Path $vcpkgRoot))
{
    Write-Host "radcpp: Cannot find vcpkg root: `$Env:VCPKG_ROOT=$vcpkgRoot"
    return
}

if ($IsWindows)
{
    $vcpkgExecutableName = "vcpkg.exe"
    $vcpkgTriplet = "x64-windows"
}

$vcpkgExecutable = Join-Path -Path $vcpkgRoot -ChildPath $vcpkgExecutableName
if (Test-Path -Path $vcpkgExecutable -PathType Leaf)
{
    Write-Host "radcpp: Found vcpkg executable: $vcpkgExecutable"
}
else
{
    Write-Host "radcpp: Cannot find vcpkg executable: $vcpkgExecutable"
    return
}

function installVcpkgPackages {
    param (
        [string[]]$packages,
        [string]$triplet
    )
    Push-Location $vcpkgRoot
    foreach ($package in $packages)
    {
        Write-Host "radcpp: install vcpkg package `"$package`"`:"
        if ($triplet) {
            Invoke-Expression "& `"$vcpkgExecutable`" install $package`:$triplet"
        } else {
            Invoke-Expression "& `"$vcpkgExecutable`" install $package"
        }
    }
    Pop-Location
}

Write-Host "radcpp: install vcpkg packages`:"
# required by rad:
installVcpkgPackages "boost","poco","rapidjson" -triplet $vcpkgTriplet
installVcpkgPackages "imath","glm","opencl" -triplet $vcpkgTriplet
installVcpkgPackages "gtest" -triplet $vcpkgTriplet
installVcpkgPackages "cpu-features" -triplet $vcpkgTriplet
installVcpkgPackages "sdl2[vulkan]" -triplet $vcpkgTriplet
installVcpkgPackages "ffmpeg" -triplet $vcpkgTriplet
installVcpkgPackages "assimp" -triplet $vcpkgTriplet

Write-Host "radcpp: update imported repos:"
$importedDir = Join-Path -Path $PSScriptRoot -ChildPath "imported/"
Push-Location $importedDir
foreach ($repo in $importedRepos)
{
    $repoPath = Join-Path -Path $importedDir -ChildPath $repo.Name
    if (-not (Test-Path -Path $repoPath -PathType Container))
    {
        Write-Host "radcpp: $($repo.Name) doesn't exist, cloning from $($repo.URL) ..."
        Invoke-Expression "git clone $($repo.URL) $($repo.Name)"
    }
    
    if (Test-Path -Path $repoPath -PathType Container)
    {
        Write-Host "radcpp: $($repo.Name) exists, updating ..."
        Push-Location $repo.Name
        Invoke-Expression "git fetch"
        Invoke-Expression "git pull"
        Pop-Location
    }
}
Pop-Location

$sourceDir = $PSScriptRoot
$buildDir = Join-Path -Path $sourceDir -ChildPath "build/"
$vcpkgToolchainFile = Join-Path -Path $vcpkgRoot -ChildPath "scripts/buildsystems/vcpkg.cmake"
Write-Host "radcpp: Build directory: $buildDir"
if (Test-Path -Path $vcpkgToolchainFile -PathType Leaf)
{
    Write-Host "radcpp: Found vcpkg toolchain file: $vcpkgToolchainFile"
}
else
{
    Write-Host "radcpp: Cannot find vcpkg toolchain file: $vcpkgToolchainFile"
    return
}

$cleanBuild = $true
if (Test-Path -Path $buildDir -PathType Container)
{
    Write-Host "radcpp: Build directory already existed: $buildDir"
    while ($true)
    {
        $choice = Read-Host -Prompt "radcpp: Make a clean build? Enter your choice (y/n for yes/no)"
        if ($choice -eq "y" -or $choice -eq "yes")
        {
            $cleanBuild = $true
            break
        }
        if ($choice -eq "n" -or $choice -eq "no")
        {
            $cleanBuild = $false
            break
        }
    }

    if ($cleanBuild)
    {
        Remove-Item -Path $buildDir -Recurse -Force
    }
}

Write-Host "radcpp: Generating project files:"
$command = "cmake -S `"$sourceDir`" -B `"$buildDir`" -DCMAKE_TOOLCHAIN_FILE=`"$vcpkgToolchainFile`""
Write-Host $command
Invoke-Expression $command

Pop-Location # $PSScriptRoot
