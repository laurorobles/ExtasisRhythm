#!/bin/bash
set -e

echo "============================================================"
echo " ExtasisRhythm v4.0.0 - macOS Installer (Free & Open Source)"
echo "============================================================"
echo ""

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Locate binaries either in parent folder (release zip layout) or current folder
if [ -d "$SCRIPT_DIR/../ExtasisRhythm.vst3" ] || [ -d "$SCRIPT_DIR/../ExtasisRhythm.component" ] || [ -d "$SCRIPT_DIR/../ExtasisRhythm.app" ]; then
    SRC_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
else
    SRC_DIR="$SCRIPT_DIR"
fi

VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
APP_DIR="/Applications"

mkdir -p "$VST3_DIR" "$AU_DIR"

INSTALLED=0

if [ -d "$SRC_DIR/ExtasisRhythm.vst3" ]; then
    echo "[1/3] Installing VST3 to $VST3_DIR..."
    rm -rf "$VST3_DIR/ExtasisRhythm.vst3"
    cp -R "$SRC_DIR/ExtasisRhythm.vst3" "$VST3_DIR/"
    xattr -cr "$VST3_DIR/ExtasisRhythm.vst3" 2>/dev/null || true
    INSTALLED=1
else
    echo "[1/3] ExtasisRhythm.vst3 not found in $SRC_DIR (skipping)"
fi

if [ -d "$SRC_DIR/ExtasisRhythm.component" ]; then
    echo "[2/3] Installing Audio Unit (AU) to $AU_DIR..."
    rm -rf "$AU_DIR/ExtasisRhythm.component"
    cp -R "$SRC_DIR/ExtasisRhythm.component" "$AU_DIR/"
    xattr -cr "$AU_DIR/ExtasisRhythm.component" 2>/dev/null || true
    INSTALLED=1
else
    echo "[2/3] ExtasisRhythm.component not found in $SRC_DIR (skipping)"
fi

if [ -d "$SRC_DIR/ExtasisRhythm.app" ]; then
    echo "[3/3] Installing Standalone App to $APP_DIR..."
    if [ -w "$APP_DIR" ]; then
        rm -rf "$APP_DIR/ExtasisRhythm.app"
        cp -R "$SRC_DIR/ExtasisRhythm.app" "$APP_DIR/"
        xattr -cr "$APP_DIR/ExtasisRhythm.app" 2>/dev/null || true
    else
        sudo rm -rf "$APP_DIR/ExtasisRhythm.app"
        sudo cp -R "$SRC_DIR/ExtasisRhythm.app" "$APP_DIR/"
        sudo xattr -cr "$APP_DIR/ExtasisRhythm.app" 2>/dev/null || true
    fi
    INSTALLED=1
else
    echo "[3/3] ExtasisRhythm.app not found in $SRC_DIR (skipping)"
fi

echo ""
if [ "$INSTALLED" -eq 1 ]; then
    echo "============================================================"
    echo " Installation Complete! Open your DAW and rescan plugins."
    echo " Licensed under GPL-3.0 | Extasis Records (@laurorobles)"
    echo "============================================================"
else
    echo "ERROR: No plugin bundles found next to the installer."
    exit 1
fi
