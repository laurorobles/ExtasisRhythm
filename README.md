# Extasis Rhythm (v3.0.2)

Extasis Rhythm is a smart, polyrhythmic, and vintage-flavored drum machine plugin (VST3 / Standalone) designed for modern music production. Inspired by the golden era of 12-bit samplers and modern generative sequencers, it provides unparalleled rhythm creation with a cutting-edge DSP engine.

## 🌟 What's New in Version 3.0.2
* **Smart Fill Memory (MPC / Pocket Operator Style):** Completely rewritten Fill engine. The sequencer now intelligently remembers your last state when exiting a Fill, ensuring a seamless transition back to your original rhythm without dropping the groove.
* **Cleaner UI:** The purchase/license link has been moved exclusively to the Serial Activation window, giving you a cleaner, distraction-free interface.
* **Cross-Platform Stability:** Resolved algorithm edge cases that caused crashes on certain Windows and Linux host DAWs.
* **Preset Engine:** Fixed missing preset declarations for more reliable preset loading and saving.
* **FFT Smart Auto-Tagging:** Drop any chaotic folder of samples, and the plugin will use Spectral Centroid analysis to instantly recognize Kicks, Snares, and Hi-Hats.
* **True 12-Bit Sample-and-Hold:** Authentic PCM sample-rate reduction algorithm mirroring the legendary E-mu SP-1200 crunch.
* **Smooth Sidechain (PUMP):** Redesigned envelope follower (attack/release) for musical and organic master-bus ducking.
* **Clickless Mutes:** Advanced smoothing envelopes for perfectly clean live muting and soloing.

## 💾 Installation

We provide pre-compiled binaries for **Windows**, **macOS**, and **Linux**. You don't need to compile any code!

### macOS
1. Go to the [Releases](https://github.com/laurorobles/ExtasisRhythm/releases) page and download `ExtasisRhythm-macOS.zip`.
2. Extract the ZIP.
3. Move `ExtasisRhythm.vst3` to `/Library/Audio/Plug-Ins/VST3/`.
4. (Optional) Run the Standalone `ExtasisRhythm.app` directly.

### Windows
1. Go to the [Releases](https://github.com/laurorobles/ExtasisRhythm/releases) page and download `ExtasisRhythm-Windows.zip`.
2. Extract the ZIP.
3. Move `ExtasisRhythm.vst3` to `C:\Program Files\Common Files\VST3\`.

### Linux
1. Go to the [Releases](https://github.com/laurorobles/ExtasisRhythm/releases) page and download `ExtasisRhythm-Linux.zip`.
2. Extract the ZIP and copy the `.vst3` directory to `~/.vst3/` or `/usr/lib/vst3/`.

## 🎹 Quick Start
* **Loading Kits:** Click the folder icon to select a directory containing your `.wav` files. The Smart Engine will automatically map them to the 8 channels.
* **Sequencer:** Use the 16-step polyrhythmic sequencer. Adjust the lengths of each channel individually for generative Euclidean rhythms.
* **DSP Rack:** Apply Filters, 12-bit crush, Overdrive, Reverb, Delay, and Master Pump (Sidechain) on the fly.

## 📄 License & Activation
Upon opening the plugin, you'll be prompted to enter a 16-character license key (e.g., `EXTR-XXXX-XXXX-XXXX-XXXX`).
You can obtain your license key from our Gumroad page. The validation is **100% offline**, meaning you do not need an active internet connection to authorize or use the plugin.

---
*Created by Extasis Records*
