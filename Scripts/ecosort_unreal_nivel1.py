"""
EcoSort: San Juan del Rio  |  NIVEL 1 - ORILLA GRIS   (Unreal Engine 5.x, Python)

Construye en el nivel ABIERTO: terreno y rio, agua con corriente animada, Itzel, botes,
residuos, juegos de parque rotos, tuberia con lodo, puente, skyline, iluminacion nublada
(sol + atmosfera + niebla + post proceso), 5 camaras, PlayerStart, puntos de spawn y zonas.
Crea los materiales en /Game/EcoSort/Materiales.

USO (resumen):  Edit > Plugins > "Python Editor Script Plugin" + "Editor Scripting Utilities"
(reinicia) > File/Tools > Execute Python Script > este archivo.   (Pasos completos en el chat.)

Todas las medidas estan en METROS dentro del script (se convierten a cm).
Para volver a ejecutarlo: simplemente corre el script otra vez (borra lo anterior).
"""
import unreal
import math
import random

# ======================= CONFIGURACION =======================
CARPETA = "EcoSort_N1"                  # carpeta en el World Outliner
MAT_DIR = "/Game/EcoSort/Materiales"    # donde se guardan los materiales
SEED = 11
DENSIDAD = 1.0                          # 0.5 = mitad de objetos (si tu PC va lenta)
LIMPIAR_ESCENA = True                   # borra lo creado antes por este script
BORRAR_LUCES_PREVIAS = True             # quita sol/cielo/niebla que traiga el nivel
CREAR_CAMARAS = True
GUARDAR_NIVEL = False                   # True = guarda el nivel al terminar
# =============================================================

_ACTOR_SS = None
_SHAPES = {}
_CONT = [0]

SHAPE_PATHS = {
    "cube": "/Engine/BasicShapes/Cube.Cube",
    "sphere": "/Engine/BasicShapes/Sphere.Sphere",
    "cylinder": "/Engine/BasicShapes/Cylinder.Cylinder",
    "cone": "/Engine/BasicShapes/Cone.Cone",
}


# ---------------------------------------------------------------- utilidades
def V(x, y, z):
    """Metros -> Vector en cm."""
    return unreal.Vector(x * 100.0, y * 100.0, z * 100.0)


def R(roll=0.0, pitch=0.0, yaw=0.0):
    return unreal.Rotator(roll=roll, pitch=pitch, yaw=yaw)


def D(n):
    return max(1, int(round(n * DENSIDAD)))


def log(txt):
    unreal.log("[EcoSort] " + txt)


def _spawn(cls, loc, rot=None):
    global _ACTOR_SS
    if rot is None:
        rot = R()
    if _ACTOR_SS is None:
        try:
            _ACTOR_SS = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        except Exception:
            _ACTOR_SS = False
    if _ACTOR_SS:
        return _ACTOR_SS.spawn_actor_from_class(cls, loc, rot)
    return unreal.EditorLevelLibrary.spawn_actor_from_class(cls, loc, rot)


def _all_actors():
    try:
        return unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    except Exception:
        return unreal.EditorLevelLibrary.get_all_level_actors()


def _shape(forma):
    """Carga la forma basica y mide su tamano real (asi las dimensiones salen exactas)."""
    if forma not in _SHAPES:
        mesh = unreal.EditorAssetLibrary.load_asset(SHAPE_PATHS[forma])
        try:
            e = mesh.get_bounds().box_extent
            dims = (max(e.x * 2, 1.0), max(e.y * 2, 1.0), max(e.z * 2, 1.0))
        except Exception:
            dims = (100.0, 100.0, 100.0)
        _SHAPES[forma] = (mesh, dims)
    return _SHAPES[forma]


class Grupo(object):
    """Un conjunto de piezas con posicion y giro (yaw en grados) propios."""

    def __init__(self, nombre, x=0.0, y=0.0, z=0.0, yaw=0.0, carpeta=""):
        self.n = nombre
        self.x, self.y, self.z = x, y, z
        self.yaw = yaw
        self.carpeta = carpeta

    def p(self, l):
        c, s = math.cos(math.radians(self.yaw)), math.sin(math.radians(self.yaw))
        return (self.x + l[0] * c - l[1] * s, self.y + l[0] * s + l[1] * c, self.z + l[2])


def _pieza(g, nombre, forma, loc, size, rot, m, mov=False):
    mesh, dims = _shape(forma)
    a = _spawn(unreal.StaticMeshActor, V(*loc), rot)
    comp = a.static_mesh_component
    comp.set_static_mesh(mesh)
    a.set_actor_scale3d(unreal.Vector(size[0] * 100.0 / dims[0],
                                      size[1] * 100.0 / dims[1],
                                      size[2] * 100.0 / dims[2]))
    a.set_actor_label("%s_%s" % (g.n, nombre))
    path = CARPETA + ("/" + g.carpeta if g.carpeta else "")
    a.set_folder_path(path)
    if m is not None:
        comp.set_material(0, m)
    if mov:
        try:
            comp.set_mobility(unreal.ComponentMobility.MOVABLE)
        except Exception:
            pass
    _CONT[0] += 1
    return a


def box(g, nombre, l, s, m, yaw=0.0, roll=0.0, pitch=0.0, mov=False):
    return _pieza(g, nombre, "cube", g.p(l), s, R(roll, pitch, yaw + g.yaw), m, mov)


