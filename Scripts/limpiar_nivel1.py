# Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.
"""
Script de Limpieza para Nivel 1 (Orilla Gris)
Elimina elementos obsoletos y decorativos no interactuables:
1. El muñeco estático de Itzel (de la carpeta Personaje).
2. Los residuos estáticos decorativos del script inicial (cilindros, esferas, tapas).
3. Botes decorativos viejos si existieran.
Conserva el 100% del entorno (terreno, río, puente, juegos rotos, árboles y vegetación).
"""

import unreal

def limpiar_nivel1():
    unreal.log("[EcoSort] Iniciando limpieza de actores obsoletos en Nivel 1...")

    try:
        actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        todos_los_actores = actor_subsystem.get_all_level_actors()
    except Exception as e:
        unreal.log_error(f"[EcoSort] No se pudo obtener el subsistema de actores: {e}")
        return

    actores_a_borrar = []
    conteo_residuos = 0
    conteo_personaje = 0
    conteo_otros = 0

    for actor in todos_los_actores:
        if not actor or not unreal.SystemLibrary.is_valid(actor):
            continue

        label = actor.get_actor_label()
        clase = actor.get_class().get_name()
        
        # Obtener ruta de carpeta en el Outliner
        try:
            folder = str(actor.get_folder_path())
        except Exception:
            folder = ""

        # -------------------------------------------------------------
        # 1. Muñeco estático de Itzel (de la carpeta Personaje)
        # -------------------------------------------------------------
        if "Personaje" in folder or label.startswith("Itzel_") or label == "Itzel":
            # No tocar el personaje real jugable si estuviera en escena
            if "BP_Itzel" not in clase:
                actores_a_borrar.append(actor)
                conteo_personaje += 1
                continue

        # -------------------------------------------------------------
        # 2. Residuos falsos decorativos (StaticMeshActor)
        # -------------------------------------------------------------
        if "Residuos" in folder or label.startswith("Residuo_"):
            # Solo borrar si es un StaticMeshActor simple y NO nuestros Blueprints interactuables
            if "BP_Residuo" not in clase and "EcoSortResiduo" not in clase:
                actores_a_borrar.append(actor)
                conteo_residuos += 1
                continue

        # -------------------------------------------------------------
        # 3. Botes decorativos viejos (si queda alguno tipo StaticMesh)
        # -------------------------------------------------------------
        if "Botes" in folder and ("Bote_Azul" in label or "Bote_Amarillo" in label or "Bote_Rojo" in label):
            if "BP_Bote" not in clase and "EcoSortBote" not in clase:
                actores_a_borrar.append(actor)
                conteo_otros += 1
                continue

    # Proceder al borrado seguro
    total = len(actores_a_borrar)
    for actor in actores_a_borrar:
        actor_subsystem.destroy_actor(actor)

    unreal.log(f"[EcoSort] =========================================")
    unreal.log(f"[EcoSort] ¡Limpieza completada con éxito!")
    unreal.log(f"[EcoSort] - Muñeco de Itzel falso eliminado: {conteo_personaje} piezas")
    unreal.log(f"[EcoSort] - Residuos decorativos no interactuables eliminados: {conteo_residuos}")
    if conteo_otros > 0:
        unreal.log(f"[EcoSort] - Botes viejos eliminados: {conteo_otros}")
    unreal.log(f"[EcoSort] Total de actores limpiados: {total}")
    unreal.log(f"[EcoSort] =========================================")

if __name__ == "__main__":
    limpiar_nivel1()
