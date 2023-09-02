Push-Location $PSScriptRoot
Write-Host "Current working directory: $(Get-Location)"

$vcpkgRoot = $Env:VCPKG_ROOT
if ($null -eq $vcpkgRoot -or !(Test-Path $vcpkgRoot -PathType Container)) {
    Write-Host "radcpp: You need to setup vcpkg and set `$Env:VCPKG_ROOT to its path."
    Write-Host "radcpp: Please refer to: https://vcpkg.io/en/getting-started.html"
    return
}

$vcpkgPackages = @(
    "boost"
    "gtest"
    "opencl"
    "glm"
)

if ($IsWindows -eq $null -and $Env:OS.StartsWith("Windows")) {
    # $IsWindows was added in PowerShell Core 6 and PowerShell 7.
    $IsWindows = $true
}

$vcpkgTriplet = $null
if ($IsWindows) {
    $vcpkgTriplet = "x64-windows"
}

$vcpkgExecutable = "vcpkg"
if ($IsWindows) {
    $vcpkgExecutable = "vcpkg.exe"
}
$vcpkgExecutable = Join-Path -Path $vcpkgRoot -ChildPath $vcpkgExecutable
if (Test-Path -Path $vcpkgExecutable -PathType Leaf) {
    Write-Host "Found vcpkg executable: $vcpkgExecutable"
} else {
    Write-Host "vcpkg executable does not exist!"
    return
}

Push-Location $vcpkgRoot
Write-Host "Current working directory: $(Get-Location)"
Write-Host "Updating vcpkg:"
Invoke-Expression "git pull"
foreach ($package in $vcpkgPackages) {
    Write-Host "Installing vcpkg package $package`:"
    if ($null -ne $vcpkgTriplet) {
        Invoke-Expression "& `"$vcpkgExecutable`" install $package`:$vcpkgTriplet"
    } else {
        Invoke-Expression "& `"$vcpkgExecutable`" install $package"
    }
}
Pop-Location

Pop-Location