def esfera(g, nombre, l, s, m, yaw=0.0, mov=False):
    return _pieza(g, nombre, "sphere", g.p(l), s, R(0, 0, yaw + g.yaw), m, mov)


def cilindro(g, nombre, l, d, h, m, mov=False):
    return _pieza(g, nombre, "cylinder", g.p(l), (d, d, h), R(0, 0, g.yaw), m, mov)


def cono(g, nombre, l, d, h, m, mov=False):
    return _pieza(g, nombre, "cone", g.p(l), (d, d, h), R(0, 0, g.yaw), m, mov)


def eje(g, nombre, a, b, d, m, forma="cylinder", mov=False):
    """Cilindro (o cono) cuyo eje va del punto a al punto b (coords locales del grupo)."""
    p1, p2 = g.p(a), g.p(b)
    dx, dy, dz = p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]
    largo = math.sqrt(dx * dx + dy * dy + dz * dz)
    if largo < 0.001:
        largo = 0.001
    rot = unreal.MathLibrary.make_rot_from_z(unreal.Vector(dx, dy, dz))
    c = ((p1[0] + p2[0]) / 2, (p1[1] + p2[1]) / 2, (p1[2] + p2[2]) / 2)
    return _pieza(g, nombre, forma, c, (d, d, largo), rot, m, mov)


def tabla(g, nombre, a, b, ancho, espesor, m, mov=False):
    """Tablon plano que va del punto a al punto b (rampas, tablas de subibaja...)."""
    p1, p2 = g.p(a), g.p(b)
    dx, dy, dz = p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]
    largo = math.sqrt(dx * dx + dy * dy + dz * dz)
    if largo < 0.001:
        largo = 0.001
    rot = unreal.MathLibrary.make_rot_from_xz(unreal.Vector(dx, dy, dz), unreal.Vector(0, 0, 1))
    c = ((p1[0] + p2[0]) / 2, (p1[1] + p2[1]) / 2, (p1[2] + p2[2]) / 2)
    return _pieza(g, nombre, "cube", c, (largo, ancho, espesor), rot, m, mov)


# ---------------------------------------------------------------- materiales
def mat(nombre, color, rough=0.8, metal=0.0, emis=None, alpha=None, ruido=None, agua=None):
    """Crea (o reutiliza) un material /Game/EcoSort/Materiales/M_<nombre>.
    ruido=(escala_por_cm, color_b)  -> variacion de color procedural (sin texturas)
    agua=velocidad_cm_s             -> el ruido se desplaza con el tiempo (corriente)"""
    path = "%s/M_%s" % (MAT_DIR, nombre)
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    m = None
    try:
        unreal.EditorAssetLibrary.make_directory(MAT_DIR)
        m = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            "M_" + nombre, MAT_DIR, unreal.Material, unreal.MaterialFactoryNew())
        L = unreal.MaterialEditingLibrary
        MP = unreal.MaterialProperty

        def nuevo(cls, x, y):
            return L.create_material_expression(m, cls, x, y)

        def c3(rgb, x=-600, y=0):
            e = nuevo(unreal.MaterialExpressionConstant3Vector, x, y)
            e.set_editor_property("constant", unreal.LinearColor(rgb[0], rgb[1], rgb[2], 1.0))
            return e

        def c1(v, x=-600, y=0):
            e = nuevo(unreal.MaterialExpressionConstant, x, y)
            e.set_editor_property("r", float(v))
            return e

        if ruido is not None:
            esc, col_b = ruido
            if agua is not None:
                tiempo = nuevo(unreal.MaterialExpressionTime, -1500, 250)
                vel = c3((agua, 0.0, 0.0), -1500, 400)
                mul = nuevo(unreal.MaterialExpressionMultiply, -1300, 300)
                L.connect_material_expressions(tiempo, "", mul, "A")
                L.connect_material_expressions(vel, "", mul, "B")
                wp = nuevo(unreal.MaterialExpressionWorldPosition, -1300, 120)
                suma = nuevo(unreal.MaterialExpressionAdd, -1100, 200)
                L.connect_material_expressions(wp, "", suma, "A")
                L.connect_material_expressions(mul, "", suma, "B")
                pos = suma
            else:
                pos = nuevo(unreal.MaterialExpressionWorldPosition, -1000, 0)
            nz = nuevo(unreal.MaterialExpressionNoise, -800, 0)
            nz.set_editor_property("scale", float(esc))
            nz.set_editor_property("levels", 3)
            nz.set_editor_property("output_min", 0.0)
            nz.set_editor_property("output_max", 1.0)
            L.connect_material_expressions(pos, "", nz, "Position")
            lp = nuevo(unreal.MaterialExpressionLinearInterpolate, -400, 0)
            L.connect_material_expressions(c3(color, -650, -150), "", lp, "A")
            L.connect_material_expressions(c3(col_b, -650, -300), "", lp, "B")
            L.connect_material_expressions(nz, "", lp, "Alpha")
            L.connect_material_property(lp, "", MP.MP_BASE_COLOR)
        else:
            L.connect_material_property(c3(color), "", MP.MP_BASE_COLOR)

        L.connect_material_property(c1(rough, -600, 200), "", MP.MP_ROUGHNESS)
        if metal > 0:
            L.connect_material_property(c1(metal, -600, 300), "", MP.MP_METALLIC)
        if emis is not None:
            L.connect_material_property(c3(emis, -600, 400), "", MP.MP_EMISSIVE_COLOR)
        if alpha is not None:
            m.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
            try:
                m.set_editor_property("two_sided", True)
            except Exception:
                pass
            L.connect_material_property(c1(alpha, -600, 500), "", MP.MP_OPACITY)
        L.recompile_material(m)
        unreal.EditorAssetLibrary.save_loaded_asset(m)
    except Exception as e:
        log("AVISO material %s: %s (se usa material simple)" % (nombre, e))
    return m


