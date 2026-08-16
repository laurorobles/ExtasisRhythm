import internetarchive as ia

# Lista exacta de las 30 máquinas esenciales
maquinas_deseadas = [
    "808", "909", "606", "707", "727", "R-8",
    "SP-1200", "Drumulator", "DMX", "LinnDrum", "LM-1", "MPC60", "Drumtraks",
    "RX5", "R-100", "RZ1", "HR-16", "RY30", "SDS-V",
    "KR-55", "CR-78", "DR-55", "DR-110", "Minipops", "Rhythm King", "DDM-110", "DDM-220",
    "Machinedrum", "XBase", "Vermona"
]

print("Conectando con Archive.org para buscar kits esenciales...")
item_id = "drum-machines-collection"
item = ia.get_item(item_id)

files_to_download = []

# Recorremos los archivos del repositorio para filtrar los zips deseados
for file_dict in item.files:
    nombre_archivo = file_dict.get('name', '')
    
    if nombre_archivo.endswith('.zip'):
        for maquina in maquinas_deseadas:
            # Búsqueda flexible (ignorando mayúsculas/minúsculas)
            if maquina.lower() in nombre_archivo.lower():
                if nombre_archivo not in files_to_download:
                    print(f"[Seleccionado] -> {nombre_archivo}")
                    files_to_download.append(nombre_archivo)
                break

# Descargamos la lista completa de golpe de forma segura
if files_to_download:
    print(f"\nIniciando descarga de {len(files_to_download)} kits seleccionados...")
    item.download(files=files_to_download, destdir="./Samples_Zips")
    print("\n¡Descarga de kits finalizada con éxito en la carpeta Samples_Zips!")
else:
    print("No se encontraron archivos coincidentes.")