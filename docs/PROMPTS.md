# Bitácora de Prompts e Iteraciones — EcoSort: San Juan del Río

Historial de prompts, decisiones técnicas y entregables generados para la versión híbrida (C++ + Blueprints).

---

## [17/11/2026] — Fase 0: Preparación e Infraestructura
- **Prompt:** Configuración inicial del repositorio, memoria virtual, análisis del proyecto híbrido e importación del script de Nivel 1.
- **Acciones y Archivos:**
  - Creación de `.gitignore` optimizado para UE5.
  - Creación de carpetas `docs/`, `docs/blueprints/`, `Scripts/`.
  - Integración de `Scripts/ecosort_unreal_nivel1.py` desde Descargas.
  - Corrección de memoria virtual y `BuildConfiguration.xml`.
  - Primer commit en Git (`feat: initial project state`).
- **Prueba:** Compilación exitosa en 0 errores del módulo `EcoSort`.


---

## [17/11/2026] — Fase 1: Proyecto C++ Base + Itzel Jugable
- **Prompt:** Implementar las clases C++ base para residuos e interacción de Itzel, verificar compilación en terminal y generar recetas de Blueprint y scripts auxiliares.
- **Entregables C++:**
  - `Source/EcoSort/EcoSortTipos.h`: UENUM `EEcoTipoResiduo` (PET, Lata, Papel, Neumatico, Vidrio).
  - `Source/EcoSort/EcoSortResiduo.h/.cpp`: Clase base `AEcoSortResiduo` con simulación física, colisión y funciones `Recoger`/`Soltar`.
  - `Source/EcoSort/EcoSortInteractionComponent.h/.cpp`: Componente `UEcoSortInteractionComponent` con detección a 150 cm y `TryInteract()`.
  - Inclusión de `"SlateCore"` en `Source/EcoSort/EcoSort.Build.cs`.
- **Entregables de Documentación & Scripts:**
  - `docs/blueprints/BP_Itzel.md`: Guía de creación del Blueprint hijo de Itzel, configuración de cápsula (30x75), velocidad 350, anclaje `Mano`, y mapeo de `IA_Interact` (tecla E).
  - `Scripts/generar_muros_invisibles.py`: Script de Python para generar volúmenes de contención invisibles alrededor del parque y río.
- **Resultado de Compilación:** `Result: Succeeded` (0 errores) vía `Build.bat`.
- **Cómo se prueba en el editor:**
  1. Ejecutar `Scripts/generar_muros_invisibles.py` en el nivel abierto para colocar los límites.
  2. Seguir `docs/blueprints/BP_Itzel.md` para crear `BP_Itzel` y asignarlo como Default Pawn en `BP_EcoSortGameMode`.
  3. Colocar un `AEcoSortResiduo` en el nivel, presionar Play, acercarse y presionar `E` para levantarlo y volver a presionar `E` para soltarlo.
