"""Generate Earth / Moon / Mars space backgrounds for PlaneWar."""
from PIL import Image, ImageDraw, ImageFilter
import numpy as np
import math, random, os

W, H = 480, 720
OUT = os.path.join(os.path.dirname(__file__), '..', 'Resources', 'res')
os.makedirs(OUT, exist_ok=True)


# ── Helpers ───────────────────────────────────────────────────────────────────

def new_bg(color):
    return Image.new('RGBA', (W, H), color + (255,))


def scatter_stars(img, seed, count, base_rgb, brightness_range=(80, 255)):
    draw = ImageDraw.Draw(img)
    rng = random.Random(seed)
    for _ in range(count):
        x = rng.randint(0, W - 1)
        y = rng.randint(0, H - 1)
        b = rng.randint(*brightness_range) / 255.0
        c = tuple(int(ch * b) for ch in base_rgb) + (255,)
        sz = rng.choices([0, 1, 2], weights=[6, 3, 1])[0]
        if sz == 0:
            img.putpixel((x, y), c)
        else:
            draw.ellipse([x - sz, y - sz, x + sz, y + sz], fill=c)


def glow_ring(img, cx, cy, r, color_rgb, steps=22, max_alpha=90):
    """Paint atmosphere glow around a sphere."""
    layer = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(layer)
    for i in range(steps, 0, -1):
        alpha = int(max_alpha * (steps - i) / steps)
        off = i * 2
        draw.ellipse([cx - r - off, cy - r - off, cx + r + off, cy + r + off],
                     fill=color_rgb + (alpha,))
    img.alpha_composite(layer)


def sphere_shaded(img, cx, cy, r, base_rgb, dark_rgb, light_rgb):
    """
    Draw a sphere with Lambert shading.
    Light source: upper-left (-0.5, -0.7, 0.5 normalised).
    """
    lx, ly, lz = -0.5, -0.7, 0.5
    llen = math.sqrt(lx*lx + ly*ly + lz*lz)
    lx, ly, lz = lx/llen, ly/llen, lz/llen

    arr = np.array(img, dtype=np.float32)
    Y, X = np.ogrid[:H, :W]
    dx = (X - cx).astype(np.float32)
    dy = (Y - cy).astype(np.float32)
    d2 = dx*dx + dy*dy
    mask = d2 <= float(r * r)

    r_f = float(r)
    dz = np.sqrt(np.maximum(0.0, r_f*r_f - d2))
    nx = dx / r_f
    ny = dy / r_f
    nz = dz / r_f

    # Lambert diffuse + ambient
    dot = -(nx * lx + ny * ly + nz * lz)
    ambient = 0.18
    diffuse = np.clip(dot, 0, 1)
    light = ambient + (1.0 - ambient) * diffuse

    # Blend surface colour: use dark_rgb in shadow, base_rgb in mid, light_rgb at highlight
    t_hi = np.clip((light - 0.5) * 2, 0, 1)          # 0→1 for highlight
    t_sh = np.clip((0.4 - light) * 2.5, 0, 1)         # 0→1 for shadow

    base  = np.array(base_rgb,  dtype=np.float32)
    dark  = np.array(dark_rgb,  dtype=np.float32)
    light_col = np.array(light_rgb, dtype=np.float32)

    surface = base[None, None, :] * (1 - t_hi[..., None] - t_sh[..., None]) + \
              light_col[None, None, :] * t_hi[..., None] + \
              dark[None, None, :] * t_sh[..., None]
    surface = np.clip(surface, 0, 255)

    arr[mask, :3] = surface[mask]
    arr[mask, 3]  = 255
    return Image.fromarray(arr.astype(np.uint8), 'RGBA')


def paste_rgba(base, overlay):
    base.alpha_composite(overlay)


# ── Earth ─────────────────────────────────────────────────────────────────────

def make_earth():
    img = new_bg((3, 6, 22))
    scatter_stars(img, seed=11, count=260, base_rgb=(210, 220, 255))

    cx, cy, r = W // 2, H // 2 - 10, 198

    glow_ring(img, cx, cy, r, (40, 110, 255), steps=24, max_alpha=80)

    # Base sphere (ocean blue)
    img = sphere_shaded(img, cx, cy, r,
                        base_rgb=(24, 98, 210),
                        dark_rgb=(8, 35, 90),
                        light_rgb=(100, 170, 255))

    draw = ImageDraw.Draw(img)

    # ── Continents (simplified outlines) ─────────────────────────────────────
    rng = random.Random(42)

    def continent(pts, fill):
        shifted = [(cx + x, cy + y) for x, y in pts]
        draw.polygon(shifted, fill=fill)

    land = (46, 150, 60, 255)
    land2 = (35, 125, 48, 255)

    # North America
    continent([(-150, -100), (-80, -130), (-30, -90), (-40, -20),
               (-90,  30),  (-150, 10)], land)
    # South America
    continent([(-90, 40), (-50, 30), (-30, 100), (-60, 160), (-100, 120)], land2)
    # Europe + Africa
    continent([(30, -130), (80, -110), (100,  -60), (80,  20),
               (50,  40),  (30,  160), (10,   50),  (10, -90)], land)
    # Asia
    continent([(80, -140), (185, -120), (190, -30), (150, 30),
               (100,  10), (80,  -60)], land)
    # Australia
    continent([(120, 60), (170, 50), (175, 110), (135, 130), (110, 95)], land2)

    # Polar ice caps
    draw.ellipse([cx - 50, cy - r, cx + 50, cy - r + 35],
                  fill=(220, 235, 255, 220))
    draw.ellipse([cx - 35, cy + r - 28, cx + 35, cy + r],
                  fill=(210, 230, 255, 180))

    # Cloud wisps
    rng2 = random.Random(77)
    cloud = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    cd = ImageDraw.Draw(cloud)
    for _ in range(28):
        ox = rng2.randint(-r + 20, r - 20)
        oy = rng2.randint(-r + 20, r - 20)
        if ox * ox + oy * oy > (r - 12) ** 2:
            continue
        cw = rng2.randint(18, 55)
        ch = rng2.randint(4, 12)
        alpha = rng2.randint(40, 90)
        cd.ellipse([cx + ox - cw, cy + oy - ch,
                    cx + ox + cw, cy + oy + ch],
                   fill=(230, 240, 255, alpha))
    img.alpha_composite(cloud.filter(ImageFilter.GaussianBlur(2)))

    return img.convert('RGB')


