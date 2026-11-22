"""
EcoSort: Reemplazar botes decorativos por los Blueprints interactivos en el Nivel 1.
"""
import unreal

def V(x, y, z):
    return unreal.Vector(x * 100.0, y * 100.0, z * 100.0)

def main():
    actor_ss = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    
    # Rutas de los 3 Blueprints
    botes_info = [
        ("BP_Bote_Azul", "/Game/EcoSort/BP_Bote_Azul", -0.5),
        ("BP_Bote_Amarillo", "/Game/EcoSort/BP_Bote_Amarillo", 0.7),
        ("BP_Bote_Rojo", "/Game/EcoSort/BP_Bote_Rojo", 1.9),
    ]

    for nombre, bp_path, x_m in botes_info:
        # Intentar cargar la clase del Blueprint
        bp_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
        if not bp_class:
            bp_class = unreal.load_object(None, f"{bp_path}.{nombre}_C")
        
        if not bp_class:
            unreal.log_error(f"No se pudo cargar la clase: {bp_path}")
            continue

        pos = V(x_m, -8.5, 0.0)
        actor = actor_ss.spawn_actor_from_class(bp_class, pos)
        if actor:
            actor.set_actor_label(nombre)
            actor.set_folder_path("EcoSort_N1/Botes_Interactivos")
            unreal.log(f"[EcoSort] Creado {nombre} en {pos}")

    unreal.log("=== [EcoSort] Botes interactivos colocados con exito ===")

if __name__ == "__main__":
    main()
