"""
EcoSort: San Juan del Rio  |  MUROS INVISIBLES DE COLISION (Nivel 1)
Genera volumenes invisibles de bloqueo para evitar que Itzel caiga al rio contaminado
o se salga de los limites del parque.

USO EN UNREAL ENGINE:
Tools > Execute Python Script > seleccionar este archivo.
"""
import unreal

CARPETA_OUTLINER = "EcoSort_N1/Colisiones_Limites"

def V(x, y, z):
    """Convierte metros a unreal.Vector en centimetros."""
    return unreal.Vector(x * 100.0, y * 100.0, z * 100.0)

def crear_muro_invisible(nombre, pos_m, tamano_m):
    """
    Crea un StaticMeshActor tipo Cubo sin visualizacion (Hidden in Game)
    pero con colision 'BlockAll' para actuar como muro invisible.
    """
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    
    # Eliminar version previa si existe
    for a in actor_subsystem.get_all_level_actors():
        if a.get_actor_label() == nombre:
            actor_subsystem.destroy_actor(a)
            break

    # Spawn del actor cubo
    pos_cm = V(pos_m[0], pos_m[1], pos_m[2])
    cubo = actor_subsystem.spawn_actor_from_class(unreal.StaticMeshActor, pos_cm)
    if not cubo:
        unreal.log_error(f"No se pudo crear el muro: {nombre}")
        return None

    cubo.set_actor_label(nombre)
    cubo.set_folder_path(CARPETA_OUTLINER)

    # Configurar malla estatica de cubo basico
    mesh_comp = cubo.static_mesh_component
    mesh_asset = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    if mesh_asset:
        mesh_comp.set_static_mesh(mesh_asset)

    # Escala (cubo basico mide 100x100x100 cm = 1x1x1 m)
    escala = unreal.Vector(tamano_m[0], tamano_m[1], tamano_m[2])
    cubo.set_actor_scale3d(escala)

    # Ocultar en juego y bloquear al jugador
    mesh_comp.set_editor_property("hidden_in_game", True)
    mesh_comp.set_editor_property("cast_shadow", False)
    mesh_comp.set_editor_property("visibility", False)
    mesh_comp.set_collision_profile_name("BlockAll")

    unreal.log(f"[EcoSort] Creado muro invisible: {nombre} en pos {pos_m} tamano {tamano_m}")
    return cubo

def main():
    unreal.log("=== [EcoSort] Generando Muros Invisibles de Contencion ===")

    # 1. Muro Orilla del Rio (evita caer al agua desde el parque)
    # Parque va de Y = -4.1m hacia atras; Rio esta entre Y = -3.9m y +3.9m
    crear_muro_invisible(
        nombre="MuroInvisible_OrillaRio",
        pos_m=(0.0, -4.1, 1.5),
        tamano_m=(90.0, 0.4, 3.0)  # 90m de largo, 40cm ancho, 3m alto
    )

    # 2. Muro Limite Trasero del Parque (Y = -45m)
    crear_muro_invisible(
        nombre="MuroInvisible_FondoParque",
        pos_m=(0.0, -45.0, 1.5),
        tamano_m=(90.0, 0.5, 3.0)
    )

    # 3. Muro Lateral Izquierdo (X = -45m)
    crear_muro_invisible(
        nombre="MuroInvisible_LateralIzquierdo",
        pos_m=(-45.0, -24.5, 1.5),
        tamano_m=(0.5, 41.0, 3.0)
    )

    # 4. Muro Lateral Derecho (X = +45m)
    crear_muro_invisible(
        nombre="MuroInvisible_LateralDerecho",
        pos_m=(45.0, -24.5, 1.5),
        tamano_m=(0.5, 41.0, 3.0)
    )

    unreal.log("=== [EcoSort] Muros Invisibles generados con exito ===")

if __name__ == "__main__":
    main()
