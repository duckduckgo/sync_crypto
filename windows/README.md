# Windows support


## Libsodium

To update the libsodium headers and binaries, build the [stable branch](https://github.com/jedisct1/libsodium/tree/stable/builds/msvc/vs2022) in all platforms and configurations. The Visual Studio solution is located in `.\builds\msvc`. Commit the appropriate artifacts (e.g. `native_lib\third-party\windows\Win32\Debug\v143\dynamic\..`).

The currently committed binaries are for version 1.0.19, built from tag `1.0.19-RELEASE`, commit `https://github.com/jedisct1/libsodium/commit/fb4533b0a941b3a5b1db5687d1b008a5853d1f29`.

## WindowsBrowser.Sync.Crypto.Native

This is the VC++ project for building agains libsodium and sync_cryto.

## WindowsBrowser.Sync.Crypto.Managed

This is the .NET project, which contains the P/Invoke wrappers to be able to consume the native dll.

The project is generating NuGet packages on build.

## Updating the managed wrapper

0. Go to the `windows` folder
1. Setup NuGet publishing on your machine. See how to [setup the key](https://app.asana.com/0/0/1205079183137069/f).

```
dotnet nuget add source https://pkgs.dev.azure.com/ddgwindows/windows-nuget/_packaging/windows-nuget-feed/nuget/v3/index.json -n azuredevopsnew -u nugetpush -p THEKEY
```

2. Open PowerShell and make sure you use PowerShell 7 (call `pwsh`)
3. `.\publish.ps1 1.2.3.0`. You can also supply a version postfix to publish a prerelease version of the packages: `.\publish.ps1 1.2.3.0 test`. This will publish the packages with version `1.2.3-test`.