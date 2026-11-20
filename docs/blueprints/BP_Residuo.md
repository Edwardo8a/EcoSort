# Receta de Blueprint: Residuos Clasificables (BP_Residuo_PET, BP_Residuo_Lata, BP_Residuo_Papel)

**Proyecto:** EcoSort: San Juan del Río  
**Rol del Actor:** Residuos que aparecen en el suelo para ser recogidos por Itzel  
**Clase Padre en C++:** `AEcoSortResiduo`  
**Ubicación recomendada:** `/Game/EcoSort/Blueprints/`

---

## 1. Crear la Clase Base en Blueprint

1. En el **Almacén de contenido**, ve a `/Game/EcoSort/Blueprints/`.
2. Clic derecho > **Clase de Blueprint** > Busca en Todas las clases:  
   👉 **`EcoSortResiduo`**
3. Nómbralo **`BP_Residuo_Base`**.

---

## 2. Configuración en `BP_Residuo_Base`

1. Abre `BP_Residuo_Base`.
2. En el panel **Componentes**, haz clic en **`MallaResiduo`**:
   - En **Física**: asegura que **Simular física** *(Simulate Physics)* esté activado (`True`).
   - En **Colisiones**: perfil `PhysicsActor` (o `BlockAllDynamic`).
   - Masa: aprox. `1.0 kg` (ligero para que ruede bien al caer).
3. Compila y guarda.

---

## 3. Crear los 3 Residuos Específicos

Haz clic derecho sobre `BP_Residuo_Base` > **Crear clase de Blueprint secundaria**:

### A. `BP_Residuo_PET` (Botella de plástico)
- **Malla estática:** Cilindro pequeño o botella (`/Engine/BasicShapes/Cylinder`).
  - Escala: `X: 0.15, Y: 0.15, Z: 0.35`
- **Material:** Asigna `/Game/EcoSort/Materiales/M_PET` (o un material transparente/azul tenue).
- En el panel **Detalles > EcoSort > Residuo**:
  - **Tipo Residuo:** Selecciona **`PET / Plastico`**.

### B. `BP_Residuo_Lata` (Lata de aluminio / metal)
- **Malla estática:** Cilindro pequeño (`/Engine/BasicShapes/Cylinder`).
  - Escala: `X: 0.2, Y: 0.2, Z: 0.25`
- **Material:** Asigna `/Game/EcoSort/Materiales/M_Lata` (material metálico gris/plateado).
- En el panel **Detalles > EcoSort > Residuo**:
  - **Tipo Residuo:** Selecciona **`Lata / Metal`**.

### C. `BP_Residuo_Papel` (Caja o bola de papel/cartón)
- **Malla estática:** Cubo pequeño (`/Engine/BasicShapes/Cube`).
  - Escala: `X: 0.25, Y: 0.25, Z: 0.2`
- **Material:** Asigna `/Game/EcoSort/Materiales/M_Papel` (material beige o blanco opaco).
- En el panel **Detalles > EcoSort > Residuo**:
  - **Tipo Residuo:** Selecciona **`Papel / Carton`**.

---

## 4. Colocar Residuos de Prueba en el Nivel

1. Abre el mapa `Nivel1_OrillaGris`.
2. Arrastra una instancia de `BP_Residuo_PET` y una de `BP_Residuo_Lata` al suelo cerca de `PlayerStart`.
3. Al presionar **Play**:
   - Acércate a la botella PET y presiona **E** -> Itzel la recogerá frente a su pecho.
   - Camina hacia el contenedor azul (`BP_Bote_Azul`) y presiona **E** -> La botella se depositará, desaparecerá y sumará +10 puntos.
   - Si intentas meter la botella PET en el contenedor amarillo (`BP_Bote_Amarillo`), el bote la rechazará, caerá al suelo y restará -5 puntos.
