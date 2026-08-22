# 🥁 Extasis Rhythm (v2.0)

<p align="center">
  <img src="assets/logo.png" alt="Extasis Rhythm Logo" width="220" />
</p>

<p align="center">
  <strong>Hybrid Vintage Drum Machine & Multi-FX Polyrhythmic Groovebox Workstation</strong><br>
  <em>Built with JUCE 7 (C++20) for macOS (Universal Binary) & Windows (x64).</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-2.0.0-blue.svg" alt="Version 2.0" />
  <img src="https://img.shields.io/badge/Platform-macOS%20%7C%20Windows-lightgrey.svg" alt="Platform" />
  <img src="https://img.shields.io/badge/Format-VST3%20%7C%20Standalone-orange.svg" alt="Format" />
  <img src="https://img.shields.io/badge/License-Commercial%20%2F%20Open%20Source-green.svg" alt="License" />
</p>

---

<p align="center">
  <img src="assets/screenshot.png" alt="Extasis Rhythm Interface" width="900" style="border-radius: 8px; box-shadow: 0 4px 16px rgba(0,0,0,0.4);" />
</p>

---

## ✨ Overview

**Extasis Rhythm** combines the raw, tactile grit of legendary 80s/90s hardware beatboxes (*Roland TR-808/TR-909, E-mu SP-1200, Akai MPC60*) with modern polymetric sequencing (*Elektron Octatrack/Digitakt*) and a dedicated vintage studio effect rack.

### 🌟 Key Highlights:
- **12 Independent Drum Voices**: Kick, Snare, Closed Hat, Open Hat, Clap, Rimshot, Hi Perc, Mid Perc, Low Perc, Cowbell, Crash, Ride.
- **Hermite Cubic Resampling Engine**: 4-point continuous polynomial interpolation for artifact-free pitch transposition ($\pm 24$ semitones).
- **Polyrhythmic Step Sequencer**:
  - Independent track lengths (1 to 32 steps).
  - Metric scaling modes: **`FIX`** (standard 16th grid) and **`FIT`** (polyrhythmic metric compression over 4/4 bars).
  - Playback directions per channel: **`FWD`** (Forward), **`REV`** (Reverse), **`RND`** (Random generative), **`PNB`** (Ping-Pong / Pendulum).
  - Per-step multi-velocity, pitch transposition locks, and portamento **`Glide`**.
- **Dedicated 16-Step Fill Engine**: Immediate transitions and roll injection on the fly.
- **Vintage Multi-FX Rack**:
  - **Filter**: Dual resonant State-Variable DJ Filter (HPF / LPF).
  - **PCM Crunch**: Variable bit depth (16 to 4-bit) & clock downsampling (6.25 to 100 kHz).
  - **Overdrive**: Asymmetric non-linear soft saturation with tone filter.
  - **Transient Shaper**: Punch and sustain sculpting.
  - **Dynamic Envelope Filter**: Auto-Wah follower with vocal resonance.
  - **Pump Compressor**: Sidechain bus ducker.
  - **Analog Flanger & Stereo CE Chorus**: Quadrature LFO modulation with zero click artifacts.
  - **Modulated Tape Delay & Spring Reverb Tank**: Analog-modeled allpass network with soft-clipping.
- **Master Bus & Metering**:
  - Brickwall ceiling limiter (-0.2 dBFS), Tape soft clipper, Vinyl simulator, Real-time CPU monitor, and 14-segment stereo RMS/Peak LED ladder.

---

## 📖 Official Documentation & Manual

For full technical specifications, knob mappings, and parameter descriptions, see the **[Official User Manual (MANUAL.md)](MANUAL.md)**.

---

## 📥 Download Pre-Compiled Binaries

Official pre-compiled installers (**VST3** and **Standalone**) are available for macOS (Apple Silicon & Intel) and Windows (x64):

- **Releases & Builds**: Check the **[GitHub Releases](https://github.com/laurorobles/ExtasisRhythm/releases)** or **[GitHub Actions Artifacts](https://github.com/laurorobles/ExtasisRhythm/actions)** tab.
- **Official Store & Sound Packs**: [extasisrecords.bandcamp.com](https://extasisrecords.bandcamp.com)

---

## 🛠️ Building from Source

### Prerequisites:
- **CMake 3.22+**
- **C++20 Compiler**: Xcode Clang (macOS) or Visual Studio 2022 / MSVC (Windows)
- **Ninja** (optional, recommended for fast builds)

### Build Instructions:

```bash
# 1. Clone the repository
git clone --recursive https://github.com/laurorobles/ExtasisRhythm.git
cd ExtasisRhythm

# 2. Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Build VST3 and Standalone targets
cmake --build build --config Release --parallel
```

The compiled plugins will be generated in:
- macOS: `build/ExtasisRhythm_artefacts/Release/VST3/Extasis Rhythm.vst3`
- Windows: `build/ExtasisRhythm_artefacts/Release/VST3/Extasis Rhythm.vst3`

---

---

## 🔑 License Activation & Support

Extasis Rhythm features an offline cryptographic license verification system. 
Upon purchase from our official store or partner marketplaces, you will receive your personal 16-character license key (`EXTR-XXXX-XXXX-XXXX-XXXX`) to unlock full permanent access.

- **Official Store**: [extasisrecords.bandcamp.com](https://extasisrecords.bandcamp.com)
- **Support & Inquiries**: Contact through Extasis Records Bandcamp.

---

## 👥 Credits & Contact

- **DSP Architecture & Development**: Lauro Robles
- **Label & Releases**: [Extasis Records](https://extasisrecords.bandcamp.com)
