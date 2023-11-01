Push-Location $PSScriptRoot
Write-Host "Current working directory: $(Get-Location)"

$repos = @(
    [PSCustomObject]@{
        Name = "spvgen"
        URL = "https://github.com/JerryMa-SDE/spvgen.git"
    },
    [PSCustomObject]@{
        Name = "Compressonator"
        URL = "https://github.com/JerryMa-SDE/Compressonator.git"
    }
)

foreach ($repo in $repos)
{
    if (-not (Test-Path -Path $repo.Name -PathType Container))
    {
        Write-Host "$($repo.Name) doesn't exist, cloning from $($repo.URL) ..."
        Invoke-Expression "git clone $($repo.URL) $($repo.Name)"
    }
    
    if (Test-Path -Path $repo.Name -PathType Container)
    {
        Write-Host "$($repo.Name) exists, updating ..."
        Push-Location $repo.Name
        Invoke-Expression "git fetch"
        Invoke-Expression "git pull"
        Pop-Location
    }
}

Pop-Location
