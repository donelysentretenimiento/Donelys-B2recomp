"""Biblioteca de mods de la edición mejorada: monta mods\\ a partir de las descargas originales, sin tocarlas.

    deps\\python\\python.exe -I tools\\mods\\biblioteca.py            (desde la carpeta de B2recomp)

Lee mods_extraidos\\<código>\\ (lo que hay en mods_originales\\, descomprimido) y escribe:
  - mods\\<carpeta>\\romfs\\... o exefs\\... con la estructura que leen runtime\\nnfs.c (RomFS) y scripts\\pchtxt.py (código);
  - mods\\<carpeta>\\mod.ini: nombre, versión, autor, enlace, categoría, requisitos, conflictos esperados (la ventana Mods...);
  - las fusiones de los .csv de combos (Buffed Weapons solo cambia valores ATK_: se aplican sobre el otro mod);
  - config\\perfiles\\*.ini: los perfiles de la ventana Mods... ([mods] <carpeta> = on | off);
  - docs\\edicion-mejorada\\inventario_mods.csv: cada archivo con su SHA-256, ruta en RomFS y paquete .cpk que sustituye.
Una carpeta de mods\\ que ya existe se reemplaza solo si lleva un mod.ini con 'generado = biblioteca.py' (las tuyas, nunca).
"""
import csv, hashlib, os, shutil, sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
import cpk  # noqa: E402

EXT = os.path.join(ROOT, 'mods_extraidos')
MODS = os.path.join(ROOT, 'mods')
GB = 'https://gamebanana.com/mods/'