# ── Moon ──────────────────────────────────────────────────────────────────────

def make_moon():
    img = new_bg((6, 6, 8))
    scatter_stars(img, seed=22, count=220, base_rgb=(240, 240, 245),
                  brightness_range=(120, 255))

    cx, cy, r = W // 2, H // 2 - 10, 198

    glow_ring(img, cx, cy, r, (160, 160, 170), steps=14, max_alpha=40)

    img = sphere_shaded(img, cx, cy, r,
                        base_rgb=(165, 165, 165),
                        dark_rgb=(60,  60,  65),
                        light_rgb=(215, 215, 215))

    draw = ImageDraw.Draw(img)

    # Craters
    rng = random.Random(55)
    crater_layer = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    cd = ImageDraw.Draw(crater_layer)

    for _ in range(38):
        ox = rng.randint(-(r - 15), r - 15)
        oy = rng.randint(-(r - 15), r - 15)
        if ox * ox + oy * oy > (r - 14) ** 2:
            continue
        cr = rng.randint(6, 28)
        # Outer shadow ring
        cd.ellipse([cx+ox-cr-3, cy+oy-cr-3, cx+ox+cr+3, cy+oy+cr+3],
                    fill=(50, 50, 55, 80))
        # Crater bowl
        cd.ellipse([cx+ox-cr, cy+oy-cr, cx+ox+cr, cy+oy+cr],
                    fill=(110, 110, 115, 150))
        # Inner highlight
        hcr = max(2, cr // 3)
        cd.ellipse([cx+ox-hcr, cy+oy-hcr, cx+ox+hcr, cy+oy+hcr],
                    fill=(200, 200, 200, 60))

    img.alpha_composite(crater_layer.filter(ImageFilter.GaussianBlur(1)))

    # Large prominent mare (dark basalt regions)
    mare = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    md = ImageDraw.Draw(mare)
    for (ox, oy, mr) in [(-50, -40, 55), (40, 60, 40), (-20, 70, 30)]:
        if ox*ox + oy*oy < (r - mr - 5)**2:
            md.ellipse([cx+ox-mr, cy+oy-mr, cx+ox+mr, cy+oy+mr],
                        fill=(90, 90, 98, 100))
    img.alpha_composite(mare.filter(ImageFilter.GaussianBlur(3)))

    return img.convert('RGB')


# ── Mars ──────────────────────────────────────────────────────────────────────

def make_mars():
    img = new_bg((18, 5, 2))
    scatter_stars(img, seed=33, count=240, base_rgb=(255, 210, 160),
                  brightness_range=(80, 220))

    cx, cy, r = W // 2, H // 2 - 10, 198

    # Reddish atmospheric glow
    glow_ring(img, cx, cy, r, (200, 60, 10), steps=20, max_alpha=70)

    img = sphere_shaded(img, cx, cy, r,
                        base_rgb=(190, 90,  40),
                        dark_rgb=(80,  25,  8),
                        light_rgb=(240, 160, 90))

    draw = ImageDraw.Draw(img)
    rng = random.Random(66)

    # Surface texture (subtle darker patches - ancient basins)
    basin_layer = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    bd = ImageDraw.Draw(basin_layer)
    for _ in range(12):
        ox = rng.randint(-(r-30), r-30)
        oy = rng.randint(-(r-30), r-30)
        if ox*ox + oy*oy > (r-25)**2:
            continue
        bw = rng.randint(25, 70)
        bh = rng.randint(15, 45)
        alpha = rng.randint(25, 55)
        bd.ellipse([cx+ox-bw, cy+oy-bh, cx+ox+bw, cy+oy+bh],
                    fill=(60, 18, 5, alpha))
    img.alpha_composite(basin_layer.filter(ImageFilter.GaussianBlur(4)))

    # Polar ice cap (north / top of sphere)
    polar = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    pd = ImageDraw.Draw(polar)
    pd.ellipse([cx - 55, cy - r, cx + 55, cy - r + 42],
                fill=(235, 238, 248, 210))
    pd.ellipse([cx - 30, cy - r, cx + 30, cy - r + 20],
                fill=(248, 250, 255, 230))
    img.alpha_composite(polar.filter(ImageFilter.GaussianBlur(2)))

    # Dust storm band (horizontal semi-transparent streak)
    dust = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    dd = ImageDraw.Draw(dust)
    dd.ellipse([cx-r+10, cy-18, cx+r-10, cy+18], fill=(210, 130, 50, 35))
    img.alpha_composite(dust.filter(ImageFilter.GaussianBlur(5)))

    return img.convert('RGB')


# ── Main ──────────────────────────────────────────────────────────────────────

if __name__ == '__main__':
    print('Generating Earth...')
    make_earth().save(os.path.join(OUT, 'bg_earth.jpg'), quality=92)

    print('Generating Moon...')
    make_moon().save(os.path.join(OUT, 'bg_moon.jpg'), quality=92)

    print('Generating Mars...')
    make_mars().save(os.path.join(OUT, 'bg_mars.jpg'), quality=92)

    print('Done. Files saved to', OUT)
