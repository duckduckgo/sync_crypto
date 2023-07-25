param([string]$version, [string]$defaultConfiguration, [string]$defaultPlatform)
#Requires -Version 7.0
$DebugPreference = "Continue"
$ErrorActionPreference = "Stop"

Write-Debug "Validating version number ($version)"
[System.Version]::Parse($version)

function Build ($configuration, $platform) {
    Write-Debug "Building sync crypto ($configuration | $platform)"
    $msbuild = ./vswhere -latest -prerelease -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe
    & $msbuild WindowsBrowser.Sync.Crypto.sln `
        /p:Configuration=$configuration `
        /p:Platform=$platform `
        /p:Version=$version
    
    If ($LastExitCode -ne 0) 
    {
        Write-Debug "Failed to build sync crypto ($configuration | $platform)"
        exit 1
    }
}

$rcFilePath = (Get-Item "WindowsBrowser.Sync.Crypto.Native\Resource.rc").FullName
$rcTemplateFilePath = (Get-Item "WindowsBrowser.Sync.Crypto.Native\Resource.rc.template").FullName
$fileContent = Get-Content -Path $rcTemplateFilePath -Raw
$newFileContent = $fileContent -replace '\$VERSION_COMMA_SEPARATED', $version.Replace(".", ",")
$newFileContent = $newFileContent -replace '\$VERSION', $version
$newFileContent = $newFileContent -replace '\$YEAR', $((Get-Date).Year)
$newFileContent | Set-Content -Path $rcFilePath -NoNewline

if ($defaultConfiguration -and $defaultPlatform) {
    Write-Debug "Building $configuration / $platform"
    Build $defaultConfiguration $defaultPlatform
}
else 
{
    Write-Debug "Building all config and platform"
    foreach ($configuration in @("Release", "Debug")) {
        foreach ($platform in @("x86", "x64")) {
            Build $configuration $platform
        }
    }
}
