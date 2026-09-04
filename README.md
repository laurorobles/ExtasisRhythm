# Extasis Rhythm (v3.1.0)

Extasis Rhythm is a smart, polyrhythmic, and vintage-flavored drum machine plugin (VST3 / Standalone) designed for modern music production. Inspired by the golden era of 12-bit samplers and modern generative sequencers, it provides unparalleled rhythm creation with a cutting-edge DSP engine.

## 🌟 What's New in Version 3.1.0
* **Modular Sampler (Drag & Drop):** You can now drag and drop individual `.wav` or `.aif` files directly onto any of the 12 channels to build your custom drum kits on the fly.
* **Drag to DAW (Offline Render):** Instantly export your current pattern! Simply click and drag the new 〰️ WAV button to your DAW's timeline (Ableton, FL Studio) or your Desktop to instantly bounce a 16-second loop.
* **Collect & Save Custom Kits:** The new `COLLECT KIT` button gathers all your randomly dragged samples, organizes them, renames them, and permanently saves them to a new user preset folder.
* **Robust Core Engine:** Implemented thread-safe `thread_local` memory locks for the UI and DSP threads, ensuring absolutely zero glitches or race conditions when doing offline renders or dragging files.
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

> **⚠️ macOS Gatekeeper Notice:** If macOS tells you the app is "Damaged" and should be moved to the Trash, this is a standard Apple security feature for indie plugins. Simply open the Terminal app, drag the `Fix_Mac_Error.command` file (included in the ZIP) into the Terminal window, and press **Enter**. This will safely bypass the quarantine and allow the plugin to run.

### Windows
1. Go to the [Releases](https://github.com/laurorobles/ExtasisRhythm/releases) page and download `ExtasisRhythm-Windows.zip`.
2. Extract the ZIP.
3. Move `ExtasisRhythm.vst3` to `C:\Program Files\Common Files\VST3\`.

### Linux
1. Go to the [Releases](https://github.com/laurorobles/ExtasisRhythm/releases) page and download `ExtasisRhythm-Linux.zip`.
2. Extract the ZIP and copy the `.vst3` directory to `~/.vst3/` or `/usr/lib/vst3/`.

## 🎹 Quick Start
* **Loading Kits:** Click the folder icon to select a directory, or simply Drag & Drop your own `.wav` files onto individual channels to build a modular kit. The Smart Engine handles the mapping.
* **Sequencer:** Use the 16-step polyrhythmic sequencer. Adjust the lengths of each channel individually for generative Euclidean rhythms.
* **DSP Rack:** Apply Filters, 12-bit crush, Overdrive, Reverb, Delay, and Master Pump (Sidechain) on the fly.

## 📄 License & Activation
Upon opening the plugin, you'll be prompted to enter a 16-character license key (e.g., `EXTR-XXXX-XXXX-XXXX-XXXX`).
You can obtain your license key from our Gumroad page. The validation is **100% offline**, meaning you do not need an active internet connection to authorize or use the plugin.

---
*Created by Extasis Records*
