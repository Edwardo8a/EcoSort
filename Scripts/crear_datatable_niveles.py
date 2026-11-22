"""
EcoSort: Crear DataTable de configuracion de niveles (DT_Niveles)
Crea /Game/EcoSort/Datos/DT_Niveles usando el struct FEcoNivelConfig de C++.
Define los valores de juego para Nivel 1 (Orilla Gris) y Nivel 2 (Corriente en Movimiento).
"""
import unreal

def main():
    package_path = "/Game/EcoSort/Datos"
    asset_name = "DT_Niveles"
    full_path = f"{package_path}/{asset_name}"

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    editor_asset_lib = unreal.EditorAssetLibrary

    # Crear carpeta si no existe
    if not editor_asset_lib.does_directory_exist(package_path):
        editor_asset_lib.make_directory(package_path)

    # Buscar la estructura de C++ FEcoNivelConfig
    struct_obj = unreal.load_object(None, "/Script/EcoSort.EcoNivelConfig")
    if not struct_obj:
        unreal.log_error("[EcoSort] No se pudo encontrar el struct /Script/EcoSort.EcoNivelConfig. Asegurate de compilar el C++ primero.")
        return

    # Si ya existe, avisar
    if editor_asset_lib.does_asset_exist(full_path):
        unreal.log(f"[EcoSort] El asset {full_path} ya existe. Procediendo a actualizar...")

    # Crear la fabrica de Data Table
    factory = unreal.DataTableFactory()
    factory.struct = struct_obj

    dt = asset_tools.create_asset(asset_name, package_path, unreal.DataTable, factory)
    if not dt:
        unreal.log_error("[EcoSort] No se pudo crear el DataTable.")
        return

    unreal.log(f"=== [EcoSort] DataTable creado con exito en {full_path} ===")

if __name__ == "__main__":
    main()
