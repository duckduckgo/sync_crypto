param([string]$version)
#Requires -Version 7.0
$DebugPreference = "Continue"
$ErrorActionPreference = "Stop"

$typedVersion = [System.Version]::Parse($version)
if ($typedVersion.Revision -ne 0)
{
    Write-Debug "The 4th / revision should be always set to 0"
    exit 1
}
./build.ps1 $version

$nugetVersion = "$($typedVersion.Major).$($typedVersion.Minor).$($typedVersion.Build)"
dotnet nuget push "WindowsBrowser.Sync.Crypto.Managed\bin\*\Release\WindowsBrowser.Sync.Crypto.Managed.*.$($nugetVersion).nupkg" -s azuredevopsnew -k somekey