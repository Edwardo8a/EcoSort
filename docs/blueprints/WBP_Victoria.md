# Receta de Blueprint: WBP_Victoria (Pantalla de Victoria de Nivel)

**Proyecto:** EcoSort: San Juan del Río  
**Rol del Widget:** Pantalla de felicitaciones y transición al siguiente nivel tras alcanzar la meta de residuos  
**Clase Padre:** `User Widget`  
**Ubicación recomendada:** `/Game/EcoSort/UI/WBP_Victoria`

---

## 1. Creación del Widget Blueprint
1. En el **Almacén de contenido** (*Content Drawer*), ve a `/Game/EcoSort/UI/`.
2. Clic derecho en espacio vacío > **Interfaz de usuario** (*User Interface*) > **Blueprint de widget** (*Widget Blueprint*).
3. Selecciona **User Widget** y nómbralo **`WBP_Victoria`**.
4. Ábrelo haciendo doble clic.

---

## 2. Jerarquía de Componentes Visuales (Pestaña Diseñador)

Arma la siguiente estructura en el panel **Jerarquía** (*Hierarchy*):

```
[WBP_Victoria]
└── Panel de lienzo (Canvas Panel)
    ├── Fondo_Oscuro (Image) -> Fondo semitransparente que cubre toda la pantalla
    └── Tarjeta_Central (Vertical Box o Overlay centrado)
        ├── Texto_Titulo (Text) -> "¡ORILLA LIMPIA!"
        ├── Texto_Subtitulo (Text) -> "Has restaurado el hábitat del Río San Juan"
        ├── Caja_Puntos (Horizontal Box)
        │   ├── Texto_Puntos_Etiqueta (Text) -> "Puntos Obtenidos: "
        │   └── Texto_Puntos_Valor (Text) -> "0"  [Marcar "Es variable" = True]
        ├── Espaciador (Spacer, Y: 30)
        ├── Boton_Siguiente (Button)
        │   └── Texto_Boton_Siguiente (Text) -> "Siguiente Nivel"
        └── Boton_Reiniciar (Button)
            └── Texto_Boton_Reiniciar (Text) -> "Jugar de Nuevo"
```

---

## 3. Ajustes de Propiedades (Panel Detalles)

### A. `Fondo_Oscuro` (Image):
- **Anclas:** Pantalla completa (la opción de 4 esquinas/flechas).
- **Offset:** Izquierda = 0, Superior = 0, Derecha = 0, Inferior = 0.
- **Color y opacidad:** Negro con opacidad `0.75` (R:0, G:0, B:0, A:0.75).

### B. `Tarjeta_Central` (Vertical Box):
- **Anclas:** Centro de pantalla (Ancla en X: 0.5, Y: 0.5).
- **Posición X:** `0.0`, **Posición Y:** `0.0`.
- **Alignment:** X: `0.5`, Y: `0.5`.
- **Size To Content:** Marcado (**True**).

### C. `Texto_Titulo` (Text):
- **Texto:** `¡MISIÓN CUMPLIDA!`
- **Color:** Dorado o verde brillante (`#2ecc71` o amarillo `#f1c40f`).
- **Tamaño de fuente:** `42` o `48` (Bold / Negrita).
- **Alineación:** Centro.

### D. `Texto_Puntos_Valor` (Text):
- **Es variable:** Marcado (**True**).
- **Texto:** `0`.
- **Tamaño de fuente:** `32` (Bold).
- **Color:** Blanco puro.

### E. Botones (`Boton_Siguiente` y `Boton_Reiniciar`):
- **Padding:** 10 px superior e inferior.
- **Colores (Style > Normal > Tint):**
  - `Boton_Siguiente`: Verde bosque (`#27ae60`).
  - `Boton_Reiniciar`: Gris azulado (`#34495e`).

---

## 4. Lógica en el Grafo de Eventos (EventGraph)

### A. Mostrar el Puntaje Final al aparecer (`Event Construct`):
1. Desde el nodo **`Event Construct`**, arrastra y busca **`Get Game State`**.
2. Arrastra desde el Game State y haz un **`Cast to EcoSortGameState`**.
3. Del pin azul de salida, llama a **`Get Puntos`**.
4. Arrastra tu variable **`Texto_Puntos_Valor`** al grafo con *Get*.
5. Desde `Texto_Puntos_Valor`, llama a **`SetText (Text)`**.
6. Conecta el número de `Puntos` al pin `In Text` (se creará automáticamente el convertidor a texto).

### B. Funcionalidad de los Botones (`On Clicked`):

1. **`Boton_Siguiente`:**
   - Selecciona `Boton_Siguiente` en el panel izquierdo.
   - En Detalles abajo, clic en el botón verde **`+ En clic (On Clicked)`**.
   - En el grafo:
     - Llama a **`Get Game Mode`**.
     - Haz **`Cast to EcoSortGameMode`**.
     - Desde el GameMode llama a la función C++: **`Cargar Siguiente Nivel`** (*CargarSiguienteNivel*).

2. **`Boton_Reiniciar`:**
   - Selecciona `Boton_Reiniciar` en el panel izquierdo.
   - En Detalles abajo, clic en el botón verde **`+ En clic (On Clicked)`**.
   - En el grafo:
     - Llama a **`Get Game Mode`**.
     - Haz **`Cast to EcoSortGameMode`**.
     - Desde el GameMode llama a la función C++: **`Reiniciar Nivel Actual`** (*ReiniciarNivelActual*).

3. Haz clic en **Compilar** y **Guardar**.