def materiales():
    M = {}
    M["suelo"] = mat("Suelo_Seco", (.30, .28, .25), .95, ruido=(0.0035, (.15, .14, .13)))
    M["muro"] = mat("Muro_Piedra", (.38, .37, .35), .9, ruido=(0.02, (.2, .19, .18)))
    M["lecho"] = mat("Lecho", (.06, .05, .04), 1.0)
    M["agua"] = mat("Agua_Sucia", (.06, .08, .035), .08, alpha=.93,
                    ruido=(0.012, (.14, .15, .06)), agua=40.0)
    M["lodo"] = mat("Lodo", (.16, .18, .05), .25)
    M["aceite"] = mat("Aceite", (.03, .02, .06), .04, alpha=.85)
    M["morado"] = mat("Itzel_Playera", (.36, .16, .62), .8)
    M["amarillo"] = mat("Itzel_Guantes", (.96, .8, .1), .6)
    M["piel"] = mat("Itzel_Piel", (.78, .52, .38), .6)
    M["pelo"] = mat("Itzel_Pelo", (.04, .025, .02), .5)
    M["jean"] = mat("Itzel_Pantalon", (.12, .18, .38), .8)
    M["negro"] = mat("Negro", (.03, .03, .03), .9)
    M["bolsa"] = mat("Bolsa_Basura", (.04, .04, .05), .25)
    M["pet"] = mat("PET", (.6, .85, .95), .08, alpha=.6)
    M["tapa"] = mat("Tapa", (.9, .9, .95), .4)
    M["lata"] = mat("Lata", (.7, .7, .72), .25, metal=1.0)
    M["papel"] = mat("Papel", (.85, .84, .78), .9)
    M["carton"] = mat("Carton", (.5, .38, .22), .95)
    M["tronco"] = mat("Tronco_Seco", (.2, .17, .15), 1.0, ruido=(0.05, (.09, .08, .07)))
    M["pasto"] = mat("Pasto_Seco", (.35, .3, .18), 1.0)
    M["hoja"] = mat("Hoja_Seca", (.32, .22, .09), 1.0)
    M["barril"] = mat("Barril_Oxido", (.35, .15, .06), .6, metal=.6, ruido=(0.03, (.18, .08, .04)))
    M["piedra"] = mat("Piedra", (.3, .3, .3), 1.0, ruido=(0.02, (.17, .17, .17)))
    M["madera"] = mat("Madera", (.28, .18, .1), .9, ruido=(0.04, (.15, .09, .05)))
    M["metal"] = mat("Metal", (.2, .2, .22), .5, metal=1.0)
    M["oxido"] = mat("Metal_Oxidado", (.38, .16, .06), .85, metal=.4, ruido=(0.03, (.17, .07, .03)))
    M["plastico"] = mat("Plastico_Destenido", (.55, .45, .16), .8, ruido=(0.03, (.3, .25, .12)))
    M["azul"] = mat("Bote_Azul", (.1, .3, .9), .5)
    M["amar"] = mat("Bote_Amarillo", (.95, .8, .1), .5)
    M["rojo"] = mat("Bote_Rojo", (.85, .1, .1), .5)
    M["blanco"] = mat("Blanco", (.9, .9, .9), .6)
    M["lampara"] = mat("Lampara_Apagada", (.25, .25, .22), .3)
    M["cerro"] = mat("Cerros", (.32, .31, .3), 1.0)
    M["edif"] = [mat("Edif_%d" % i, c, .95) for i, c in enumerate(
        [(.42, .4, .38), (.36, .35, .34), (.46, .43, .4), (.33, .33, .35)])]
    return M


# ---------------------------------------------------------------- modelos
def make_basura(M, tipo, x, y, z, idx):
    g = Grupo("Residuo_%03d" % idx, x, y, z, carpeta="Residuos")
    a = random.uniform(0, math.pi)
    dx, dy = math.cos(a), math.sin(a)
    if tipo == 0:      # botella PET
        eje(g, "PET", (-dx * .11, -dy * .11, .05), (dx * .11, dy * .11, .05), .1, M["pet"], mov=True)
        eje(g, "tapa", (dx * .11, dy * .11, .05), (dx * .14, dy * .14, .05), .045, M["tapa"], mov=True)
    elif tipo == 1:    # lata
        eje(g, "lata", (-dx * .06, -dy * .06, .04), (dx * .06, dy * .06, .04), .08, M["lata"], mov=True)
    elif tipo == 2:    # papel arrugado
        esfera(g, "papel", (0, 0, .06), (.2, .16, .1), M["papel"], yaw=math.degrees(a), mov=True)
    else:              # llanta
        cilindro(g, "llanta", (0, 0, .09), .5, .18, M["negro"], mov=True)
        cilindro(g, "hueco", (0, 0, .1), .26, .19, M["lecho"], mov=True)


