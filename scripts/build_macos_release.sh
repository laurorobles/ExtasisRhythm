#!/bin/bash
set -e

echo "🚀 Compilando versión de macOS (Release)..."
cmake -B build_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_release --config Release --parallel 4

echo "📦 Empaquetando ZIPs..."
rm -rf ~/Desktop/ExtasisRhythm-macOS ~/Desktop/ExtasisRhythm-macOS-con-Samples

# --- ZIP 1: macOS NORMAL ---
mkdir -p ~/Desktop/ExtasisRhythm-macOS/Documentation
cp MANUAL.md README.md TECHNICAL.md ~/Desktop/ExtasisRhythm-macOS/Documentation/
cp -R build_release/ExtasisRhythm_artefacts/Release/VST3/ExtasisRhythm.vst3 ~/Desktop/ExtasisRhythm-macOS/
cp -R build_release/ExtasisRhythm_artefacts/Release/Standalone/ExtasisRhythm.app ~/Desktop/ExtasisRhythm-macOS/

cd ~/Desktop
rm -f ExtasisRhythm-macOS.zip
zip -r ExtasisRhythm-macOS.zip ExtasisRhythm-macOS/ > /dev/null
echo "✅ ExtasisRhythm-macOS.zip creado"
cd - > /dev/null

# --- ZIP 2: macOS CON SAMPLES ---
mkdir -p ~/Desktop/ExtasisRhythm-macOS-con-Samples/Documentation
cp MANUAL.md README.md TECHNICAL.md ~/Desktop/ExtasisRhythm-macOS-con-Samples/Documentation/
cp -R build_release/ExtasisRhythm_artefacts/Release/VST3/ExtasisRhythm.vst3 ~/Desktop/ExtasisRhythm-macOS-con-Samples/
cp -R build_release/ExtasisRhythm_artefacts/Release/Standalone/ExtasisRhythm.app ~/Desktop/ExtasisRhythm-macOS-con-Samples/
cp -R ExtasisRhythm_Samples ~/Desktop/ExtasisRhythm-macOS-con-Samples/

cd ~/Desktop
rm -f ExtasisRhythm-macOS-con-Samples.zip
zip -r ExtasisRhythm-macOS-con-Samples.zip ExtasisRhythm-macOS-con-Samples/ > /dev/null
echo "✅ ExtasisRhythm-macOS-con-Samples.zip creado"
cd - > /dev/null

echo "🎉 ¡Todo listo en el Escritorio!"
