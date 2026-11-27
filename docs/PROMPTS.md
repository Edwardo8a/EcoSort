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


---

## [19/11/2026] — Fase 2: Residuos y Botes de Reciclaje
- **Prompt:** Implementar la lógica completa de depósito en C++, contenedor de reciclaje `AEcoSortBote` con validación de tipos, `AEcoSortGameState` con eventos para HUD y actualización de `UEcoSortInteractionComponent`.
- **Entregables C++:**
  - `Source/EcoSort/EcoSortBote.h/.cpp`: Clase `AEcoSortBote` con `UBoxComponent` trigger (`ZonaDeposito`), propiedad `TiposAceptados`, función `Depositar(Residuo)` y evento `OnDepositoResultado`.
  - `Source/EcoSort/EcoSortGameState.h/.cpp`: Manejo de `Puntos` (+10 acierto, -5 error), `HabitatPorcentaje` (0-100%), `NivelRio` y delegados dinámicos sin Tick para la UI.
  - `Source/EcoSort/EcoSortInteractionComponent.h/.cpp`: Lógica inteligente de interacción: si se lleva residuo y hay un bote en radio 200 cm deposita; si no hay bote suelta; si tiene manos vacías recoge a 150 cm.
- **Entregables de Documentación:**
  - `docs/blueprints/BP_Bote.md`: Receta para `BP_Bote_Base` y sus 3 variantes (`BP_Bote_Azul`, `BP_Bote_Amarillo`, `BP_Bote_Rojo`) con materiales y configuración de `TiposAceptados`.
  - `docs/blueprints/BP_Residuo.md`: Receta para `BP_Residuo_Base` y sus 3 variantes (`BP_Residuo_PET`, `BP_Residuo_Lata`, `BP_Residuo_Papel`) con física y escala adecuada.
- **Cómo se prueba en el editor:**
  1. Crear los Blueprints hijos siguiendo las recetas de `docs/blueprints/`.
  2. Colocar en el nivel los 3 botes y residuos de prueba.
  3. Presionar Play, recoger un residuo con `E` y llevarlo al contenedor correcto (suma puntos y se destruye) o al incorrecto (lo rechaza y resta puntos).


---

## [21/11/2026] — Fase 3: GameState, GameMode y HUD con Mira de Apuntado
- **Prompt:** Implementar `FEcoNivelConfig` para DataTable, `AEcoSortGameMode` con detección de victoria/derrota y carga de niveles, `UEcoSortHUDBase` reactivo a delegados, mejora de apuntado asistido por cámara en `UEcoSortInteractionComponent`, script para generar `DT_Niveles` y receta de `WBP_HUD` con mira central (crosshair).
- **Entregables C++:**
  - `Source/EcoSort/EcoSortTipos.h`: Estructura `FEcoNivelConfig` derivada de `FTableRowBase` (meta, salud inicial, drenaje por segundo, intervalo de spawn, siguiente nivel).
  - `Source/EcoSort/EcoSortGameState.h/.cpp`: Métodos `SetMetaResiduos()`, `SetNivelRio()` y delegados dinámicos multicast para HUD.
  - `Source/EcoSort/EcoSortGameMode.h/.cpp`: Lógica de partida, lectura de `DT_Niveles`, temporizador de drenaje de río, condiciones de victoria/derrota (`OnVictoria`, `OnDerrota`) y creación automática del HUD.
  - `Source/EcoSort/EcoSortHUDBase.h/.cpp`: Clase base UMG que se suscribe a los eventos del GameState y expone BlueprintImplementableEvents para actualizar barras, puntos y feedback.
  - `Source/EcoSort/EcoSortInteractionComponent.h/.cpp`: Apuntado mejorado con SphereTrace desde la cámara hacia el centro de la mira y ponderación por ángulo frontal.
- **Entregables de Documentación & Scripts:**
  - `Scripts/crear_datatable_niveles.py`: Script para crear y configurar `/Game/EcoSort/Datos/DT_Niveles`.
  - `docs/blueprints/WBP_HUD.md`: Receta detallada paso a paso para diseñar `WBP_HUD` con mira central de 8x8px, barra de salud de río, barra de hábitat, puntos y eventos de feedback visual (+10 verde / -5 rojo).
- **Resultado de Compilación:** `Result: Succeeded` (0 errores) vía `Build.bat`.
- **Prueba en Juego y Verificación:**
  - Verificado en editor por el usuario: `WBP_HUD` activo en pantalla, mira central centrada, barra de río azul al 100%, barra de hábitat verde al 20%, contador de puntos en 20 y contador de residuos clasificados en 2/10 al recoger y depositar residuos con la tecla `E`.

---

## [26/11/2026] — Fase 4: Spawner de Residuos y Limpieza / Restauración del Río
- **Prompt:** Implementar el generador de residuos automático (`AEcoSortSpawner`), el sistema de restauración ambiental progresiva (`AEcoSortRestauracion`) y el widget de victoria de nivel (`WBP_Victoria`).

