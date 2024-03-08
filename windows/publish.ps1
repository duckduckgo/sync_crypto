param([string]$version, [string]$versionPostfix="")
#Requires -Version 7.0
$DebugPreference = "Continue"
$ErrorActionPreference = "Stop"

$typedVersion = [System.Version]::Parse($version)
if ($typedVersion.Revision -ne 0)
{
    Write-Debug "The 4th / revision should be always set to 0"
    exit 1
}
./build.ps1 $version $versionPostfix

$nugetVersion = "$($typedVersion.Major).$($typedVersion.Minor).$($typedVersion.Build)"
$nugetVersion = if ($versionPostfix) { "$nugetVersion-$versionPostfix" } else { $nugetVersion }
Write-Host "Publishing nuget version: $nugetVersion"
dotnet nuget push "WindowsBrowser.Sync.Crypto.Managed\bin\*\Release\WindowsBrowser.Sync.Crypto.Managed.*.$($nugetVersion).nupkg" --skip-duplicate -s azuredevopsnew -k somekey