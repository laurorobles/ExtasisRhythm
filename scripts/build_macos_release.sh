#!/bin/bash
set -e

echo "🚀 Compilando versión de macOS (Release)..."
cmake -B build_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_release --config Release --parallel 4

echo "📦 Preparando archivos..."
rm -rf ~/Desktop/ExtasisRhythm-macOS ~/Desktop/ExtasisRhythm-macOS-con-Samples

# Función para crear el script de desbloqueo y el INSTALL.txt mejorado
preparar_carpeta() {
    local TARGET_DIR=$1
    
    # 1. Crear Documentación
    mkdir -p "$TARGET_DIR/Documentation"
    cp MANUAL.md README.md TECHNICAL.md "$TARGET_DIR/Documentation/"
    
    # 2. Copiar App y VST3
    cp -R build_release/ExtasisRhythm_artefacts/Release/VST3/ExtasisRhythm.vst3 "$TARGET_DIR/"
    cp -R build_release/ExtasisRhythm_artefacts/Release/Standalone/ExtasisRhythm.app "$TARGET_DIR/"
    
    # 3. Crear el script mágico de desbloqueo (Fix_Mac_Error.command)
    cat << 'INNER_EOF' > "$TARGET_DIR/Fix_Mac_Error.command"
#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo "=========================================================="
echo "      Extasis Rhythm - macOS Gatekeeper Unlocker"
echo "=========================================================="
echo "Reparando permisos y removiendo cuarentena de Apple..."
echo ""
xattr -cr "$DIR/ExtasisRhythm.app" 2>/dev/null || true
xattr -cr "$DIR/ExtasisRhythm.vst3" 2>/dev/null || true
xattr -cr "/Applications/ExtasisRhythm.app" 2>/dev/null || true
xattr -cr "$HOME/Applications/ExtasisRhythm.app" 2>/dev/null || true
xattr -cr "/Library/Audio/Plug-Ins/VST3/ExtasisRhythm.vst3" 2>/dev/null || true
xattr -cr "$HOME/Library/Audio/Plug-Ins/VST3/ExtasisRhythm.vst3" 2>/dev/null || true
echo "✅ ¡Exito! Ya puedes mover el VST3 a tu carpeta de plugins"
echo "✅ y abrir la App Standalone sin que macOS la bloquee."
echo ""
echo "Puedes cerrar esta ventana de terminal."
echo "=========================================================="
INNER_EOF
    chmod +x "$TARGET_DIR/Fix_Mac_Error.command"

    # 4. Crear el INSTALL.txt explicando esto
    cat << 'INNER_EOF' > "$TARGET_DIR/INSTALL.txt"
EXTASIS RHYTHM v3.0.7 — macOS Installation
==========================================

⚠️ IMPORTANTE: SI MAC DICE QUE LA APP "ESTÁ DAÑADA" ⚠️
Apple bloquea los plugins descargados de internet (Gatekeeper).
Para solucionarlo en 1 segundo:
1. Abre la aplicación "Terminal" en tu Mac.
2. Arrastra el archivo "Fix_Mac_Error.command" hacia la ventana de la Terminal.
3. Presiona Enter.
4. La terminal confirmará el desbloqueo. ¡Listo!

------------------------------------------
INSTALACIÓN NORMAL:
1. VST3: Mueve "ExtasisRhythm.vst3" a /Library/Audio/Plug-Ins/VST3/
2. Standalone: Mueve "ExtasisRhythm.app" a tus Aplicaciones.

Created by Extasis Records
INNER_EOF
}

# --- ZIP 1: macOS NORMAL ---
mkdir -p ~/Desktop/ExtasisRhythm-macOS
preparar_carpeta ~/Desktop/ExtasisRhythm-macOS

# Empaquetar usando ditto (Preserva perfectamente los permisos y symlinks de macOS)
cd ~/Desktop
rm -f ExtasisRhythm-macOS.zip
ditto -c -k --sequesterRsrc --keepParent ExtasisRhythm-macOS ExtasisRhythm-macOS.zip
echo "✅ ExtasisRhythm-macOS.zip creado"
cd - > /dev/null

# --- ZIP 2: macOS CON SAMPLES ---
mkdir -p ~/Desktop/ExtasisRhythm-macOS-con-Samples
preparar_carpeta ~/Desktop/ExtasisRhythm-macOS-con-Samples
cp -R ExtasisRhythm_Samples ~/Desktop/ExtasisRhythm-macOS-con-Samples/

cd ~/Desktop
rm -f ExtasisRhythm-macOS-con-Samples.zip
ditto -c -k --sequesterRsrc --keepParent ExtasisRhythm-macOS-con-Samples ExtasisRhythm-macOS-con-Samples.zip
echo "✅ ExtasisRhythm-macOS-con-Samples.zip creado"
cd - > /dev/null

echo "🎉 ¡Todo listo en el Escritorio! ZIPs 100% seguros para Mac."