def make_bolsa(M, x, y, z, idx):
    g = Grupo("Bolsa_%02d" % idx, x, y, z, carpeta="Residuos")
    esfera(g, "bolsa", (0, 0, .22), (.6, .54, .48), M["bolsa"], yaw=random.uniform(0, 180), mov=True)
    cono(g, "nudo", (0, 0, .5), .14, .15, M["bolsa"], mov=True)


def make_arbol_seco(M, x, y, idx):
    h = random.uniform(2.0, 3.2)
    g = Grupo("ArbolSeco_%02d" % idx, x, y, 0, yaw=random.uniform(0, 360), carpeta="Vegetacion")
    eje(g, "tronco", (0, 0, 0), (0, 0, h), .3, M["tronco"])
    for i in range(random.randint(4, 6)):
        t = random.uniform(.5, 1.0)
        a = random.uniform(0, 6.28)
        L = random.uniform(.8, 1.5)
        z0 = h * random.uniform(.55, .95)
        d = (math.sin(t) * math.cos(a), math.sin(t) * math.sin(a), math.cos(t))
        eje(g, "rama", (0, 0, z0), (d[0] * L, d[1] * L, z0 + d[2] * L), .09, M["tronco"])
        if random.random() < .6:
            eje(g, "ramita", (d[0] * L, d[1] * L, z0 + d[2] * L),
                (d[0] * L * 1.4, d[1] * L * 1.4 + .2, z0 + d[2] * L + .4), .04, M["tronco"])


def make_banca(M, nombre, x, y, yaw, rota):
    g = Grupo(nombre, x, y, 0, yaw=yaw, carpeta="Props_Parque")
    box(g, "asiento", (0, 0, .45), (1.5, .42, .06), M["madera"], roll=(12 if rota else 0))
    box(g, "respaldo", (0, -.2, .75), (1.5, .06, .4), M["madera"], roll=(28 if rota else 0), pitch=-8)
    box(g, "pata_i", (-.6, 0, .22), (.08, .4, .44), M["metal"])
    if not rota:
        box(g, "pata_d", (.6, 0, .22), (.08, .4, .44), M["metal"])
    else:
        box(g, "pata_caida", (.9, .6, .04), (.08, .4, .44), M["metal"], yaw=40, roll=80)


def make_farol(M, nombre, x, y, doblado):
    g = Grupo(nombre, x, y, 0, carpeta="Props_Parque")
    top = (.7, 0, 3.0) if doblado else (0, 0, 3.2)
    eje(g, "poste", (0, 0, 0), top, .1, M["metal"])
    esfera(g, "foco", (top[0], top[1], top[2] + .1), (.4, .4, .4), M["lampara"])


def make_roca(M, x, y, s, idx):
    g = Grupo("Roca_%02d" % idx, x, y, 0, carpeta="Props_Parque")
    esfera(g, "roca", (0, 0, 0), (s * 2, s * 2 * random.uniform(.7, 1.2), s * 1.2), M["piedra"],
           yaw=random.uniform(0, 180))


def make_columpio(M, x, y, yaw):
    g = Grupo("Columpio_Roto", x, y, 0, yaw=yaw, carpeta="Juegos_Rotos")
    ox = M["oxido"]
    eje(g, "viga", (-1.75, 0, 2.2), (1.75, 0, 2.2), .1, ox)
    for sx in (-1.6, 1.6):
        for sy in (-1, 1):
            if sx == 1.6 and sy == 1:
                continue     # pata que se cayo
            eje(g, "pata", (sx, 0, 2.2), (sx, sy * .8, 0), .09, ox)
    eje(g, "pata_caida", (1.5, .3, .06), (3.6, 1.3, .06), .09, ox)
    eje(g, "cadena", (-.6, 0, 2.2), (-.6, 0, .75), .025, M["metal"])
    box(g, "asiento_colgando", (-.45, 0, .65), (.5, .2, .04), M["madera"], pitch=-45)
    for dx in (.45, .75):
        eje(g, "cadena_corta", (dx, 0, 2.2), (dx, 0, 1.5), .025, M["metal"])
    box(g, "asiento_caido", (.9, .7, .03), (.5, .2, .04), M["madera"], yaw=35)


def make_resbaladilla(M, x, y, yaw):
    g = Grupo("Resbaladilla_Rota", x, y, 0, yaw=yaw, carpeta="Juegos_Rotos")
    ox, pl = M["oxido"], M["plastico"]
    eje(g, "riel_escalera", (-.35, -.85, 0), (-.35, -.5, 1.4), .06, ox)
    eje(g, "riel_roto", (.35, -.85, 0), (.35, -.67, .7), .06, ox)
    for i, z in enumerate((.3, .6, .9, 1.2)):
        if i == 2:
            continue         # escalon faltante
        box(g, "escalon", (0, -.85 + .35 * z / 1.4, z), (.7, .06, .04), ox)
    box(g, "plataforma", (0, 0, 1.4), (.9, 1.0, .06), pl)
    eje(g, "poste", (-.4, .45, 0), (-.4, .45, 1.4), .08, ox)
    eje(g, "poste_chueco", (.4, .45, 0), (.65, .45, 1.38), .08, ox)
    tabla(g, "rampa_alta", (0, .5, 1.4), (0, 1.59, .78), .6, .05, pl)
    tabla(g, "borde_rampa", (.3, .5, 1.45), (.3, 1.59, .83), .06, .1, pl)
    tabla(g, "rampa_doblada", (.05, 1.95, .55), (.25, 2.9, .05), .6, .05, pl)


