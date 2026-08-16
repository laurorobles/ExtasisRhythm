#!/bin/bash

# Usamos rutas absolutas para evitar errores de ubicación
BASE_DIR="$(cd "$(dirname "$0")" && pwd)"
ZIP_DIR="$BASE_DIR/Samples_Zips"
OUT_DIR="$BASE_DIR/Samples"

echo "========================================="
echo " INICIANDO PROCESAMIENTO Y FILTRADO"
echo " Directorio ZIP: $ZIP_DIR"
echo "========================================="

mkdir -p "$OUT_DIR"

# Verificamos si la carpeta de zips existe y tiene contenido
if [ ! -d "$ZIP_DIR" ]; then
    echo "[ERROR] No se encuentra la carpeta Samples_Zips en $ZIP_DIR"
    exit 1
fi

# Buscamos archivos zip o ZIP de forma insensible a mayúsculas
found_zips=0
for ZIP_FILE in "$ZIP_DIR"/*.{zip,ZIP}; do
    [ -e "$ZIP_FILE" ] || continue
    found_zips=1
    
    FILENAME=$(basename "$ZIP_FILE")
    # Limpiamos el nombre quitando la extensión (.zip o .ZIP)
    KIT_NAME=$(echo "$FILENAME" | sed -E 's/\.(zip|ZIP)$//' | tr ' ' '_' | sed 's/[^a-zA-Z0-9_-]//g')

    echo "-----------------------------------------"
    echo "[PROCESANDO] Archivo: $FILENAME"
    
    TARGET_DIR="$OUT_DIR/$KIT_NAME"
    mkdir -p "$TARGET_DIR"
    
    TEMP_DIR=$(mktemp -d)
    
    # Descomprimimos mostrando errores si los hay
    unzip -q "$ZIP_FILE" -d "$TEMP_DIR" 2>/dev/null
    
    # Contamos cuántos wavs encontró en el zip temporal
    wav_count=$(find "$TEMP_DIR" -type f -iname "*.wav" | wc -l)
    echo "   -> Archivos .wav encontrados en el ZIP: $wav_count"

    # Función interna de extracción
    extract_sound() {
        local pattern=$1
        local final_name=$2
        
        local found=$(find "$TEMP_DIR" -type f -iname "*.wav" | grep -iE "$pattern" | head -n 1)
        
        if [ -n "$found" ]; then
            cp "$found" "$TARGET_DIR/$final_name.wav"
            echo "      + Asignado: $final_name.wav (desde $(basename "$found"))"
        fi
    }

    # Diccionario de búsqueda heurística
    extract_sound "kick|bd|bass" "BD"
    extract_sound "snare|sd|snr" "SD"
    extract_sound "clap|cp|cl" "CP"
    extract_sound "closed.*hat|ch|hhc|hat.*c" "CH"
    extract_sound "open.*hat|oh|hho|hat.*o" "OH"
    extract_sound "mid.*tom|tm|tom.*m" "TM"
    extract_sound "low.*tom|tl|tom.*l" "TL"
    extract_sound "hi.*tom|th|tom.*h" "TH"
    extract_sound "cowbell|cb|bell" "CB"
    extract_sound "cymbal|cy|ride|crash|crh" "CY"
    extract_sound "rim|rs" "RS"
    extract_sound "perc|pc|clave|maraca|shaker|tamb|wood" "PC"

    # Limpiamos temporal
    rm -rf "$TEMP_DIR"
done

if [ $found_zips -eq 0 ]; then
    echo "[AVISO] No se encontraron archivos .zip en $ZIP_DIR"
fi

echo "========================================="
echo " ¡PROCESAMIENTO FINALIZADO!"
echo "========================================="