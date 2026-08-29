# UNEQUIP QUIVER NG

A C++ SKSE plugin for Skyrim that equips a quiver when using a ranged weapon and unequips it otherwise.

https://www.nexusmods.com/skyrimspecialedition/mods/128582

---

# CommonLibSSE NG

Because this uses [CommonLibSSE NG](https://github.com/alandtse/CommonLibSSE-NG), it supports Skyrim SE and AE (also GOG and VR if enabled).

[CommonLibSSE NG](https://github.com/alandtse/CommonLibSSE-NG) is a fork of the popular [powerof3 fork](https://github.com/powerof3/CommonLibSSE) of the _original_ `CommonLibSSE` library created by [Ryan McKenzie](https://github.com/Ryan-rsm-McKenzie) in [2018](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/commit/224773c424bdb8e36c761810cdff0fcfefda5f4a).

The library is pulled in as a **vcpkg overlay port**, defined in [`cmake/ports/commonlibsse-ng`](cmake/ports/commonlibsse-ng).
It pins [alandtse/CommonLibSSE-NG](https://github.com/alandtse/CommonLibSSE-NG) `v6.7.1`, which is the actively
maintained NG fork and supports Skyrim runtimes up to `1.6.1170` / `1.6.1179` (GOG) and `1.7.99`.

> The project previously consumed `commonlibsse-ng` from the Color-Glass vcpkg registry. That registry has been
> unmaintained since 2023 and only ever shipped up to CommonLibSSE NG `3.7.0`, so it was dropped in favour of the
> overlay port. To bump CommonLibSSE NG later, update `REF` / `SHA512` in
> [`cmake/ports/commonlibsse-ng/portfile.cmake`](cmake/ports/commonlibsse-ng/portfile.cmake) and the `version-semver`
> in the port's `vcpkg.json`.

# Requirements

- [Visual Studio 2022](https://visualstudio.microsoft.com/) or newer (_the free Community edition_), with the
  **Desktop development with C++** workload (MSVC, CMake and Ninja)
- [`vcpkg`](https://github.com/microsoft/vcpkg)
  - 1. Clone the repository using git OR [download it as a .zip](https://github.com/microsoft/vcpkg/archive/refs/heads/master.zip)
  - 2. Go into the `vcpkg` folder and double-click on `bootstrap-vcpkg.bat`
  - 3. Edit your system or user Environment Variables and add a new one:
    - Name: `VCPKG_ROOT`  
      Value: `C:\path\to\wherever\your\vcpkg\folder\is`

## Opening the project

Once you have Visual Studio installed, you can open this folder in basically any C++ editor, e.g. [VS Code](https://code.visualstudio.com/) or [CLion](https://www.jetbrains.com/clion/) or [Visual Studio](https://visualstudio.microsoft.com/)
- > _for VS Code, if you are not automatically prompted to install the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions, please install those and then close VS Code and then open this project as a folder in VS Code_

You may need to click `OK` on a few windows, but the project should automatically run CMake!

It will _automatically_ download [CommonLibSSE NG](https://github.com/alandtse/CommonLibSSE-NG) and everything you need to get started.

## Building from the command line

From a **Developer PowerShell for VS** (so that `cl.exe` and `ninja` are on `PATH`):

```powershell
cmake --preset release
cmake --build build/release
```

> The first configure builds CommonLibSSE NG from source and takes several minutes.

By default, when this project compiles it will output a `.dll` for your SKSE plugin into the `build/` folder.
Set the `SKYRIM_FOLDER` or `SKYRIM_MODS_FOLDER` environment variable to have the `.dll` copied into your game or
mod manager automatically.
