# Receta de Blueprint: BP_Itzel

**Proyecto:** EcoSort: San Juan del Río  
**Rol del Actor:** Personaje jugable principal (Itzel, 13 años, voluntaria ambiental)  
**Clase Padre en Unreal:** `BP_ThirdPersonCharacter` (hereda movimiento en 3ra persona y Enhanced Input)  
**Ubicación recomendada:** `/Game/EcoSort/Blueprints/BP_Itzel`

---

## 1. Creación del Blueprint Hijo
1. En el **Content Drawer**, navega a `/Game/ThirdPerson/Blueprints/` (o busca `BP_ThirdPersonCharacter`).
2. Haz clic derecho sobre `BP_ThirdPersonCharacter` > **Create Child Blueprint Class**.
3. Nómbralo **`BP_Itzel`** y muévelo a la carpeta `/Game/EcoSort/Blueprints/`.

---

## 2. Ajustes de Componentes y Valores (Details Panel)

### A. Cápsula de Colisión (`CapsuleComponent`)
- **Capsule Radius:** `30.0`
- **Capsule Half Height:** `75.0` *(adaptado a la estatura de una niña de 13 años)*

### B. Maniquí por Defecto (`Mesh`)
- Selecciona el componente `Mesh` (el maniquí gris de Unreal).
- En el panel Details > **Rendering** > marca **Visible = False** (o **Hidden in Game = True**).

### C. Velocidad de Movimiento (`CharacterMovementComponent`)
- Selecciona `CharacterMovement (CharMoveComp)`.
- Busca **Max Walk Speed**: cámbialo de 500/600 a **`350.0`** *(velocidad adecuada para explorar y recoger residuos sin saltarse los objetos)*.

### D. Componente de Interacción C++ (`EcoSortInteractionComponent`)
1. En la esquina superior izquierda, clic en **+ Add**.
2. Busca y añade **`EcoSortInteraction`** (nuestra clase C++ recién compilada).
3. En sus detalles:
   - **Radio Interaccion:** `150.0`
   - **Nombre Componente Mano:** `Mano`

### E. Punto de Anclaje de Residuos (`SceneComponent` "Mano")
1. Clic en **+ Add** > selecciona **Scene**.
2. Renómbralo exactamente como **`Mano`** (¡cuidado con mayúsculas!).
3. Hazlo hijo de la Cápsula (o arrástralo sobre la cápsula).
4. Posición relativa (**Location**):
   - **X:** `45.0` (frente al pecho)
   - **Y:** `0.0`
   - **Z:** `10.0`

---

## 3. Integración de la Malla de Itzel (`SM_Itzel`)

El script de Python `ecosort_unreal_nivel1.py` genera un personaje articulado con formas geométricas dentro de la carpeta del Outliner `EcoSort_N1/Personaje`.

### Pasos para fusionar en un solo Static Mesh:
1. Abre el mapa donde ejecutaste `ecosort_unreal_nivel1.py`.
2. En el **World Outliner**, despliega la carpeta **`EcoSort_N1`** y selecciona todos los actores dentro de **`Personaje`** (cabeza, torso morado, brazos, guantes amarillos, bolsa).
3. Haz clic derecho sobre la selección en el visor o en el Outliner > **Merge Actors** (o ve a la barra superior: *Tools > Merge Actors*).
4. En la ventana que aparece:
   - Marca **Replace Source Actors** = False.
   - Haz clic en el botón inferior **Merge Actors**.
   - Guarda el archivo en `/Game/EcoSort/Meshes/` con el nombre **`SM_Itzel`**.
5. Abre **`BP_Itzel`**, haz clic en **+ Add** > **Static Mesh**, nómbralo `MallaVisualItzel`:
   - **Static Mesh:** Asigna `SM_Itzel`.
   - **Collision Presets:** `NoCollision` *(para que la física del personaje la maneje únicamente la cápsula)*.
   - Ajusta su posición `Z` relativa para que los pies toquen la base de la cápsula (aprox. `Z = -75.0`).

---

## 4. Configuración del Input de Interacción (`IA_Interact` - Tecla E)

### A. Crear la Acción de Entrada:
1. En `/Game/EcoSort/Input/` (o `/Game/ThirdPerson/Input/Actions/`):
   - Clic derecho > **Input** > **Input Action**.
   - Nómbrala **`IA_Interact`**.
2. Abre tu Input Mapping Context activo (usualmente `/Game/ThirdPerson/Input/IMC_Default`):
   - Clic en el botón **+** junto a *Mappings*.
   - Selecciona **`IA_Interact`**.
   - En el campo de tecla asigna: **`E`** (teclado) y opcionalmente botón frontal izquierdo (Gamepad Face Button Left / X / Cuadrado).
   - Guarda y cierra.

### B. Conectar en el Event Graph de `BP_Itzel`:
1. Abre **`BP_Itzel`** y ve a la pestaña **Event Graph**.
2. Haz clic derecho en el grafo vacío y busca el evento:
   - **`EnhancedInputAction IA_Interact`**
3. Desde el panel *Components*, arrastra **`EcoSortInteraction`** al grafo.
4. Desde el pin azul de `EcoSortInteraction`, arrastra y busca la función C++:
   - **`Try Interact`**
5. Conecta el pin blanco de salida **Started** (o **Triggered**) de `IA_Interact` al pin de entrada de **`Try Interact`**.
6. Compila y guarda el Blueprint.

---

## 5. Asignar `BP_Itzel` en el GameMode
1. En el **Content Drawer**, haz clic derecho > **Blueprint Class** > elige **`GameModeBase`** (o crea un hijo de `EcoSortGameMode`).
2. Nómbralo **`BP_EcoSortGameMode`**.
3. Abre `BP_EcoSortGameMode` y en el panel **Classes**:
   - **Default Pawn Class:** Selecciona **`BP_Itzel`**.
4. En tu nivel, en el menú superior ve a **World Settings**:
   - En **GameMode Override**, selecciona **`BP_EcoSortGameMode`**.