def make_subibaja(M, x, y, yaw):
    g = Grupo("Subibaja_Roto", x, y, 0, yaw=yaw, carpeta="Juegos_Rotos")
    ox, pl = M["oxido"], M["plastico"]
    box(g, "soporte", (0, 0, .22), (.5, .12, .44), ox)
    eje(g, "eje", (-.25, 0, .45), (.25, 0, .45), .14, ox)
    tabla(g, "tabla_alta", (0, -.1, .5), (0, 1.8, .98), .3, .07, pl)
    eje(g, "manija", (-.15, 1.5, .98), (.15, 1.5, .98), .03, ox)
    tabla(g, "tabla_rota", (0, -1.0, .07), (.4, -2.1, .07), .3, .07, pl)


def make_barras(M, x, y, yaw):
    g = Grupo("Barras_Rotas", x, y, 0, yaw=yaw, carpeta="Juegos_Rotos")
    ox = M["oxido"]
    eje(g, "poste", (-1.2, 0, 0), (-1.2, 0, 2.4), .1, ox)
    eje(g, "poste_chueco", (1.2, 0, 0), (1.5, 0, 2.35), .1, ox)
    eje(g, "barra_doblada", (-1.2, 0, 2.35), (0, 0, 2.05), .06, ox)
    eje(g, "barra_baja", (-1.2, 0, 1.1), (-.85, 0, 1.1), .05, ox)
    eje(g, "barra_caida", (-1.0, -.7, .06), (1.2, -.4, .06), .05, ox)


def make_eliptico(M, x, y, yaw, idx):
    g = Grupo("Eliptico_Roto_%d" % idx, x, y, 0, yaw=yaw, carpeta="Juegos_Rotos")
    ox = M["oxido"]
    box(g, "base", (0, 0, .05), (.8, 1.5, .1), ox)
    eje(g, "columna", (0, .45, .1), (0, .35, 1.7), .12, M["metal"])
    eje(g, "disco", (-.05, .45, .5), (.05, .45, .5), .56, ox)
    eje(g, "manija", (-.28, .4, 1.0), (-.28, -.05, 1.6), .06, ox)
    eje(g, "brazo_pedal", (-.28, .45, .45), (-.28, -.35, .3), .05, ox)
    box(g, "pedal", (-.28, -.35, .3), (.22, .4, .05), ox)
    eje(g, "brazo_roto", (.28, .4, .5), (.28, .55, .4), .05, ox)
    box(g, "pedal_caido", (.55, -.6, .04), (.22, .4, .05), ox, yaw=35)


def make_letrero(M, texto, x, y, yaw):
    g = Grupo("Letrero", x, y, 0, yaw=yaw, carpeta="Props_Parque")
    eje(g, "poste_i", (-.7, 0, 0), (-.7, 0, 1.6), .08, M["madera"])
    eje(g, "poste_d", (.7, 0, 0), (.7, 0, 1.6), .08, M["madera"])
    box(g, "tablero", (0, 0, 1.4), (1.8, .06, .9), M["madera"], roll=6)
    try:
        t = _spawn(unreal.TextRenderActor, V(*g.p((0, -.05, 1.4))), R(0, 0, yaw + 180))
        comp = t.text_render
        comp.set_text(texto)
        comp.set_world_size(11.0)
        comp.set_text_render_color(unreal.Color(220, 220, 215, 255))
        try:
            comp.set_horizontal_alignment(unreal.HorizTextAligment.EHTA_CENTER)
        except Exception:
            pass
        t.set_actor_label("Letrero_Texto")
        t.set_folder_path(CARPETA + "/Props_Parque")
    except Exception as e:
        log("AVISO letrero de texto: %s" % e)


