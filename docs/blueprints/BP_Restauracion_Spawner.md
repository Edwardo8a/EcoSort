# Receta de Blueprint: Spawner y Restauración Ambiental (Fase 4)

**Proyecto:** EcoSort: San Juan del Río  
**Rol de los Actores:** Generación continua de basura y limpieza visual progresiva del río  

---

## 1. Blueprint: `BP_EcoSortSpawner` (Generador de Residuos)

### Creación:
1. En el **Almacén de contenido**, ve a `/Game/EcoSort/Blueprints/`.
2. Clic derecho > **Clase de Blueprint** > Busca y selecciona la clase padre **`EcoSortSpawner`**.
3. Nómbralo **`BP_EcoSortSpawner`**.
4. Ábrelo y ve al panel **Detalles**:
   - **Clases Residuos:** En la lista, haz clic en **+ Añadir** y agrega los 3 tipos de residuos:
     - `BP_Residuo_PET`
     - `BP_Residuo_Lata`
     - `BP_Residuo_Papel`
   - **Radio Spawn:** `800.0` (8 metros alrededor del spawner).
   - **Intervalo Spawn:** `4.0` (cada 4 segundos intenta soltar uno nuevo).
   - **Max Residuos En Escena:** `8` (máximo 8 residuos libres al mismo tiempo).
   - **Altura Caída:** `45.0`.
5. Compila y guarda.
6. Arrastra una o dos instancias de **`BP_EcoSortSpawner`** a la orilla del parque en tu mapa `Nivel1_OrillaGris`.

---

## 2. Blueprint: `BP_EcoSortRestauracion` (Limpieza del Río y Fauna)

### Creación:
1. En `/Game/EcoSort/Blueprints/`, clic derecho > **Clase de Blueprint** > Selecciona **`EcoSortRestauracion`**.
2. Nómbralo **`BP_EcoSortRestauracion`**.
3. Compila, guarda y arrástralo a tu nivel (en cualquier parte del mapa, por ejemplo cerca de la orilla).

### Configuración en el Nivel (Detalles):
1. Selecciona el **`BP_EcoSortRestauracion`** que acabas de colocar en el nivel.
2. En el panel **Detalles**:
   - **Actor Agua Rio:** Haz clic en el gotero (cuentagotas) y selecciona el plano de agua del río (`Agua_Rio` o la malla de agua en el Outliner).
   - **Nombre Parametro Color:** `BaseColor` (o el nombre del parámetro de color en el material del agua).
   - **Color Agua Contaminada:** Café oscuro lodoso (R: 0.22, G: 0.17, B: 0.09).
   - **Color Agua Limpia:** Turquesa cristalino (R: 0.05, G: 0.45, B: 0.65).
   - **Hitos de Fauna:** Puedes seleccionar actores del Outliner (patos, plantas, piedras limpias) para que aparezcan en los hitos del 30%, 60% y 100%.

---

## 3. Asignar `WBP_Victoria` en el GameMode
1. Abre **`BP_ThirdPersonGameMode`** (o tu `BP_EcoSortGameMode`).
2. En el panel **Detalles**:
   - Busca la categoría **EcoSort | UI**.
   - En **Victoria Widget Class**: Selecciona **`WBP_Victoria`**.
3. Compila y guarda.
