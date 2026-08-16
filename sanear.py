import os
import re

samples_dir = "./Samples"

for root, dirs, files in os.walk(samples_dir, topdown=False):
    for name in files:
        old_path = os.path.join(root, name)
        ext = os.path.splitext(name)[1].lower()
        base = os.path.splitext(name)[0]
        
        # Limpiar base a solo caracteres alfanuméricos y guiones
        clean_base = re.sub(r'[^a-zA-Z0-9_-]', '_', base)
        clean_base = re.sub(r'_+', '_', clean_base).strip('_')
        
        new_name = f"{clean_base}{ext}" if clean_base else f"sound{ext}"
        new_path = os.path.join(root, new_name)
        
        if old_path != new_path:
            os.rename(old_path, new_path)
            
    for name in dirs:
        old_path = os.path.join(root, name)
        clean_name = re.sub(r'[^a-zA-Z0-9_-]', '_', name)
        clean_name = re.sub(r'_+', '_', clean_name).strip('_')
        new_path = os.path.join(root, clean_name if clean_name else "kit")
        
        if old_path != new_path:
            os.rename(old_path, new_path)

print("¡Nombres de samples y carpetas saneados con éxito!")