# carpeta, código, origen (en mods_extraidos), destino ('romfs' = tal cual; 'romfs/pl' = archivos sueltos a esa ruta;
# 'exefs'), categoría, versión, autor, ficha, requisitos, notas
LIB = [
    ('DS4Prompts', 'A01', 'A01_prompts_ds4/DS4Prompts', 'mod', 'Interfaz', '1.0.0', 'Timo654', 360121, '',
     'Iconos de PlayStation. El juego lo usa solo si controllers.ini [general] button_prompts lo elige (o el mando conectado).'),
    ('XBOXPrompts', 'A01', 'A01_prompts_xbox/XBOXPrompts', 'mod', 'Interfaz', '1.0.0', 'Timo654', 360121, '',
     'Iconos de Xbox. Mismo mecanismo que DS4Prompts; los dos pueden estar activados.'),
    ('A02 Witch of Genesis', 'A02', 'A02_witch_of_genesis/Switch/The Witch of Genesis', 'mod', 'Personajes', '2.0',
     'DniweTamp', 526599, '', 'Sustituye el traje de Bayonetta 1 (pl0010-pl0013, pl001a) y los pendientes de cinemática (es016d).'),
    ('A02b Witch of Genesis - Naive Angel', 'A02', 'A02_witch_of_genesis/Switch/The Witch of Genesis - Naive Angel Addon',
     'mod', 'Personajes', '2.0', 'DniweTamp', 526599, 'A02 Witch of Genesis',
     'Complemento: cambia solo pl0010 sobre A02 (gana por orden alfabético).'),
    ('A03 One Eyed Witch', 'A03', 'A03_one_eyed_witch/One Eyed Witch/contents/01007960049A0000', 'mod', 'Personajes', '1.0',
     'CreativeHandle / pearl23', 657021, '', 'Sustituye Gekka C (pl004d) y la cabeza de Gekka A/B (pl0027).'),
    ('A04 All 4 One', 'A04', 'A04_all_4_one/All 4 One (B3)', 'mod', 'Armas', '1.2', 'Fool_Arcana', 609336, '',
     'Armas de Jeanne estilo Bayonetta 3: juego (wp002a), aliada (pl0224), galería (um102a), cinemáticas (es0214, es0215).'),
    ('A05 Prototype Bayonetta 1', 'A05', 'A05_prototype_bayo1', 'romfs/pl', 'Personajes', 's/v', 'pearl23', 582884, '',
     'Sustituye el traje antiguo (Old Bayonetta, pl0020-pl0022). Sin el pelo largo.'),
    ('A06 Soul Cereza B1', 'A06', 'A06_soul_modpack/Soul Cereza (B1)', 'mod', 'Personajes', '1.11', 'Fool_Arcana', 609675, '',
     'Cereza (traje antiguo) con pelo de Bayonetta 1. Una sola variante de Cereza a la vez.'),
    ('A06 Soul Cereza B2', 'A06', 'A06_soul_modpack/Soul Cereza (B2)', 'mod', 'Personajes', '1.11', 'Fool_Arcana', 609675, '',
     'Cereza (traje antiguo), variante Bayonetta 2.'),
    ('A06 Soul Cereza B3', 'A06', 'A06_soul_modpack/Soul Cereza (B3)', 'mod', 'Personajes', '1.11', 'Fool_Arcana', 609675, '',
     'Cereza (traje antiguo), variante Bayonetta 3.'),
    ('A06 Soul Jeanne B1', 'A06', 'A06_soul_modpack/Soul Jeanne (B1)', 'mod', 'Personajes', '1.11', 'Fool_Arcana', 609675, '',
     'Jeanne (traje antiguo) estilo Bayonetta 1. Una sola variante de Jeanne a la vez.'),
    ('A06 Soul Jeanne B2', 'A06', 'A06_soul_modpack/Soul Jeanne (B2)', 'mod', 'Personajes', '1.11', 'Fool_Arcana', 609675, '',
     'Jeanne (traje por defecto) estilo Bayonetta 2: pl0060/pl0061, igual que A07.'),
    ('A06 Soul Jeanne B3', 'A06', 'A06_soul_modpack/Soul Jeanne (B3)', 'mod', 'Personajes', '1.11', 'Fool_Arcana', 609675, '',
     'Jeanne (traje antiguo) estilo Bayonetta 3.'),
    ('A06 Soul Rosa', 'A06', 'A06_soul_modpack/Soul Rosa', 'mod', 'Personajes', '1.11', 'Fool_Arcana', 609675, '',
     'Rosa: cuerpo, cabeza, armadura umbra, Rosa moribunda y cinemáticas. Comparte pl022b con las variantes de Cereza.'),
    ('A07 Platinum Witch', 'A07', 'A07_platinum_witch/The Platinum Witch', 'mod', 'Personajes', 's/v', 'DniweTamp', 702078, '',
     'Jeanne de Bayonetta 3 sobre el traje Biker (por defecto): pl0060, pl0061, pl0220, pl0223 y galería (umc100/104/132).'),
    ('A08 Bayonetta 1 Face', 'A08', 'A08_bayo1_face/Bayonetta 1 face - Switch', 'mod', 'Personajes', 's/v', 'DniweTamp', 714754,
     '', 'Cara de Bayonetta 1 en el traje de Bayonetta 1 (pl0013): excluyente con A02.'),
    ('B01 Longer Witch Time', 'B01', 'B01_longer_witch_time/Longer Witch Time', 'mod', 'Jugabilidad', '1.2.0', 'DniweTamp',
     582621, '', 'Parche de código (.pchtxt) para el build F19B2E01E1794F93: necesita Aplicar (recompilar).'),
    ('B02 Colour My World V9', 'B02', 'B02_colour_my_world/CMW MOVESET V9 FOR SWITCH', 'romfs/pl', 'Armas', 'V9',
     'YouWhenMe / DniweTamp', 427401, '', 'Movimientos de Colour My World sobre Love is Blue (pl001f, plcombo_0000).'),
    ('B03 CMW Gun Recolor', 'B03', 'B03_cmw_gun_recolor/Colour My World retexture for Love is Blue guns', 'mod', 'Armas', 's/v',
     'DniweTamp', 324234, '', 'Aspecto de Colour My World en las pistolas Love is Blue (wp0000): excluyente con B07.'),
    ('B04 Buffed Weapons', 'B04', 'B04_buffed_weapons/Buffed Weapon Damage', 'mod', 'Jugabilidad', 's/v', 'Hias', 712639, '',
     'Más daño de armas (17 plcombo .csv, solo valores ATK_) y de Wicked Weaves (38 archivos et). Rosa no incluida.'),
    ('B05 Killgore SaiFung Moveset', 'B05', 'B05_killgore_saifung', 'mod', 'Armas', 's/v', 'pearl23', 640396,
     'Truco «Arma híbrida Killgore/Sai-Fung»', 'Movimientos del arma híbrida descartada (plcombo_1515). Sin el truco no se ve.'),
    ('B06 Beta Tartarus Moveset', 'B06', 'B06_beta_tartarus/Mod', 'romfs/pl', 'Armas', '1.0.1', 'AlexanderN', 489579,
     'Arma 9 en la ranura A (truco «Gatling»)', 'Movimientos de βTartarus (plcombo_1414). La partida incluida NO se importa.'),
    ('B07 Scarborough Fair over LiB', 'B07', 'B07_scarborough_over_lib/SF over LiB', 'mod', 'Armas', '1.0', 'pearl23', 656969,
     '', 'Scarborough Fair en lugar de Love is Blue: wp0000, galería um1000, cinemáticas es0152/es0153.'),
    ('B08 Reworked Kafka', 'B08', 'B08_reworked_kafka/Reworked Kafka', 'mod', 'Armas', '1.0', 'oothiee', 699547, '',
     'Combos y daño aéreo de Kafka (plcombo_0b0b).'),
    ('C01 Triple Alraune (3)', 'C01', 'C01_triple_alraune/Triple Threat Alraune/Three Alraunes', 'mod', 'Desafíos', '1.0',
     'Fool_Arcana', 687081, '', 'Tres Alraune a la vez (ph5/p560). Excluyente con la variante de seis.'),
    ('C01 Triple Alraune (6)', 'C01', 'C01_triple_alraune/Triple Threat Alraune/Six Alraunes', 'mod', 'Desafíos', '1.0',
     'Fool_Arcana', 687081, '', 'Seis Alraune a la vez (ph5/p560).'),
    ('C02 Better Balder Moveset', 'C02', 'C02_better_balder/pl', 'romfs/pl', 'Personajes', 's/v', 'AlexanderN', 429500,
     'Truco «Jugar como Balder»', 'Combos de Balder (plcombo_3030).'),
    ('C03 Chris Redfield', 'C03', 'C03_chris_redfield/Daemon Collection Chris Redfield', 'mod', 'Personajes', '1.0.0',
     'Fool_Arcana', 636154, '', 'Sustituye el traje de monja (pl004a, pl004b).'),
    ('C03 Chris Redfield (sin camiseta)', 'C03', 'C03_chris_redfield/Daemon Collection Chris Redfield (Shirtless)', 'mod',
     'Personajes', '1.0.0', 'Fool_Arcana', 636154, '', 'Variante sin camiseta (mismos archivos).'),
]
# fusiones de .csv: (carpeta, csv, mod con combos propios, notas). Buffed Weapons cambia solo ATK_ de líneas del original:
# esas líneas, si el otro mod las dejó como en el juego, toman el valor de Buffed; las demás quedan como las escribió su autor.
FUSIONS = [
    ('Z1 Fusion Buffed + CMW', 'plcombo_0000.csv', 'B02 Colour My World V9'),
    ('Z2 Fusion Buffed + Kafka', 'plcombo_0b0b.csv', 'B08 Reworked Kafka'),
    ('Z3 Fusion Buffed + Balder', 'plcombo_3030.csv', 'C02 Better Balder Moveset'),
]
PROFILES = {
    'Juego original': [],
    'Vanilla mejorado': ['DS4Prompts', 'XBOXPrompts'],
    'Bayonetta 3 Experience': ['DS4Prompts', 'XBOXPrompts', 'A02 Witch of Genesis', 'A04 All 4 One', 'A07 Platinum Witch',
                               'A06 Soul Rosa', 'B02 Colour My World V9', 'B03 CMW Gun Recolor'],
    'Combate expandido': ['DS4Prompts', 'XBOXPrompts', 'B01 Longer Witch Time', 'B02 Colour My World V9', 'B04 Buffed Weapons',
                          'B08 Reworked Kafka', 'Z1 Fusion Buffed + CMW', 'Z2 Fusion Buffed + Kafka', 'C01 Triple Alraune (3)'],
    'Contenido experimental': ['DS4Prompts', 'XBOXPrompts', 'B05 Killgore SaiFung Moveset', 'B06 Beta Tartarus Moveset',
                               'C02 Better Balder Moveset', 'C01 Triple Alraune (6)', 'C03 Chris Redfield'],
}
CODE = 'B01 Longer Witch Time'                              # el único que se compila: los demás se leen al arrancar