# ---------------------------------------------------------------- secciones
def s_terreno(M):
    g = Grupo("Terreno", carpeta="Terreno")
    box(g, "Parque_Frente", (0, -24.55, -.5), (90, 40.9, 1), M["suelo"])
    box(g, "Orilla_Fondo", (0, 24.55, -.5), (90, 40.9, 1), M["suelo"])
    box(g, "Muro_Frente", (0, -4.0, -.45), (90, .2, .9), M["muro"])
    box(g, "Muro_Fondo", (0, 4.0, -.45), (90, .2, .9), M["muro"])
    box(g, "Lecho_Rio", (0, 0, -.9), (90, 7.8, .2), M["lecho"])
    gr = Grupo("Rio", carpeta="Rio")
    box(gr, "Agua_Rio", (0, 0, -.28), (90, 7.8, .06), M["agua"])
    for i in range(D(12)):      # manchas de lodo flotando
        esfera(gr, "Lodo_%02d" % i, (random.uniform(-14, 14), random.uniform(-3.2, 3.2), -.24),
               (random.uniform(1.5, 3), random.uniform(1, 2), .05), M["lodo"], yaw=random.uniform(0, 180))
    for i, (ox_, oy_) in enumerate(((-4, 1.2), (3, -1.8), (9, 2.0))):   # aceite
        esfera(gr, "Aceite_%d" % i, (ox_, oy_, -.235), (2.6, 1.8, .03), M["aceite"], yaw=random.uniform(0, 180))
    gm = Grupo("Relieve", carpeta="Terreno/Relieve")
    for i in range(D(16)):      # monticulos de tierra reseca
        x = random.uniform(-30, 30)
        y = random.choice([-1, 1]) * random.uniform(7, 30)
        esfera(gm, "Monticulo_%02d" % i, (x, y, -.3), (random.uniform(3, 8), random.uniform(3, 6), random.uniform(1.0, 1.8)),
               M["suelo"], yaw=random.uniform(0, 180))
    for i in range(D(26)):      # grietas y manchas oscuras
        esfera(gm, "Grieta_%02d" % i, (random.uniform(-16, 16), random.uniform(-16, -5), .01),
               (random.uniform(1, 2.6), random.uniform(.5, 1.2), .02), M["lecho"], yaw=random.uniform(0, 180))
    for i in range(D(30)):      # hojas secas
        box(gm, "Hoja_%02d" % i, (random.uniform(-16, 16), random.uniform(-16, -5), .02),
            (.18, .12, .01), M["hoja"], yaw=random.uniform(0, 180))


def s_fondo(M):
    g = Grupo("Fondo", carpeta="Fondo")
    for i in range(9):
        esfera(g, "Cerro_%d" % i, (-40 + i * 10, 70, -3), (18, 10, 7 + random.random() * 4), M["cerro"])
    for i in range(18):
        w, h = random.uniform(3, 5), random.uniform(4, 12)
        box(g, "Edificio_%02d" % i, (-34 + i * 4, random.uniform(48, 56), h / 2), (w, 3, h), random.choice(M["edif"]))
    box(g, "Torre_Parroquia", (-20, 44, 7), (3, 3, 14), M["edif"][0])
    cono(g, "Torre_Techo", (-20, 44, 15.5), 4.8, 3, M["edif"][1])


def s_puente(M):
    g = Grupo("Puente", carpeta="Puente")
    box(g, "Losa", (20, 0, 2.6), (3.4, 14, .5), M["muro"])
    for sy in (-1, 1):
        box(g, "Pretil", (20 + sy * 1.6, 0, 3.2), (.25, 14, .8), M["muro"])
    for py in (-3.2, 0, 3.2):
        box(g, "Pila", (20, py, 1.2), (2.6, .9, 2.9), M["muro"])


def s_props(M):
    make_banca(M, "Banca_Rota_1", 6, -8, 17, True)
    make_banca(M, "Banca_Rota_2", -11, -9.5, -23, True)
    make_farol(M, "Farol_Apagado_1", 3, -6, False)
    make_farol(M, "Farol_Doblado_2", -9, -6, True)
    make_letrero(M, "PROHIBIDO\nTIRAR BASURA", -13, -6.5, 15)
    # tuberia de desague con lodo
    g = Grupo("Tuberia", -7, 6.5, 0, carpeta="Props_Parque")
    eje(g, "tubo", (0, 2.5, .1), (0, -2.5, .1), 1.1, M["metal"])
    eje(g, "boca", (0, -2.4, .1), (0, -2.7, .1), 1.3, M["metal"])
    esfera(g, "lodo_charco", (0, -3.1, -.24), (4.8, 3.2, .06), M["lodo"])
    esfera(g, "lodo_pared", (0, -2.2, .1), (1.2, 1.6, .05), M["lodo"])
    # barandal roto
    gb = Grupo("Barandal", carpeta="Props_Parque")
    for x in range(-14, 15, 2):
        if random.random() < .4:
            continue
        torcido = random.uniform(-.25, .25)
        eje(gb, "poste_%d" % x, (x, -4.4, 0), (x + torcido, -4.4 + random.uniform(-.2, .2), .9), .08, M["metal"])
    box(gb, "riel", (-4, -4.4, .8), (18, .08, .1), M["metal"], yaw=1)
    # barriles oxidados y rocas
    for i in range(4):
        gbar = Grupo("Barril_%d" % i, random.uniform(-14, 14), random.uniform(-14, -10), 0, carpeta="Props_Parque")
        if random.random() < .5:
            cilindro(gbar, "barril", (0, 0, .45), .64, .9, M["barril"])
        else:
            eje(gbar, "barril_caido", (-.45, 0, .32), (.45, 0, .32), .64, M["barril"])
    for i in range(D(14)):
        yy = random.choice([random.uniform(-3.5, 3.5), random.uniform(-12, -5)])
        make_roca(M, random.uniform(-14, 14), yy, random.uniform(.2, .6), i)


def s_juegos(M):
    make_columpio(M, -8, -13, 6)
    make_resbaladilla(M, 9.5, -12.5, 17)
    make_subibaja(M, -13.5, -9.5, 70)
    make_barras(M, 2.5, -14, 0)
    make_eliptico(M, -2, -13.5, 23, 1)
    make_eliptico(M, 13.5, -7, -29, 2)
    g = Grupo("Charco", carpeta="Juegos_Rotos")
    esfera(g, "Charco_Columpios", (-8, -12.4, .015), (4.8, 2.6, .03), M["lodo"])


