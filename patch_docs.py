import re

# UPDATE README.md
with open('README.md', 'r') as f:
    readme = f.read()

# Update version title
readme = readme.replace('# Extasis Rhythm (v3.0.10)', '# Extasis Rhythm (v3.1.0)')

# Update "What's new" section
old_whats_new = """## 🌟 What's New in Version 3.0.2
* **Smart Fill Memory (MPC / Pocket Operator Style):** Completely rewritten Fill engine. The sequencer now intelligently remembers your last state when exiting a Fill, ensuring a seamless transition back to your original rhythm without dropping the groove.
* **Cleaner UI:** The purchase/license link has been moved exclusively to the Serial Activation window, giving you a cleaner, distraction-free interface.
* **Cross-Platform Stability:** Resolved algorithm edge cases that caused crashes on certain Windows and Linux host DAWs.
* **Preset Engine:** Fixed missing preset declarations for more reliable preset loading and saving."""

new_whats_new = """## 🌟 What's New in Version 3.1.0
* **Modular Sampler (Drag & Drop):** You can now drag and drop individual `.wav` or `.aif` files directly onto any of the 12 channels to build your custom drum kits on the fly.
* **Drag to DAW (Offline Render):** Instantly export your current pattern! Simply click and drag the new 〰️ WAV button to your DAW's timeline (Ableton, FL Studio) or your Desktop to instantly bounce a 16-second loop.
* **Collect & Save Custom Kits:** The new `COLLECT KIT` button gathers all your randomly dragged samples, organizes them, renames them, and permanently saves them to a new user preset folder.
* **Robust Core Engine:** Implemented thread-safe `thread_local` memory locks for the UI and DSP threads, ensuring absolutely zero glitches or race conditions when doing offline renders or dragging files."""

readme = readme.replace(old_whats_new, new_whats_new)

# Quick Start changes
old_quick = """* **Loading Kits:** Click the folder icon to select a directory containing your `.wav` files. The Smart Engine will automatically map them to the 8 channels."""
new_quick = """* **Loading Kits:** Click the folder icon to select a directory, or simply Drag & Drop your own `.wav` files onto individual channels to build a modular kit. The Smart Engine handles the mapping."""
readme = readme.replace(old_quick, new_quick)

with open('README.md', 'w') as f:
    f.write(readme)


# UPDATE MANUAL.md
with open('MANUAL.md', 'r') as f:
    manual = f.read()

manual = manual.replace('v3.0.10', 'v3.1.0')
manual = manual.replace('v3.0', 'v3.1.0')

# Adding Modular Sampler and Drag to DAW section
add_section = """
### 3.4. Motor Modular, Drag & Drop y Render Offline
La versión 3.1.0 transforma a Extasis Rhythm en un Sampler Modular y herramienta de exportación instantánea:
1. **Drag & Drop a Canales:** Puedes arrastrar audios sueltos (`.wav`, `.aif`) desde Finder, Explorer, o tu DAW directamente a la interfaz de cualquier canal para cargar ese sonido. También puedes usar el botón `[...]` de cada canal para abrir un buscador nativo.
2. **Collect & Save Kit:** En la barra superior encontrarás el botón **COLLECT KIT**. Al presionarlo, el plugin copiará, organizará y guardará permanentemente en disco todos los audios sueltos que hayas arrastrado, creando un Kit de Usuario instantáneo que aparecerá en el menú principal.
3. **Exportar Loop (Drag to DAW):** En la barra superior (icono **〰️ WAV**):
   * **Click y arrastrar:** Renderiza silenciosamente tu patrón en milisegundos y puedes soltar el audio resultante (`Extasis_Loop.wav`) directamente en una pista de tu DAW o en tu escritorio.
   * **Click normal:** Abre una ventana de "Guardar como..." tradicional para guardar el loop en tu disco duro."""

# Find a good place to insert it. Section 3 covers specific features.
if "### 3.3. Secuenciador Euclideano" in manual:
    # We'll insert it right after the Sequencer section
    # Let's just append it to the section 3.
    # Actually, we can use regex to inject it before "## 4. Efectos"
    manual = re.sub(r'(## 4\. Efectos)', add_section + r'\n\n\1', manual)

with open('MANUAL.md', 'w') as f:
    f.write(manual)


# UPDATE ARCHITECTURE.md
with open('ARCHITECTURE.md', 'r') as f:
    arch = f.read()

arch_add = """
### Exportación y Sampler Modular (v3.1.0)
El sistema ha migrado de ser un simple ROMpler (lector de carpetas) a un **Sampler Modular**.
- **`customSamplePaths[12]`**: El motor rastrea independientemente las rutas arrastradas manualmente. Si existe, tiene prioridad absoluta sobre el kit cargado globalmente.
- **`saveCustomKit`**: Intercepta todos los archivos, los copia físicamente a `User_Kits` usando `juce::File` de forma asíncrona (simulada) y re-mapea las variables globales de kit.
- **`renderOfflineLoop` y Thread Safety**: La exportación (Drag-to-DAW) se ejecuta en el `MessageThread` (UI) simulando un reloj de tiempo interno (`offlinePpqPosition`). Para evitar corrupciones de memoria (Race Conditions) con el DAW que sigue solicitando audio en tiempo real, el hilo de audio (`processBlock`) revisa una bandera atómica y un modificador `thread_local bool isBouncingThread`. Si está renderizando, el hilo de audio principal se muteara temporalmente devolviendo buffers limpios, cediendo la prioridad de memoria de las variables del secuenciador al hilo de UI."""

if "### Exportación" not in arch:
    arch = re.sub(r'(## Notas Finales)', arch_add + r'\n\n\1', arch)
    with open('ARCHITECTURE.md', 'w') as f:
        f.write(arch)

