# Series 500 Modular Rack

JUCE/C++ modular audio effects rack. The current rack holds up to 16 serial modules and a dedicated master bus compressor. Each slot has its own enable/bypass control, and the full rack state is saved through the host's plug-in state.

## Included processing modules

- Preamp with SSL, Neve and API-style saturation models
- Console EQ and four-band parametric EQ
- Compressor, gate, de-esser, limiter and bus compressor
- Saturation, filters, stereo width, delay and reverb

The DSP is implemented in `Source/Modules.cpp`; the rack, slot ordering, parameter state and presets are in `Source/PluginProcessor.cpp`.

## Requirements

- CMake 3.22 or newer
- C++17 compiler
- JUCE 8.0.15 (downloaded by CMake FetchContent)
- macOS: Xcode command line tools
- Windows: Visual Studio 2022 with the Desktop development with C++ workload

## Build on macOS

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

This builds VST3, AU and Standalone. To build a universal Apple Silicon/Intel binary:

```sh
cmake -S . -B build-universal -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build-universal --config Release --parallel
```

## Build on Windows

Run from a Visual Studio 2022 Developer PowerShell:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
```

Windows builds VST3 and Standalone. AU is enabled only on Apple platforms.

## Output locations

- macOS VST3: `build/RackStrip_artefacts/Release/VST3/Series 500 Modular Rack.vst3`
- macOS AU: `build/RackStrip_artefacts/Release/AU/Series 500 Modular Rack.component`
- Standalone: `build/RackStrip_artefacts/Release/Standalone/Series 500 Modular Rack.app`
- Windows VST3: `build/RackStrip_artefacts/Release/VST3/Series 500 Modular Rack.vst3`
- Windows Standalone: `build/RackStrip_artefacts/Release/Standalone/Series 500 Modular Rack.exe`

Plug-in copying into system folders is disabled by default. Enable it explicitly with `-DRACK_COPY_PLUGIN_AFTER_BUILD=ON` if desired.

## State and bypass

Parameters are registered in JUCE's `AudioProcessorValueTreeState` and serialized by `getStateInformation` / `setStateInformation`. Each slot has a host-visible `On` parameter and a matching front-panel switch. The global bypass is separate. Rack order, module type and module parameters are stored as host state.