def s_vegetacion(M):
    for i in range(D(10)):
        make_arbol_seco(M, random.uniform(-16, 16), random.choice([-14, -12, 8, 11, 14]) + random.uniform(-1, 1), i)
    g = Grupo("Pasto", carpeta="Vegetacion")
    for i in range(D(40)):
        cono(g, "Pasto_%02d" % i, (random.uniform(-15, 15), random.choice([-1, 1]) * random.uniform(5, 14), .15),
             .18, .3, M["pasto"])


def s_residuos(M):
    idx = 0
    for i in range(D(70)):
        x = random.uniform(-13, 13)
        y = random.choice([random.uniform(-13, -4.8), random.uniform(-3.6, 3.6), random.uniform(5, 12)])
        z = -.24 if abs(y) < 3.9 else 0.0
        make_basura(M, i % 4, x, y, z, idx)
        idx += 1
    for i in range(D(10)):
        make_bolsa(M, random.uniform(-13, 13), random.choice([random.uniform(-12, -5), random.uniform(5, 11)]), 0, i)
    # montones de basura junto a los barriles y cajas de carton
    for k in range(3):
        cx, cy = random.uniform(-12, 12), random.uniform(-13, -9)
        for j in range(5):
            make_bolsa(M, cx + random.uniform(-.8, .8), cy + random.uniform(-.8, .8), 0, 100 + k * 10 + j)
        g = Grupo("Caja_Carton_%d" % k, cx + 1.2, cy, 0, yaw=random.uniform(0, 180), carpeta="Residuos")
        box(g, "caja", (0, 0, .15), (.5, .4, .3), M["carton"], mov=True)


def s_personaje(M):
    g = Grupo("Itzel", -3, -8, 0, carpeta="Personaje")
    for s in (-1, 1):
        cilindro(g, "pierna", (s * .08, 0, .32), .13, .64, M["jean"], mov=True)
        box(g, "zapato", (s * .08, .04, .04), (.13, .24, .08), M["negro"], mov=True)
        eje(g, "brazo", (s * .21, 0, 1.05), (s * .34, 0, .67), .1, M["morado"], mov=True)
        esfera(g, "guante", (s * .34, 0, .67), (.15, .15, .15), M["amarillo"], mov=True)
        esfera(g, "ojo", (s * .06, .15, 1.32), (.045, .045, .045), M["negro"], mov=True)
    cilindro(g, "playera", (0, 0, .9), .4, .5, M["morado"], mov=True)
    esfera(g, "cabeza", (0, 0, 1.3), (.34, .34, .34), M["piel"], mov=True)
    esfera(g, "pelo", (0, -.03, 1.35), (.38, .38, .34), M["pelo"], mov=True)
    esfera(g, "cola", (0, -.2, 1.22), (.1, .1, .27), M["pelo"], mov=True)
    esfera(g, "bolsa", (.35, .05, .5), (.26, .21, .33), M["bolsa"], mov=True)
    for nombre, x, col in (("Bote_Azul", -.5, M["azul"]), ("Bote_Amarillo", .7, M["amar"]), ("Bote_Rojo", 1.9, M["rojo"])):
        gb = Grupo(nombre, x, -8.5, 0, carpeta="Botes")
        cilindro(gb, "cuerpo", (0, 0, .4), .6, .8, col, mov=True)
        cilindro(gb, "tapa", (0, 0, .83), .68, .07, col, mov=True)
        box(gb, "etiqueta", (0, .3, .45), (.22, .015, .28), M["blanco"], mov=True)


def s_marcadores(M):
    try:
        ps = _spawn(unreal.PlayerStart, V(-3, -8, .9), R(0, 0, 90))
        ps.set_actor_label("PLAYER_START")
        ps.set_folder_path(CARPETA + "/Game_Markers")
    except Exception as e:
        log("AVISO PlayerStart: %s" % e)
    for i in range(5):
        try:
            tp = _spawn(unreal.TargetPoint, V(-16, -3 + i * 1.5, -.2))
            tp.set_actor_label("SPAWN_Residuo_%d" % (i + 1))
            tp.set_folder_path(CARPETA + "/Game_Markers")
        except Exception as e:
            log("AVISO TargetPoint: %s" % e)
    for nombre, x in (("Azul", -.5), ("Amarillo", .7), ("Rojo", 1.9)):
        try:
            tb = _spawn(unreal.TriggerBox, V(x, -8.5, .5))
            tb.set_actor_scale3d(unreal.Vector(.8, .8, 1.2))
            tb.set_actor_label("ZONE_Bote_" + nombre)
            tb.set_folder_path(CARPETA + "/Game_Markers")
        except Exception as e:
            log("AVISO TriggerBox: %s" % e)


def _set(obj, prop, valor):
    try:
        obj.set_editor_property(prop, valor)
        return True
    except Exception:
        return False