def sha(p):
    h = hashlib.sha256()
    with open(p, 'rb') as f:
        for b in iter(lambda: f.read(1 << 20), b''):
            h.update(b)
    return h.hexdigest()


def ours(d):
    ini = os.path.join(d, 'mod.ini')
    return os.path.isfile(ini) and 'generado = biblioteca.py' in open(ini, encoding='utf-8').read()


def write_ini(d, **kv):
    with open(os.path.join(d, 'mod.ini'), 'w', encoding='utf-8', newline='\r\n') as f:
        f.write('; Ficha de la biblioteca de mods (tools\\mods\\biblioteca.py). La lee scripts\\pchtxt.py tree.\n[mod]\n')
        for k, v in kv.items():
            f.write('%s = %s\n' % (k, v))
        f.write('generado = biblioteca.py\n')


class _F:
    def __init__(s, p): s.f = open(p, 'rb')
    def read(s, o, n): s.f.seek(o); return s.f.read(n)


def cpk_index():
    rom, idx = os.path.join(ROOT, 'Bayonetta 2 PC', 'romfs'), {}
    for n in sorted(os.listdir(rom)):
        if n.endswith('.cpk'):
            c = cpk.Cpk(_F(os.path.join(rom, n)))
            for e in c.files:
                idx.setdefault(e['path'].lower(), (n, c, e))
    return idx


