#!/bin/bash
set -e

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$REPO_DIR"

echo "================================================="
echo "🥁 Packaging Extasis Rhythm for macOS..."
echo "================================================="

# 1. Build latest release binaries
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

# 2. Setup package directory
PKG_DIR="$REPO_DIR/dist/ExtasisRhythm-macOS"
rm -rf "$REPO_DIR/dist"
mkdir -p "$PKG_DIR"

# 3. Copy binaries
if [ -d "$REPO_DIR/build/ExtasisRhythm_artefacts/VST3/ExtasisRhythm.vst3" ]; then
    cp -r "$REPO_DIR/build/ExtasisRhythm_artefacts/VST3/ExtasisRhythm.vst3" "$PKG_DIR/"
fi

if [ -d "$REPO_DIR/build/ExtasisRhythm_artefacts/Standalone/ExtasisRhythm.app" ]; then
    cp -r "$REPO_DIR/build/ExtasisRhythm_artefacts/Standalone/ExtasisRhythm.app" "$PKG_DIR/"
fi

# 4. Copy samples, manual, and installer
if [ -d "$REPO_DIR/ExtasisRhythm_Samples" ]; then
    cp -r "$REPO_DIR/ExtasisRhythm_Samples" "$PKG_DIR/"
fi

if [ -f "$REPO_DIR/installer/INSTALL_MAC.command" ]; then
    cp "$REPO_DIR/installer/INSTALL_MAC.command" "$PKG_DIR/"
    chmod +x "$PKG_DIR/INSTALL_MAC.command"
fi

if [ -f "$REPO_DIR/MANUAL.md" ]; then
    cp "$REPO_DIR/MANUAL.md" "$PKG_DIR/"
fi

# 5. Create zip archive
cd "$REPO_DIR/dist"
zip -r -q -y "ExtasisRhythm-macOS.zip" "ExtasisRhythm-macOS"

echo "================================================="
echo "✅ Package created successfully!"
echo "📦 Archive: $REPO_DIR/dist/ExtasisRhythm-macOS.zip"
echo "================================================="
