param([string]$version, [string]$defaultConfiguration, [string]$defaultPlatform)
#Requires -Version 7.0
$DebugPreference = "Continue"
$ErrorActionPreference = "Stop"

Write-Debug "Validating version number ($version)"
[System.Version]::Parse($version)

function EnsureVisualStudioDeveloperEnvironment 
{
    $msbuild = "MSBuild.exe"
    Get-Command $msbuild -ErrorAction SilentlyContinue -ErrorVariable msbuildNotFoundError
    if ($msbuildNotFoundError)
    {
        $programFilesDirectories = ${Env:ProgramFiles(x86)}, ${Env:ProgramFiles}
        $vsEditions = "Enterprise", "Community", "Professional", "Preview"

        :pathLookupLoop foreach ($programFilesDir in $programFilesDirectories) {
            foreach ($vsEdition in $vsEditions) {
                if (Test-Path "$($programFilesDir)\Microsoft Visual Studio\2022\$($vsEdition)\Common7\Tools") {
                    Push-Location "$($programFilesDir)\Microsoft Visual Studio\2022\$($vsEdition)\Common7\Tools"
                    break pathLookupLoop
                }
            }
        }

        cmd /c "VsDevCmd.bat&set" |
        ForEach-Object {
            if ($_ -match "=") {
                $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
            }
        }
        Pop-Location
        Write-Host "`nVisual Studio 2022 Command Prompt variables set." -ForegroundColor Yellow
    }
    
    $msbuildPath = (Get-Command $msbuild).Source
    Write-Debug "Building with MSBuild from $msbuildPath"
}

function Build ($configuration, $platform) {
    Write-Debug "Building sync crypto ($configuration | $platform)"

    & 'MSBuild.exe' SyncCrypto.sln `
        /p:Configuration=$configuration `
        /p:Platform=$platform `
    
    If ($LastExitCode -ne 0) 
    {
        Write-Debug "Failed to build sync crypto ($configuration | $platform)"
        exit 1
    }
}

EnsureVisualStudioDeveloperEnvironment

$rcFilePath = (Get-Item "Resource.rc").FullName
$fileContent = Get-Content -Path $rcFilePath -Raw
$fileContent = $fileContent -replace "(?<=VALUE `"`FileVersion`"`,.+`,\s+`")((\d+,){3}\d+)", $version
$fileContent = $fileContent -replace "(?<=VALUE `"`ProductVersion`"`,.+`,\s+`")((\d+,){3}\d+)", $version
$fileContent | Set-Content -Path $rcFilePath

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