def romfs_files(d):
    r = os.path.join(d, 'romfs')
    return {os.path.relpath(os.path.join(a, f), r).replace('\\', '/').lower(): os.path.join(a, f)
            for a, _, fs in os.walk(r) for f in fs}


def fuse(orig, buffed, other):
    """Las líneas que Buffed cambió y el otro mod dejó como el original toman el valor de Buffed."""
    o, b, x = (t.splitlines() for t in (orig, buffed, other))   # sin fin de línea: los autores mezclan CRLF y LF
    assert len(o) == len(b), 'Buffed Weapons cambió el número de líneas: la fusión no es segura'
    bump = {lo: lb for lo, lb in zip(o, b) if lo != lb}
    for lo, lb in bump.items():                              # solo valores ATK_ (comprobado: si no, no se fusiona)
        assert [t for t in lo.split(',') if not t.startswith('ATK_')] == [t for t in lb.split(',') if not t.startswith('ATK_')], lo
    key = lambda l: tuple(t for t in l.split(',') if t)        # los autores escriben distinto número de comas vacías
    atk = {key(lo): [t for t in lb.split(',') if t.startswith('ATK_')] for lo, lb in bump.items()}
    out, hits = [], 0
    for l in x:
        new = atk.get(key(l))
        if new:                                              # la misma línea que el juego: sus ATK_ pasan a los de Buffed
            it = iter(new)
            l = ','.join(next(it) if t.startswith('ATK_') else t for t in l.split(','))
            hits += 1
        out.append(l)
    eol = '\r\n' if '\r\n' in other else '\n'
    return eol.join(out) + eol, hits


