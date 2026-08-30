# Extasis Rhythm (v2.6)

Extasis Rhythm is a smart, polyrhythmic, and vintage-flavored drum machine plugin (VST3 / Standalone) designed for modern music production. Inspired by the golden era of 12-bit samplers and modern generative sequencers, it provides unparalleled rhythm creation with a cutting-edge auto-tagging DSP engine.

## 🌟 What's New in Version 2.6
* **FFT Smart Auto-Tagging:** Drop any chaotic folder of samples, and the plugin will use Spectral Centroid analysis to instantly recognize if a file is a Kick, Snare, or Hi-Hat.
* **MIR JSON Cache:** Lightning-fast kit loading. FFT analysis results are cached locally so your kits load instantly the next time.
* **True 12-Bit Sample-and-Hold:** Authentic PCM sample-rate reduction algorithm mirroring the legendary E-mu SP-1200 crunch.
* **Smooth Sidechain (PUMP):** Redesigned envelope follower (attack/release) for musical and organic master-bus ducking.
* **Clickless Mutes:** Advanced smoothing envelopes for perfectly clean live muting and soloing.

## 💾 Installation

We provide pre-compiled binaries for **Windows**, **macOS**, and **Linux**. You don't need to compile any code!

### macOS
1. Go to the [Releases](https://github.com/USER/ExtasisRhythm/releases) page and download `ExtasisRhythm-macOS.zip`.
2. Extract the ZIP.
3. Move `ExtasisRhythm.vst3` to `/Library/Audio/Plug-Ins/VST3/`.
4. (Optional) Run the Standalone app directly from your Applications folder.

### Windows
1. Go to the [Releases](https://github.com/USER/ExtasisRhythm/releases) page and download `ExtasisRhythm-Windows.zip`.
2. Extract the ZIP.
3. Move `ExtasisRhythm.vst3` to `C:\Program Files\Common Files\VST3\`.

### Linux
1. Go to the [Releases](https://github.com/USER/ExtasisRhythm/releases) page and download `ExtasisRhythm-Linux.zip`.
2. Extract the ZIP and copy the `.vst3` directory to `~/.vst3/` or `/usr/lib/vst3/`.

## 🎹 Quick Start
* **Loading Kits:** Click the folder icon to select a directory containing your `.wav` files. The Smart Engine will automatically map them to the 12 channels.
* **Sequencer:** Use the 32-step polyrhythmic sequencer. Adjust the lengths of each channel individually for generative Euclidean rhythms.
* **DSP Rack:** Apply Filters, 12-bit crush, Overdrive, Reverb, Delay, and Master Pump (Sidechain) on the fly.

## 📄 License & Activation
Upon opening the plugin, you'll be prompted to enter a 16-character license key (e.g., `EXTR-XXXX-XXXX-XXXX-XXXX`).
You can obtain your license key from our Gumroad page. The validation is **100% offline**, meaning you do not need an active internet connection to authorize or use the plugin.

---
*Created by Extasis Records*
