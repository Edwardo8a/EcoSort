# Receta de Blueprint: Contenedores de Reciclaje (BP_Bote_Azul, BP_Bote_Amarillo, BP_Bote_Rojo)

**Proyecto:** EcoSort: San Juan del Río  
**Rol del Actor:** Contenedores clasificadores de residuos ecológicos  
**Clase Padre en C++:** `AEcoSortBote`  
**Ubicación recomendada:** `/Game/EcoSort/Blueprints/`

---

## 1. Crear la Clase Base o Hijos en Blueprint

1. En el **Almacén de contenido**, navega a `/Game/EcoSort/Blueprints/`.
2. Haz clic derecho > **Clase de Blueprint** *(Blueprint Class)*.
3. En la ventana emergente, despliega **Todas las clases** *(All Classes)* y busca:  
   👉 **`EcoSortBote`** (nuestra clase C++ recién compilada).
4. Selecciónala y nómbrala **`BP_Bote_Base`**.

---

## 2. Configurar la Malla y Componentes en `BP_Bote_Base`

1. Abre `BP_Bote_Base`.
2. En el panel **Componentes**, haz clic en **`MallaBote`**:
   - En **Detalles > Malla estática** *(Static Mesh)*: asigna un cilindro o caja (`/Engine/BasicShapes/Cylinder` o tu malla de bote).
   - Escala recomendada: `X: 0.8, Y: 0.8, Z: 1.2`
   - En **Colisiones**: `BlockAll`
3. En el panel **Componentes**, haz clic en **`ZonaDeposito`** (la caja de colisión):
   - En **Detalles > Box Extent**: pon `X: 120.0, Y: 120.0, Z: 100.0` (un área cómoda para que Itzel pueda depositar a menos de 2 metros).
   - En **Colisiones**: `OverlapAllDynamic` o `Trigger`.
4. Guarda y compila `BP_Bote_Base`.

---

## 3. Crear los 3 Botes Específicos (Azul, Amarillo, Rojo)

Haz clic derecho sobre `BP_Bote_Base` > **Crear clase de Blueprint secundaria** para cada uno:

### A. `BP_Bote_Azul` (PET / Plásticos)
- **Malla / Material:** Asigna el material azul `/Game/EcoSort/Materiales/M_Bote_Azul` (creado por el script de Python).
- En el panel **Detalles > EcoSort > Bote**:
  - **Nombre Contenedor:** `Contenedor Azul (PET)`
  - **Tipos Aceptados:** Haz clic en el botón `+` para añadir un elemento y selecciona:  
    👉 **`PET`**

### B. `BP_Bote_Amarillo` (Latas / Metal)
- **Malla / Material:** Asigna el material amarillo `/Game/EcoSort/Materiales/M_Bote_Amarillo`.
- En el panel **Detalles > EcoSort > Bote**:
  - **Nombre Contenedor:** `Contenedor Amarillo (Metal)`
  - **Tipos Aceptados:** Añade un elemento y selecciona:  
    👉 **`Lata`**

### C. `BP_Bote_Rojo` (Papel / Cartón)
- **Malla / Material:** Asigna el material rojo `/Game/EcoSort/Materiales/M_Bote_Rojo`.
- En el panel **Detalles > EcoSort > Bote**:
  - **Nombre Contenedor:** `Contenedor Rojo (Papel)`
  - **Tipos Aceptados:** Añade un elemento y selecciona:  
    👉 **`Papel`**

---

## 4. Reemplazar los actores provisionales en el Nivel 1

En el mapa `Nivel1_OrillaGris`:
1. En el **Esquematizador** (Outliner), busca dentro de la carpeta `EcoSort_N1` los actores provisionales `ZONE_Bote_Azul`, `ZONE_Bote_Amarillo` y `ZONE_Bote_Rojo`.
2. Arrastra desde el Almacén de contenido tus nuevos Blueprints:
   - `BP_Bote_Azul` en la posición del bote azul.
   - `BP_Bote_Amarillo` en la posición del bote amarillo.
   - `BP_Bote_Rojo` en la posición del bote rojo.
3. Puedes borrar los actores provisionales `ZONE_Bote_*` anteriores.