def main():
    os.makedirs(MODS, exist_ok=True)
    for name, code, src, how, cat, ver, author, gid, req, notes in LIB:
        s, d = os.path.join(EXT, src), os.path.join(MODS, name)
        if not os.path.isdir(s):
            raise SystemExit('falta %s: descomprime mods_originales en mods_extraidos' % s)
        if os.path.exists(d):
            if not ours(d):
                raise SystemExit('%s existe y no lo creó este script: no se toca' % d)
            shutil.rmtree(d)
        if how == 'mod':                                    # carpeta con romfs\ o exefs\ dentro
            shutil.copytree(s, d, ignore=shutil.ignore_patterns('*.txt'), dirs_exist_ok=True)
        else:                                               # archivos sueltos -> d\<how>\
            os.makedirs(os.path.join(d, *how.split('/')))
            for f in os.listdir(s):
                if f.lower().endswith(('.dat', '.csv')):
                    shutil.copy2(os.path.join(s, f), os.path.join(d, *how.split('/'), f))
        write_ini(d, nombre=name[4:] if name[3] == ' ' else name, codigo=code, version=ver, autor=author, enlace=GB + str(gid),
                  categoria=cat, plataforma='Switch 1.2.0 (build F19B2E01E1794F93)', requiere=req or '-',
                  aplicar='recompilar' if name == CODE else 'reiniciar el juego', notas=notes,
                  # los dos de iconos tienen las mismas rutas a propósito: nnfs.c usa solo el que elige controllers.ini
                  **({'conflictos': 'ignorar'} if code == 'A01' else {}),
                  **({'combina_con': req} if name.startswith('A02b') else {}))      # el complemento va encima de A02
    idx = cpk_index()
    for name, f, other in FUSIONS:
        n, c, e = idx['pl/' + f]
        orig = c.read(e).decode('latin1')
        buffed = open(os.path.join(MODS, 'B04 Buffed Weapons', 'romfs', 'pl', f), encoding='latin1', newline='').read()
        mine = open(os.path.join(MODS, other, 'romfs', 'pl', f), encoding='latin1', newline='').read()
        text, hits = fuse(orig, buffed, mine)
        d = os.path.join(MODS, name)
        if os.path.exists(d):
            if not ours(d):
                raise SystemExit('%s existe y no lo creó este script: no se toca' % d)
            shutil.rmtree(d)
        os.makedirs(os.path.join(d, 'romfs', 'pl'))
        with open(os.path.join(d, 'romfs', 'pl', f), 'w', encoding='latin1', newline='') as w:
            w.write(text)
        write_ini(d, nombre=name[3:], codigo='Z', version='local', autor='fusión local (no redistribuir)', enlace='-',
                  categoria='Jugabilidad', plataforma='Switch 1.2.0', requiere='B04 Buffed Weapons, ' + other,
                  combina_con='B04 Buffed Weapons, ' + other, aplicar='reiniciar el juego',
                  notas='%s: combos de %s con el daño de Buffed Weapons en %d líneas que %s no cambió.' % (f, other[4:], hits, other[4:]))
        print('fusión %s: %d líneas con el daño de Buffed Weapons' % (name, hits))
    # perfiles: todos los mods de la biblioteca listados (on / off), así el perfil decide del todo
    pd = os.path.join(ROOT, 'config', 'perfiles')
    os.makedirs(pd, exist_ok=True)
    allmods = [l[0] for l in LIB] + [z[0] for z in FUSIONS]
    for pname, on in PROFILES.items():
        assert set(on) <= set(allmods), set(on) - set(allmods)
        with open(os.path.join(pd, pname + '.ini'), 'w', encoding='utf-8', newline='\r\n') as f:
            f.write('; Perfil de mods «%s» (tools\\mods\\biblioteca.py). La ventana Mods... lo copia a config\\mods.ini.\n[mods]\n' % pname)
            for m in allmods:
                f.write('%s = %s\n' % (m, 'on' if m in on else 'off'))
    # inventario
    rows, owners = [], {}
    for m in allmods:
        d = os.path.join(MODS, m)
        for rel, p in sorted(romfs_files(d).items()):
            owners.setdefault(rel, []).append(m)
        for a, _, fs in os.walk(os.path.join(d, 'exefs')):
            for f in fs:
                owners.setdefault('exefs/' + f.lower(), []).append(m)
    for m in allmods:
        d = os.path.join(MODS, m)
        files = dict(romfs_files(d))
        for a, _, fs in os.walk(os.path.join(d, 'exefs')):
            files.update({'exefs/' + f.lower(): os.path.join(a, f) for f in fs})
        for rel, p in sorted(files.items()):
            orig = idx.get(rel)
            rows.append([m, rel, os.path.getsize(p), sha(p), orig[0] if orig else ('código (main)' if rel.startswith('exefs') else 'NUEVO'),
                         '; '.join(o for o in owners[rel] if o != m)])
    out = os.path.join(ROOT, 'docs', 'edicion-mejorada')
    os.makedirs(out, exist_ok=True)
    with open(os.path.join(out, 'inventario_mods.csv'), 'w', encoding='utf-8-sig', newline='') as f:
        w = csv.writer(f, delimiter=';')
        w.writerow(['mod', 'ruta', 'bytes', 'sha256', 'sustituye (paquete del juego)', 'misma ruta en'])
        w.writerows(rows)
    print('%d mods, %d archivos, %d rutas compartidas; perfiles: %s' % (len(allmods), len(rows),
          sum(1 for v in owners.values() if len(v) > 1), ', '.join(PROFILES)))


if __name__ == '__main__':
    main()