def s_luces(M):
    carp = CARPETA + "/Iluminacion"
    sol = _spawn(unreal.DirectionalLight, V(0, 0, 12), R(0, -48, 35))
    sol.set_actor_label("Sol_Nublado")
    sol.set_folder_path(carp)
    c = sol.get_component_by_class(unreal.DirectionalLightComponent)
    _set(c, "intensity", 2.4)
    _set(c, "light_color", unreal.Color(205, 210, 225, 255))
    _set(c, "atmosphere_sun_light", True)

    sa = _spawn(unreal.SkyAtmosphere, V(0, 0, 0))
    sa.set_actor_label("Atmosfera")
    sa.set_folder_path(carp)

    sl = _spawn(unreal.SkyLight, V(0, 0, 3))
    sl.set_actor_label("Luz_Cielo")
    sl.set_folder_path(carp)
    _set(sl.get_component_by_class(unreal.SkyLightComponent), "real_time_capture", True)

    fog = _spawn(unreal.ExponentialHeightFog, V(0, 0, 0))
    fog.set_actor_label("Niebla_Gris")
    fog.set_folder_path(carp)
    fc = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    _set(fc, "fog_density", 0.035)
    _set(fc, "fog_height_falloff", 0.25)
    if not _set(fc, "fog_inscattering_color", unreal.LinearColor(.42, .44, .47, 1)):
        _set(fc, "fog_inscattering_luminance", unreal.LinearColor(.42, .44, .47, 1))
    _set(fc, "volumetric_fog", True)

    try:
        nubes = _spawn(unreal.VolumetricCloud, V(0, 0, 0))
        nubes.set_actor_label("Nubes")
        nubes.set_folder_path(carp)
    except Exception:
        pass

    try:     # post proceso: colores apagados y frios
        pp = _spawn(unreal.PostProcessVolume, V(0, 0, 0))
        pp.set_actor_label("PostProceso_Gris")
        pp.set_folder_path(carp)
        _set(pp, "unbound", True)
        st = pp.get_editor_property("settings")
        _set(st, "override_color_saturation", True)
        _set(st, "color_saturation", unreal.Vector4(.62, .62, .65, 1.0))
        _set(st, "override_color_contrast", True)
        _set(st, "color_contrast", unreal.Vector4(1.05, 1.05, 1.05, 1.0))
        _set(st, "override_vignette_intensity", True)
        _set(st, "vignette_intensity", 0.55)
        pp.set_editor_property("settings", st)
    except Exception as e:
        log("AVISO post proceso: %s" % e)


CAMARAS = [
    ("01_general", (18, -26, 11), (0, 0, 1)),
    ("02_itzel_y_botes", (-.5, -5.6, 1.7), (-2.2, -8.2, 1.0)),
    ("03_rio_cenital", (0, -2, 22), (0, 0, 0)),
    ("04_tuberia_contaminacion", (-2, -2, 3.2), (-7, 5.5, .2)),
    ("05_panoramica", (-26, -12, 6), (4, 0, 1)),
]


def s_camaras(M):
    if not CREAR_CAMARAS:
        return
    for nombre, pos, tgt in CAMARAS:
        cam = _spawn(unreal.CameraActor, V(*pos))
        rot = unreal.MathLibrary.find_look_at_rotation(V(*pos), V(*tgt))
        cam.set_actor_rotation(rot, False)
        cam.set_actor_label("Camara_" + nombre)
        cam.set_folder_path(CARPETA + "/Camaras")
        try:
            _set(cam.get_component_by_class(unreal.CameraComponent), "field_of_view", 65.0)
        except Exception:
            pass


# ---------------------------------------------------------------- principal
def limpiar():
    clases = []
    if BORRAR_LUCES_PREVIAS:
        for nombre in ("DirectionalLight", "SkyAtmosphere", "SkyLight", "ExponentialHeightFog",
                       "VolumetricCloud", "PostProcessVolume"):
            if hasattr(unreal, nombre):
                clases.append(getattr(unreal, nombre))
    borrados = 0
    for a in _all_actors():
        try:
            carpeta = str(a.get_folder_path())
        except Exception:
            carpeta = ""
        if (LIMPIAR_ESCENA and carpeta.startswith(CARPETA)) or \
                (clases and any(isinstance(a, c) for c in clases)):
            a.destroy_actor()
            borrados += 1
    log("Actores previos eliminados: %d" % borrados)


def main():
    random.seed(SEED)
    limpiar()
    M = materiales()
    secciones = [
        ("Terreno y rio", s_terreno), ("Fondo de ciudad", s_fondo), ("Puente", s_puente),
        ("Props del parque", s_props), ("Juegos de parque rotos", s_juegos),
        ("Vegetacion seca", s_vegetacion), ("Residuos", s_residuos),
        ("Itzel y botes", s_personaje), ("Marcadores de juego", s_marcadores),
        ("Iluminacion", s_luces), ("Camaras", s_camaras),
    ]
    tarea_cm = None
    try:
        tarea_cm = unreal.ScopedSlowTask(len(secciones), "Construyendo EcoSort - Nivel 1...")
    except Exception:
        tarea_cm = None
    if tarea_cm is not None:
        with tarea_cm as tarea:
            tarea.make_dialog(True)
            for nombre, fn in secciones:
                if tarea.should_cancel():
                    log("Cancelado por el usuario")
                    break
                tarea.enter_progress_frame(1, nombre)
                log(nombre)
                fn(M)
    else:
        for nombre, fn in secciones:
            log(nombre)
            fn(M)
    if GUARDAR_NIVEL:
        try:
            unreal.EditorLevelLibrary.save_current_level()
        except Exception as e:
            log("AVISO no se pudo guardar el nivel: %s" % e)
    log("LISTO. Actores de malla creados: %d" % _CONT[0])


main()
