# Windows support


## Libsodium

To update the libsodium headers and binaries, build the [stable branch](https://github.com/jedisct1/libsodium/tree/stable/builds/msvc/vs2022) in all platforms and configurations. Commit the appropriate artifacts (e.g. `native_lib\third-party\windows\Win32\Debug\v143\dynamic\..`)

## WindowsBrowser.Sync.Crypto.Native

This is the VC++ project for building agains libsodium and sync_cryto.

## WindowsBrowser.Sync.Crypto.Managed

This is the .NET project, which contains the P/Invoke wrappers to be able to consume the native dll.

The project is generating NuGet packages on build.

## Updating the managed wrapper

0. Go to the `windows` folder
1. Setup NuGet publishing on your machine. See how to [setup the key](https://app.asana.com/0/0/1205079183137069/f).

```
dotnet nuget add source https://pkgs.dev.azure.com/ddgwindows/windows-nuget/_packaging/windows-nuget-feed/nuget/v3/index.json -n azuredevopsnew -u nugetpush -p THEKEY`
```

2. Open PowerShell and make sure you use PowerShell 7 (call `pwsh`)
3. `./build.ps1 $versionNumber` for example `./build.ps1 "1.1.0.0"`
4. Push the NuGet packages under `windows\WindowsBrowser.Sync.Crypto.Managed\bin\{x64|x86}\Release`

```
dotnet nuget push -s azuredevopsnew *.nupkg -k somekey
```

Use `-k somekey` as-is, the tool actually picking up the creds from other place.
