# WGCOpenCVInterop
A small demo on how to interop Windows.Graphics.Capture (really Direct3D11) with OpenCV.

## Building
This demo uses [`vcpkg`](https://github.com/microsoft/vcpkg) to generate a Nuget package to reference OpenCV properly in Visual Studio. You can reproduce this by doing the following:

```
> git clone https://github.com/microsoft/vcpkg
> cd vcpkg
> bootstrap-vcpkg.bat
> vcpkg install opencv:x64-windows
> vcpkg export opencv:x64-windows --nuget
```

Then reference a custom [Nuget source](https://learn.microsoft.com/en-us/nuget/consume-packages/install-use-packages-visual-studio#package-sources) containing the resulting Nuget package (e.g. a folder somewhere on your computer).
