# Receta de Widget Blueprint: WBP_HUD (Interfaz con Mira, Barras y Puntos)

**Proyecto:** EcoSort: San Juan del Río  
**Rol del Widget:** Interfaz en pantalla durante la partida (HUD)  
**Clase Padre en C++:** `UEcoSortHUDBase` (gestiona eventos y delegados automáticamente sin usar Tick)  
**Ubicación recomendada:** `/Game/EcoSort/UI/WBP_HUD`

---

## 1. Crear el Widget Blueprint Hijo de C++

1. En el **Almacén de contenido**, navega a `/Game/EcoSort/UI/` (crea la carpeta `UI` si no existe).
2. Haz clic derecho en el espacio vacío > **Interfaz de usuario** *(User Interface)* > **Blueprint de widget** *(Widget Blueprint)*.
3. En la ventana emergente, selecciona **Todas las clases** *(All Classes)* y busca:  
   👉 **`EcoSortHUDBase`**
4. Selecciónalo y nómbralo: **`WBP_HUD`**.
5. Haz doble clic para abrirlo en el diseñador de widgets.

---

## 2. Estructura Visual en el Diseñador (Hierarchy / Jerarquía)

Asegúrate de agregar los siguientes elementos desde la **Paleta** *(Palette)*:

```text
[Canvas Panel] (Panel de lienzo raíz)
  ├── [Image] Mira_Punto_Central   <-- ¡LA MIRA DE APUNTADO!
  ├── [Vertical Box] Panel_Superior_Izquierdo
  │     ├── [Text] Label_Rio ("Salud del Río")
  │     ├── [ProgressBar] Barra_Rio
  │     ├── [Text] Label_Habitat ("Restauración del Hábitat")
  │     └── [ProgressBar] Barra_Habitat
  ├── [Vertical Box] Panel_Superior_Derecho
  │     ├── [Text] Texto_Puntos ("Puntos: 0")
  │     └── [Text] Texto_Meta ("Residuos: 0 / 10")
  └── [Text] Texto_Feedback ("+10 ACIERTO")  (Oculto por defecto)
```

---

## 3. Configuración del Punto de Mira Central (Crosshair)

1. Arrastra una **Imagen** *(Image)* al Canvas Panel y renómbrala como **`Mira_Punto_Central`**.
2. En el panel **Detalles** (a la derecha):
   - **Anclajes** *(Anchors)*: Elige el **cuadro central** (Centro de la pantalla: `X: 0.5, Y: 0.5`).
   - **Posición X:** `0.0`
   - **Posición Y:** `0.0`
   - **Tamaño X:** `8.0`
   - **Tamaño Y:** `8.0`
   - **Alineación** *(Alignment)*: `X: 0.5, Y: 0.5`
   - **Tinte** *(Tint / Color)*: Blanco con un ligero borde o amarillo suave.
   *(Esto coloca un puntito nítido en el centro exacto donde apuntas con la cámara).*

---

## 4. Configurar las Barras y Textos

1. **`Barra_Rio` (ProgressBar):**
   - Relleno de color: Azul claro.
   - Porcentaje inicial: `1.0` (100%).
   - Marca la casilla **`Es variable`** *(Is Variable)* arriba en Detalles.
2. **`Barra_Habitat` (ProgressBar):**
   - Relleno de color: Verde brillante.
   - Porcentaje inicial: `0.0` (0%).
   - Marca la casilla **`Es variable`** *(Is Variable)*.
3. **`Texto_Puntos` (TextBlock):**
   - Texto inicial: `Puntos: 0`.
   - Tamaño de fuente: 24, Color: Amarillo/Dorado.
   - Marca la casilla **`Es variable`** *(Is Variable)*.
4. **`Texto_Meta` (TextBlock):**
   - Texto inicial: `Residuos: 0 / 10`.
   - Tamaño de fuente: 18, Color: Blanco.
   - Marca la casilla **`Es variable`** *(Is Variable)*.
5. **`Texto_Feedback` (TextBlock):**
   - Posición: Cerca de la mira central o arriba del centro.
   - Visibilidad inicial: **Collapsed** (Oculto).
   - Marca la casilla **`Es variable`** *(Is Variable)*.

---

## 5. Conectar los Eventos en el Gráfico (Event Graph)

Haz clic en la pestaña **Gráfico** *(Graph)* arriba a la derecha.

Como heredamos de `UEcoSortHUDBase`, **no necesitas usar Event Tick**. Solo implementamos los eventos que C++ dispara automáticamente:

1. **Evento `ActualizarPuntos`:**
   - Clic derecho en el gráfico > busca: **`Event Actualizar Puntos`**.
   - Arrastra la variable `Texto_Puntos` al gráfico (Get).
   - Conecta: `Set Text` -> Conecta el pin entero `NuevosPuntos` usando un nodo `Format Text`: `"Puntos: {Puntos}"`.
2. **Evento `ActualizarHabitat`:**
   - Clic derecho > busca: **`Event Actualizar Habitat`**.
   - Arrastra `Barra_Habitat` (Get) -> llama a `Set Percent`.
   - Conecta: divide `NuevoPorcentaje` entre `100.0` y enchúfalo a `In Percent`.
3. **Evento `ActualizarNivelRio`:**
   - Clic derecho > busca: **`Event Actualizar Nivel Rio`**.
   - Arrastra `Barra_Rio` (Get) -> llama a `Set Percent`.
   - Conecta: divide `NuevoNivel` entre `100.0` y enchúfalo a `In Percent`.
4. **Evento `ActualizarResiduos`:**
   - Clic derecho > busca: **`Event Actualizar Residuos`**.
   - Arrastra `Texto_Meta` (Get) -> `Set Text` con `Format Text`: `"Residuos: {Clasificados} / {Meta}"`.
5. **Evento `MostrarFeedbackAcierto`:**
   - Clic derecho > busca: **`Event Mostrar Feedback Acierto`**.
   - Muestra `Texto_Feedback` en verde: `"¡+10 Acierto!"`. Pon un `Delay (1.5s)` y vuelve a ocultarlo (`Set Visibility: Collapsed`).
6. **Evento `MostrarFeedbackError`:**
   - Clic derecho > busca: **`Event Mostrar Feedback Error`**.
   - Muestra `Texto_Feedback` en rojo: `"¡-5 Error! Bote incorrecto"`. Pon un `Delay (1.5s)` y ocúltalo.

---

## 6. Mostrar el HUD en el Juego

1. Abre tu GameMode (`BP_ThirdPersonGameMode` o `BP_EcoSortGameMode`).
2. En el panel **Detalles** > **EcoSort > UI**:
   - En **HUD Widget Class**, selecciona: 👉 **`WBP_HUD`**.
3. Compila y guarda. Al presionar **Play**, el HUD aparecerá automáticamente en pantalla con tu mira central.
