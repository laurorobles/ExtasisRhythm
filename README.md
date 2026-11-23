# Extasis Rhythm (v4.0.0 — Free & Open Source)

**Extasis Rhythm** is a **100% Free & Open Source (GNU GPLv3)** polyrhythmic, vintage-flavored modular drum machine plugin (VST3 / Standalone) designed for modern music production. Inspired by the crunch of golden-era 12-bit samplers and the generative flexibility of modern sequencers, it provides expressive rhythm creation through a custom C++/JUCE DSP engine.

Whether you are producing Lo-Fi, House, Techno, or Hip-Hop, Extasis Rhythm gives you complete control over every element of your beat with a full suite of built-in modules:

* **🎛️ 12-Channel Modular Sampler + Auto-Trim:** Drag & drop your own `.wav` or `.aif` files directly onto any channel, or let the FFT Smart Engine automatically analyze and map an entire folder of samples. Built-in **Smart Auto-Trim** detects and strips leading silence (with zero-crossing protection) so every drum hit lands tightly on the transient.
* **🧬 Polyrhythmic Sequencer:** 12 independent sequencer lanes with custom step lengths for Euclidean rhythms, 8 pattern slots, per-step pitch/glide, and Smart Fill Memory (MPC / Pocket Operator style).
* **🎚️ Advanced Channel Strips:** Sculpt each voice individually with Pitch, Tone, Attack, Decay, Pan, and dedicated FX Sends (Delay, Spring Reverb), along with clickless Mute/Solo.
* **📼 Vintage FX Rack:** Authentic 12-Bit and 8-Bit PCM sample-rate reduction (SP-1200 style), classic analog Chorus, Flanger, syncable Bucket-Brigade Delay, and a vintage Spring Reverb module.
* **🔥 Master Bus & Saturation:** Glue your mix together with a dedicated Overdrive circuit, DJ-style Master HPF/LPF with resonance, Analog/Vinyl modes, Anti-Aliasing, and a Master Clipper & Limiter.
* **🫁 PUMP (Envelope Ducking):** An organic, smooth master-bus envelope follower to add rhythmic pumping glue instantly—no complex DAW routing required.
* **🚀 Instant Drag-to-DAW Export:** Generate a sample-accurate bounced loop in milliseconds! Just drag the `DRAG WAV` button directly into your DAW's timeline or your Desktop.

## 🌟 What's New in Version 4.0.0
* **Free & Open Source (GNU GPLv3):** Completely free forever—no license keys, no demo timers, no restrictions.
* **Smart Auto-Trim on Sample Load:** Automatically removes leading silence on every loaded or dragged sample while preserving the full attack transient.
* **Upgraded Hardware GUI (60 FPS):** Luminous value arcs on all rotary knobs, metallic cap shading, double-click to reset defaults, 4-step beat grouping in the sequencer, dark hardware module headers, and glowing LEDs.
* **DSP Routing Improvements:** True Kick-triggered **PUMP** sidechain (ducks percussion & FX returns while keeping the Kick punchy), dedicated Vinyl HPF filter, accurate post-gain Master Limiter ceiling (-0.2 dBFS), and transparent Overdrive bypass when Drive is at 0.
* **Sample-Accurate Offline Drag-to-WAV:** Clean, feedback-free offline loop bouncing.

## 💾 Installation

Pre-compiled binaries are available for **Windows**, **macOS**, and **Linux** on the [Releases](https://github.com/laurorobles/ExtasisRhythm/releases) page.

### macOS
1. Download `ExtasisRhythm-macOS.zip` (or `ExtasisRhythm-macOS-con-Samples.zip`).
2. Extract the ZIP.
3. Move `ExtasisRhythm.vst3` to `/Library/Audio/Plug-Ins/VST3/`.
4. (Optional) Move `ExtasisRhythm.app` to `/Applications/`.

> **⚠️ macOS Gatekeeper Notice:** If macOS warns that the app is "Damaged" or from an unidentified developer, open Terminal, drag `Fix_Mac_Error.command` (included in the ZIP) into the Terminal window, and press **Enter** to remove the quarantine attribute.

### Windows
1. Download `ExtasisRhythm-Windows.zip`.
2. Extract the ZIP and move `ExtasisRhythm.vst3` to `C:\Program Files\Common Files\VST3\`.

### Linux
1. Download `ExtasisRhythm-Linux.zip`.
2. Extract the ZIP and copy `ExtasisRhythm.vst3` to `~/.vst3/` or `/usr/lib/vst3/`.

## 🛠️ Building from Source

### Prerequisites
- CMake 3.22+
- A C++17 compiler (Xcode / Clang on macOS, Visual Studio 2022 on Windows, GCC/Clang on Linux)
- Git (for the JUCE submodule)

```bash
git clone --recursive https://github.com/laurorobles/ExtasisRhythm.git
cd ExtasisRhythm
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
```

On Ubuntu/Debian Linux, install the JUCE dependencies first:
```bash
sudo apt-get install -y libasound2-dev libx11-dev libxinerama-dev libxext-dev \
  libfreetype6-dev libwebkit2gtk-4.0-dev libglu1-mesa-dev libjack-jackd2-dev libcurl4-openssl-dev
```

## 📄 License

**Extasis Rhythm** is licensed under the **GNU General Public License v3.0 (GPLv3)**. See [LICENSE](LICENSE) for full details.

---
*Created by [@laurorobles](https://github.com/laurorobles) / Extasis Records*
