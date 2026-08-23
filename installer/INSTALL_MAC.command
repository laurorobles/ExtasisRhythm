#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo "============================================================"
echo "🥁 EXTASIS RHYTHM — macOS Automated Installer"
echo "============================================================"

echo "📁 [1/3] Installing Sample Library to ~/Documents/ExtasisRhythm_Samples..."
mkdir -p "$HOME/Documents/ExtasisRhythm_Samples"
if [ -d "$DIR/ExtasisRhythm_Samples" ]; then
    cp -R "$DIR/ExtasisRhythm_Samples/"* "$HOME/Documents/ExtasisRhythm_Samples/"
fi

echo "🎹 [2/3] Installing VST3 Plugin to ~/Library/Audio/Plug-Ins/VST3/..."
mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"
if [ -d "$DIR/ExtasisRhythm.vst3" ]; then
    rm -rf "$HOME/Library/Audio/Plug-Ins/VST3/ExtasisRhythm.vst3"
    cp -R "$DIR/ExtasisRhythm.vst3" "$HOME/Library/Audio/Plug-Ins/VST3/"
fi

echo "💻 [3/3] Installing Standalone App to /Applications/..."
if [ -d "$DIR/ExtasisRhythm.app" ]; then
    rm -rf "/Applications/ExtasisRhythm.app"
    cp -R "$DIR/ExtasisRhythm.app" "/Applications/"
fi

echo ""
echo "============================================================"
echo "✅ Installation Complete! Extasis Rhythm is ready to use."
echo "============================================================"
