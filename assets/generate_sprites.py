#!/usr/bin/env python3
"""
generate_sprites.py
Generates crisp, antialiased top-down 2D character sprite sheets and visual assets
for Shadow Assassin using pure Python (zlib, struct).
Zero external dependencies required.
"""

import math
import os
import struct
import zlib

def write_png(filename, width, height, rgba_data):
    """Write an uncompressed/deflated RGBA PNG file."""
    def chunk(tag, data):
        return struct.pack('>I', len(data)) + tag + data + struct.pack('>I', zlib.crc32(tag + data) & 0xFFFFFFFF)

    raw = bytearray()
    row_bytes = width * 4
    for y in range(height):
        raw.append(0)  # filter type 0: None
        raw.extend(rgba_data[y * row_bytes : (y + 1) * row_bytes])

    ihdr = struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0)
    idat = zlib.compress(bytes(raw), 9)
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', ihdr) + chunk(b'IDAT', idat) + chunk(b'IEND', b'')

    os.makedirs(os.path.dirname(os.path.abspath(filename)), exist_ok=True)
    with open(filename, 'wb') as f:
        f.write(png)
    print(f"Generated {filename} ({width}x{height})")


class Canvas:
    """Super-sampled RGBA canvas with antialiased drawing primitives."""
    def __init__(self, width, height, scale=4):
        self.width = width
        self.height = height
        self.scale = scale
        self.sw = width * scale
        self.sh = height * scale
        # Flat buffer: R, G, B, A in [0..255]
        self.pixels = bytearray(self.sw * self.sh * 4)

    def set_pixel(self, x, y, r, g, b, a):
        if 0 <= x < self.sw and 0 <= y < self.sh:
            idx = (y * self.sw + x) * 4
            src_a = a / 255.0
            dst_a = self.pixels[idx + 3] / 255.0
            out_a = src_a + dst_a * (1.0 - src_a)
            if out_a > 0.001:
                out_r = (r * src_a + self.pixels[idx] * dst_a * (1.0 - src_a)) / out_a
                out_g = (g * src_a + self.pixels[idx + 1] * dst_a * (1.0 - src_a)) / out_a
                out_b = (b * src_a + self.pixels[idx + 2] * dst_a * (1.0 - src_a)) / out_a
                self.pixels[idx] = int(min(255, max(0, out_r)))
                self.pixels[idx + 1] = int(min(255, max(0, out_g)))
                self.pixels[idx + 2] = int(min(255, max(0, out_b)))
                self.pixels[idx + 3] = int(min(255, max(0, out_a * 255)))

    def draw_ellipse(self, cx, cy, rx, ry, color, fill=True, border_color=None, border_w=1):
        """Draw an ellipse in logical coordinates."""
        scx, scy = cx * self.scale, cy * self.scale
        srx, sry = rx * self.scale, ry * self.scale
        sbw = border_w * self.scale
        min_x = max(0, int(scx - srx - sbw - 1))
        max_x = min(self.sw - 1, int(scx + srx + sbw + 1))
        min_y = max(0, int(scy - sry - sbw - 1))
        max_y = min(self.sh - 1, int(scy + sry + sbw + 1))

        r, g, b, a = color
        for py in range(min_y, max_y + 1):
            dy = (py - scy) / max(0.1, sry)
            for px in range(min_x, max_x + 1):
                dx = (px - scx) / max(0.1, srx)
                d2 = dx * dx + dy * dy
                if d2 <= 1.0 and fill:
                    self.set_pixel(px, py, r, g, b, a)

        if border_color and border_w > 0:
            br, bg, bb, ba = border_color
            for py in range(min_y, max_y + 1):
                dy = (py - scy) / max(0.1, sry)
                for px in range(min_x, max_x + 1):
                    dx = (px - scx) / max(0.1, srx)
                    d2 = dx * dx + dy * dy
                    if 0.70 <= d2 <= 1.05:
                        self.set_pixel(px, py, br, bg, bb, ba)

    def draw_rotated_ellipse(self, cx, cy, rx, ry, angle_deg, color, fill=True, border_color=None):
        scx, scy = cx * self.scale, cy * self.scale
        srx, sry = rx * self.scale, ry * self.scale
        rad = math.radians(angle_deg)
        cos_a = math.cos(-rad)
        sin_a = math.sin(-rad)
        max_r = max(srx, sry) * 1.2
        min_x = max(0, int(scx - max_r))
        max_x = min(self.sw - 1, int(scx + max_r))
        min_y = max(0, int(scy - max_r))
        max_y = min(self.sh - 1, int(scy + max_r))

        r, g, b, a = color
        for py in range(min_y, max_y + 1):
            for px in range(min_x, max_x + 1):
                tx = px - scx
                ty = py - scy
                rx_p = (tx * cos_a - ty * sin_a) / max(0.1, srx)
                ry_p = (tx * sin_a + ty * cos_a) / max(0.1, sry)
                d2 = rx_p * rx_p + ry_p * ry_p
                if d2 <= 1.0 and fill:
                    self.set_pixel(px, py, r, g, b, a)
                if border_color and 0.75 <= d2 <= 1.05:
                    self.set_pixel(px, py, border_color[0], border_color[1], border_color[2], border_color[3])

    def draw_polygon(self, points, color, border_color=None):
        """Draw filled convex/simple polygon with points in logical coordinates."""
        spoints = [(p[0] * self.scale, p[1] * self.scale) for p in points]
        min_x = max(0, int(min(p[0] for p in spoints)))
        max_x = min(self.sw - 1, int(max(p[0] for p in spoints)))
        min_y = max(0, int(min(p[1] for p in spoints)))
        max_y = min(self.sh - 1, int(max(p[1] for p in spoints)))

        r, g, b, a = color
        n = len(spoints)
        for py in range(min_y, max_y + 1):
            y_val = py + 0.5
            intersections = []
            for i in range(n):
                p1 = spoints[i]
                p2 = spoints[(i + 1) % n]
                if (p1[1] <= y_val < p2[1]) or (p2[1] <= y_val < p1[1]):
                    x_int = p1[0] + (y_val - p1[1]) * (p2[0] - p1[0]) / (p2[1] - p1[1])
                    intersections.append(x_int)
            intersections.sort()
            for k in range(0, len(intersections) - 1, 2):
                x_start = max(min_x, int(intersections[k]))
                x_end = min(max_x, int(intersections[k + 1]))
                for px in range(x_start, x_end + 1):
                    self.set_pixel(px, py, r, g, b, a)

        if border_color:
            for i in range(n):
                p1 = points[i]
                p2 = points[(i + 1) % n]
                self.draw_line(p1[0], p1[1], p2[0], p2[1], border_color, width=1.0)

    def draw_line(self, x1, y1, x2, y2, color, width=1.0):
        sx1, sy1 = x1 * self.scale, y1 * self.scale
        sx2, sy2 = x2 * self.scale, y2 * self.scale
        sw = max(1.0, width * self.scale)
        dx = sx2 - sx1
        dy = sy2 - sy1
        length = math.sqrt(dx * dx + dy * dy)
        if length < 0.001:
            return
        ux = dx / length
        uy = dy / length
        # Normal
        nx = -uy * (sw * 0.5)
        ny = ux * (sw * 0.5)
        poly = [
            ((sx1 - nx) / self.scale, (sy1 - ny) / self.scale),
            ((sx2 - nx) / self.scale, (sy2 - ny) / self.scale),
            ((sx2 + nx) / self.scale, (sy2 + ny) / self.scale),
            ((sx1 + nx) / self.scale, (sy1 + ny) / self.scale)
        ]
        self.draw_polygon(poly, color)

    def downsample_to_bytes(self):
        """Downsample supersampled canvas to target resolution using box filter."""
        out = bytearray(self.width * self.height * 4)
        s = self.scale
        s2 = s * s
        for y in range(self.height):
            for x in range(self.width):
                acc_r = acc_g = acc_b = acc_a = 0
                for sy in range(s):
                    row_offset = ((y * s + sy) * self.sw + (x * s)) * 4
                    for sx in range(s):
                        idx = row_offset + (sx * 4)
                        acc_r += self.pixels[idx]
                        acc_g += self.pixels[idx + 1]
                        acc_b += self.pixels[idx + 2]
                        acc_a += self.pixels[idx + 3]
                out_idx = (y * self.width + x) * 4
                out[out_idx] = int(acc_r / s2)
                out[out_idx + 1] = int(acc_g / s2)
                out[out_idx + 2] = int(acc_b / s2)
                out[out_idx + 3] = int(acc_a / s2)
        return out


# ==============================================================================
# PLAYER ASSASSIN SPRITE DRAWING
# ==============================================================================

def draw_player_frame(canvas, ox, oy, anim_type, step):
    """
    Draw a 64x64 top-down player assassin frame centered at (ox + 32, oy + 32).
    Facing direction is RIGHT (+X).
    Features a rich blue tactical vest, bright blue highlights, contrasting outline,
    dark hood/boots, and glowing cyan stealth optics.
    """
    cx, cy = ox + 32, oy + 32

    # Palettes
    C_SHADOW      = (0, 0, 0, 115)       # Enhanced ground shadow for crisp floor separation
    C_OUTLINE     = (8, 12, 20, 255)      # Subtle dark contrasting outline around character
    C_BOOT        = (18, 22, 28, 255)
    C_BOOT_TRIM   = (8, 10, 14, 255)
    C_PANTS       = (24, 28, 38, 255)
    C_UNDERGARMENT= (16, 20, 28, 255)

    # Rich Blue Tactical Vest Palette
    C_VEST_DARK   = (18, 55, 155, 255)   # Rich deep royal blue (#12379B)
    C_VEST_MID    = (37, 99, 235, 255)   # Cobalt blue (#2563EB)
    C_VEST_LIGHT  = (96, 165, 250, 255)  # Brighter blue highlight ridges (#60A5FA)
    C_VEST_SPEC   = (191, 219, 254, 255) # Specular gleam highlights (#BFDBFE)

    # Dark Tactical Harness & Accents
    C_BELT        = (10, 14, 20, 255)
    C_BUCKLE      = (200, 215, 235, 255)

    # Assassin Hood & Visor
    C_HOOD_OUTER  = (22, 26, 36, 255)
    C_HOOD_SHADOW = (10, 12, 18, 255)
    C_HOOD_RIM    = (48, 58, 78, 255)
    C_VISOR_CYAN  = (0, 240, 255, 255)
    C_VISOR_GLOW  = (0, 180, 220, 140)

    # Dagger & Combat
    C_DAGGER_BLD  = (240, 245, 250, 255)
    C_DAGGER_HLT  = (70, 80, 95, 255)
    C_DAGGER_SHD  = (140, 155, 175, 255)
    C_BLOOD       = (180, 25, 25, 240)

    # 1. Soft Ground Shadow
    if anim_type != 'dead':
        canvas.draw_ellipse(cx - 1, cy + 2, 17, 13, C_SHADOW)
        canvas.draw_ellipse(cx - 1, cy + 2, 12, 9, (0, 0, 0, 70))
    else:
        # Blood pool on death
        canvas.draw_ellipse(cx + 2, cy + 1, 20, 14, (140, 15, 15, 220))
        canvas.draw_ellipse(cx + 6, cy - 3, 12, 8, (120, 10, 10, 200))
        canvas.draw_ellipse(cx - 5, cy + 4, 10, 7, (110, 8, 8, 180))

    if anim_type == 'dead':
        # Fallen operative corpse with rich blue vest and dark cowl
        canvas.draw_rotated_ellipse(cx - 2, cy - 2, 14, 8, 25, C_VEST_MID, border_color=C_OUTLINE)
        canvas.draw_rotated_ellipse(cx - 4, cy - 3, 7, 5, 25, C_VEST_LIGHT, border_color=C_VEST_DARK)
        canvas.draw_rotated_ellipse(cx + 6, cy + 2, 9, 7, -15, C_HOOD_OUTER, border_color=C_HOOD_SHADOW)
        canvas.draw_ellipse(cx + 8, cy + 3, 5, 4, C_HOOD_SHADOW)
        # Dropped dagger
        canvas.draw_line(cx + 12, cy - 8, cx + 22, cy - 14, C_DAGGER_BLD, width=2.0)
        canvas.draw_line(cx + 10, cy - 6, cx + 13, cy - 8, C_DAGGER_HLT, width=2.5)
        return

    # Feet offset based on animation walk cycle
    left_x = cx - 2
    left_y = cy - 11
    right_x = cx - 2
    right_y = cy + 11

    if anim_type == 'walk':
        # Step 0: Neutral, 1: Left fwd, 2: Passing, 3: Right fwd
        if step == 1:
            left_x += 7
            right_x -= 7
        elif step == 3:
            left_x -= 7
            right_x += 7
    elif anim_type == 'attack':
        if step == 1:  # Lunge
            left_x += 4
            right_x -= 5
        elif step == 2:  # Slash
            left_x += 7
            right_x -= 3

    # 2. Boots & Legs
    canvas.draw_rotated_ellipse(left_x, left_y, 7, 3.5, 0, C_BOOT, border_color=C_OUTLINE)
    canvas.draw_rotated_ellipse(right_x, right_y, 7, 3.5, 0, C_BOOT, border_color=C_OUTLINE)

    # 3. Torso / Cloaked Undergarment & Rich Blue Tactical Vest
    shoulder_fwd = cx + 5
    shoulder_back = cx - 11
    shoulder_top = cy - 13
    shoulder_bot = cy + 13

    if anim_type == 'walk':
        if step == 1:
            shoulder_top += 2
            shoulder_bot += 1
        elif step == 3:
            shoulder_top -= 1
            shoulder_bot -= 2

    # Under-cloak layer (dark)
    cloak_poly = [
        (shoulder_fwd, shoulder_top + 3),
        (shoulder_fwd + 3, cy),
        (shoulder_fwd, shoulder_bot - 3),
        (shoulder_back, shoulder_bot + 1),
        (shoulder_back - 4, cy),
        (shoulder_back, shoulder_top - 1)
    ]
    canvas.draw_polygon(cloak_poly, C_UNDERGARMENT, border_color=C_OUTLINE)

    # Rich Blue Tactical Vest Armor Body
    vest_fwd = cx + 4
    vest_back = cx - 9
    vest_top = cy - 11
    vest_bot = cy + 11
    if anim_type == 'walk':
        if step == 1:
            vest_top += 2
            vest_bot += 1
        elif step == 3:
            vest_top -= 1
            vest_bot -= 2

    vest_poly = [
        (vest_fwd, vest_top + 2),
        (vest_fwd + 2, cy),
        (vest_fwd, vest_bot - 2),
        (vest_back, vest_bot),
        (vest_back - 2, cy),
        (vest_back, vest_top)
    ]
    canvas.draw_polygon(vest_poly, C_VEST_MID, border_color=C_OUTLINE)

    # Tactical Vest Ballistic Chest Plate & Shoulder Pauldrons (Rich Blue with Highlights)
    canvas.draw_rotated_ellipse(cx - 2, cy, 7.5, 6.2, 0, C_VEST_MID, border_color=C_VEST_DARK)
    # Bright Blue Armor Ridge Highlights on Shoulders
    canvas.draw_rotated_ellipse(cx - 2, cy - 8, 4.0, 3.0, -10, C_VEST_LIGHT, border_color=C_VEST_DARK)
    canvas.draw_rotated_ellipse(cx - 2, cy + 8, 4.0, 3.0, 10, C_VEST_LIGHT, border_color=C_VEST_DARK)
    # Central Armor Plate Specular Ridge
    canvas.draw_line(cx - 6, cy, cx + 2, cy, C_VEST_SPEC, width=1.5)
    canvas.draw_line(cx - 4, cy - 4, cx + 1, cy - 4, C_VEST_LIGHT, width=1.2)
    canvas.draw_line(cx - 4, cy + 4, cx + 1, cy + 4, C_VEST_LIGHT, width=1.2)

    # Dark Tactical Harness Straps & Buckle
    canvas.draw_line(cx - 7, cy - 7, cx + 1, cy + 5, C_BELT, width=1.6)
    canvas.draw_line(cx - 7, cy + 7, cx + 1, cy - 5, C_BELT, width=1.6)
    canvas.draw_ellipse(cx - 2, cy, 2.2, 2.2, C_BUCKLE)

    # 4. Arms & Hands holding Dagger
    hand_l_x = cx + 9
    hand_l_y = cy - 7
    hand_r_x = cx + 11
    hand_r_y = cy + 6

    dagger_start_x = hand_r_x
    dagger_start_y = hand_r_y - 2
    dagger_tip_x = cx + 24
    dagger_tip_y = cy + 3

    if anim_type == 'attack':
        if step == 0:  # Pull back
            hand_r_x -= 4
            dagger_tip_x -= 3
        elif step == 1:  # Thrust forward
            hand_r_x += 6
            dagger_tip_x += 10
            dagger_tip_y -= 2
        elif step == 2:  # Slash arc
            hand_r_x += 5
            dagger_tip_x += 9
            dagger_tip_y -= 8
            # Slash trail
            canvas.draw_line(cx + 18, cy + 10, cx + 28, cy - 8, (255, 255, 255, 180), width=2.5)
            canvas.draw_line(cx + 19, cy + 12, cx + 31, cy - 6, (0, 220, 255, 140), width=1.5)

    # Arms (Vest sleeve armor in C_VEST_MID with highlights)
    canvas.draw_line(cx - 1, cy - 10, hand_l_x, hand_l_y, C_VEST_MID, width=4.0)
    canvas.draw_line(cx - 1, cy + 10, hand_r_x, hand_r_y, C_VEST_MID, width=4.0)
    canvas.draw_line(cx - 1, cy - 10, hand_l_x, hand_l_y, C_VEST_LIGHT, width=1.5)
    canvas.draw_line(cx - 1, cy + 10, hand_r_x, hand_r_y, C_VEST_LIGHT, width=1.5)

    # Gloved hands
    canvas.draw_ellipse(hand_l_x, hand_l_y, 2.5, 2.5, C_BOOT, border_color=C_OUTLINE)
    canvas.draw_ellipse(hand_r_x, hand_r_y, 2.8, 2.8, C_BOOT, border_color=C_OUTLINE)

    # Combat Dagger
    # Hilt & Crossguard
    canvas.draw_line(dagger_start_x - 1, dagger_start_y - 2, dagger_start_x - 1, dagger_start_y + 4, C_DAGGER_HLT, width=2.0)
    # Double-edged blade
    canvas.draw_line(dagger_start_x, dagger_start_y, dagger_tip_x, dagger_tip_y, C_DAGGER_BLD, width=2.2)
    canvas.draw_line(dagger_start_x, dagger_start_y + 0.6, dagger_tip_x - 2, dagger_tip_y + 0.6, C_DAGGER_SHD, width=1.2)
    # Specular blade edge gleam
    canvas.draw_line(dagger_start_x + 2, dagger_start_y - 0.4, dagger_tip_x - 3, dagger_tip_y - 0.4, (255, 255, 255, 255), width=0.8)

    # 5. Assassin Head & Distinctive Cowl / Hood
    hood_cx = cx + 1
    hood_cy = cy

    # Hood base with contrasting dark outline and rim
    canvas.draw_ellipse(hood_cx, hood_cy, 8.5, 7.5, C_HOOD_OUTER, border_color=C_OUTLINE)
    canvas.draw_ellipse(hood_cx, hood_cy, 7.5, 6.5, C_HOOD_OUTER, border_color=C_HOOD_RIM)

    # Pointed beak / crest at the front of hood (+X)
    beak_poly = [
        (hood_cx + 5, hood_cy - 4),
        (hood_cx + 10, hood_cy),
        (hood_cx + 5, hood_cy + 4),
    ]
    canvas.draw_polygon(beak_poly, C_HOOD_OUTER, border_color=C_HOOD_RIM)

    # Deep hood shadow void
    shadow_poly = [
        (hood_cx + 4, hood_cy - 3),
        (hood_cx + 8, hood_cy),
        (hood_cx + 4, hood_cy + 3),
        (hood_cx + 2, hood_cy)
    ]
    canvas.draw_polygon(shadow_poly, C_HOOD_SHADOW)

    # Glowing Cyan Visor / Stealth Optics
    canvas.draw_ellipse(hood_cx + 5, hood_cy - 1.8, 1.8, 0.9, C_VISOR_CYAN)
    canvas.draw_ellipse(hood_cx + 5, hood_cy + 1.8, 1.8, 0.9, C_VISOR_CYAN)
    # Visor glow halo
    canvas.draw_ellipse(hood_cx + 5.5, hood_cy - 1.8, 3.0, 1.8, C_VISOR_GLOW)
    canvas.draw_ellipse(hood_cx + 5.5, hood_cy + 1.8, 3.0, 1.8, C_VISOR_GLOW)


# ==============================================================================
# ENEMY GUARD SPRITE DRAWING
# ==============================================================================

def draw_guard_frame(canvas, ox, oy, anim_type, step):
    """
    Draw a 64x64 top-down guard frame centered at (ox + 32, oy + 32).
    Facing direction is RIGHT (+X).
    """
    cx, cy = ox + 32, oy + 32

    # Palettes
    C_SHADOW      = (0, 0, 0, 85)
    C_BOOT        = (15, 18, 24, 255)
    C_BOOT_TRIM   = (30, 36, 48, 255)
    C_PANTS       = (35, 42, 54, 255)
    C_VEST_RED    = (190, 30, 30, 255)
    C_VEST_DARK   = (130, 18, 18, 255)
    C_VEST_LIGHT  = (225, 45, 45, 255)
    C_ARMOR_PLATE = (32, 38, 48, 255)
    C_HELMET_BASE = (45, 52, 65, 255)
    C_HELMET_DARK = (28, 34, 44, 255)
    C_HELMET_RIM  = (65, 75, 95, 255)
    C_VISOR_DARK  = (15, 18, 22, 255)
    C_HEADSET     = (20, 20, 24, 255)
    C_GUN_STEEL   = (55, 62, 75, 255)
    C_GUN_DARK    = (28, 32, 40, 255)
    C_GUN_BARREL  = (85, 95, 115, 255)
    C_FLASHLIGHT  = (240, 245, 180, 255)
    C_MUZZLE      = (255, 210, 50, 255)
    C_BADGE_GOLD  = (240, 180, 30, 255)

    # 1. Ground Shadow
    if anim_type != 'dead':
        canvas.draw_ellipse(cx - 1, cy + 2, 17, 13, C_SHADOW)
    else:
        # Fallen guard blood pool
        canvas.draw_ellipse(cx + 3, cy, 21, 15, (150, 20, 20, 220))
        canvas.draw_ellipse(cx + 8, cy - 4, 13, 9, (120, 12, 12, 200))
        # Fallen guard body
        canvas.draw_rotated_ellipse(cx - 3, cy - 2, 15, 9, 30, C_VEST_DARK, border_color=C_ARMOR_PLATE)
        canvas.draw_rotated_ellipse(cx + 7, cy + 3, 9, 8, -20, C_HELMET_BASE, border_color=C_HELMET_DARK)
        # Cracked visor line
        canvas.draw_line(cx + 5, cy + 1, cx + 11, cy + 5, (255, 50, 50, 255), width=1.5)
        # Dropped rifle
        canvas.draw_line(cx + 10, cy - 10, cx + 24, cy - 16, C_GUN_STEEL, width=3.0)
        return

    # Feet stride offset
    left_x = cx - 2
    left_y = cy - 12
    right_x = cx - 2
    right_y = cy + 12

    if anim_type == 'patrol':
        if step == 1:
            left_x += 6
            right_x -= 6
        elif step == 3:
            left_x -= 6
            right_x += 6
    elif anim_type == 'chase':
        if step == 1:
            left_x += 9
            right_x -= 9
        elif step == 3:
            left_x -= 9
            right_x += 9

    # 2. Combat Boots
    canvas.draw_rotated_ellipse(left_x, left_y, 7.5, 4.0, 0, C_BOOT, border_color=C_BOOT_TRIM)
    canvas.draw_rotated_ellipse(right_x, right_y, 7.5, 4.0, 0, C_BOOT, border_color=C_BOOT_TRIM)

    # 3. Torso / Armored Security Vest (Crimson Red with Ballistic Plates)
    vest_fwd = cx + 6
    vest_back = cx - 11
    vest_top = cy - 14
    vest_bot = cy + 14

    vest_poly = [
        (vest_fwd, vest_top + 3),
        (vest_fwd + 2, cy),
        (vest_fwd, vest_bot - 3),
        (vest_back, vest_bot + 1),
        (vest_back - 3, cy),
        (vest_back, vest_top - 1)
    ]
    canvas.draw_polygon(vest_poly, C_VEST_RED, border_color=C_VEST_DARK)

    # Black ballistic plate inserts on chest & shoulders
    canvas.draw_rotated_ellipse(cx - 3, cy, 7, 6, 0, C_ARMOR_PLATE, border_color=C_VEST_DARK)
    canvas.draw_ellipse(cx - 2, cy - 8, 3.5, 2.5, C_ARMOR_PLATE)
    canvas.draw_ellipse(cx - 2, cy + 8, 3.5, 2.5, C_ARMOR_PLATE)

    # Gold Security Badge
    canvas.draw_ellipse(cx + 1, cy - 4, 1.8, 1.8, C_BADGE_GOLD)

    # 4. Arms & Tactical Carbine Rifle
    hand_l_x = cx + 13
    hand_l_y = cy - 4
    hand_r_x = cx + 8
    hand_r_y = cy + 6

    gun_barrel_end_x = cx + 26
    gun_barrel_end_y = cy + 1

    if anim_type == 'alert':
        # Weapon raised high alert
        gun_barrel_end_x = cx + 24
        gun_barrel_end_y = cy - 2
    elif anim_type == 'attack':
        # Firing forward with muzzle flash
        gun_barrel_end_x = cx + 28
        gun_barrel_end_y = cy + 1

    # Arm sleeves (red uniform fabric)
    canvas.draw_line(cx - 1, cy - 11, hand_l_x - 3, hand_l_y, C_VEST_RED, width=4.0)
    canvas.draw_line(cx - 1, cy + 11, hand_r_x - 2, hand_r_y, C_VEST_RED, width=4.0)

    # Gloved hands gripping rifle
    canvas.draw_ellipse(hand_l_x, hand_l_y, 2.6, 2.6, C_BOOT)
    canvas.draw_ellipse(hand_r_x, hand_r_y, 2.8, 2.8, C_BOOT)

    # Rifle Receiver & Stock
    canvas.draw_line(cx + 4, cy + 5, cx + 16, cy + 2, C_GUN_STEEL, width=4.5)
    # Gun Barrel extending forward
    canvas.draw_line(cx + 14, cy + 1.5, gun_barrel_end_x, gun_barrel_end_y, C_GUN_BARREL, width=2.5)
    # Tactical Flashlight mounted under barrel
    canvas.draw_line(cx + 17, cy - 0.5, gun_barrel_end_x - 2, gun_barrel_end_y - 1.5, C_GUN_DARK, width=1.8)
    canvas.draw_ellipse(gun_barrel_end_x - 2, gun_barrel_end_y - 1.5, 1.5, 1.2, C_FLASHLIGHT)

    # Muzzle flash if attacking
    if anim_type == 'attack' and step == 0:
        canvas.draw_ellipse(gun_barrel_end_x + 5, gun_barrel_end_y, 5.0, 3.5, C_MUZZLE)
        canvas.draw_ellipse(gun_barrel_end_x + 7, gun_barrel_end_y, 2.5, 1.5, (255, 255, 255, 255))

    # 5. Guard Tactical Riot Helmet
    helmet_cx = cx + 1
    helmet_cy = cy

    # Helmet dome
    canvas.draw_ellipse(helmet_cx, helmet_cy, 9.0, 8.0, C_HELMET_BASE, border_color=C_HELMET_RIM)
    # Helmet ridge / visor brim extending forward (+X)
    canvas.draw_ellipse(helmet_cx + 5, helmet_cy, 3.5, 6.0, C_HELMET_DARK, border_color=C_HELMET_RIM)
    # Dark Visor / eye slot
    canvas.draw_ellipse(helmet_cx + 6, helmet_cy, 2.2, 4.5, C_VISOR_DARK)

    # Headset / communication earcups on top & bottom
    canvas.draw_ellipse(helmet_cx - 1, helmet_cy - 8.5, 2.5, 2.0, C_HEADSET)
    canvas.draw_ellipse(helmet_cx - 1, helmet_cy + 8.5, 2.5, 2.0, C_HEADSET)
    canvas.draw_line(helmet_cx - 1, helmet_cy - 8.5, helmet_cx - 1, helmet_cy + 8.5, C_HEADSET, width=1.0)


# ==============================================================================
# MAIN ASSET GENERATOR
# ==============================================================================

def generate_all_assets():
    target_dir = os.path.join(os.path.dirname(__file__), 'images')
    os.makedirs(target_dir, exist_ok=True)

    # 1. Player Sprite Sheet (4 cols x 3 rows = 256 x 192)
    # Row 0: Walk Cycle (0: idle, 1: walk1, 2: walk2, 3: walk3)
    # Row 1: Attack (0: prep, 1: thrust, 2: slash, 3: recover)
    # Row 2: Dead (0: slump, 1: fallen corpse, 2..3: empty)
    pw, ph = 256, 192
    p_canvas = Canvas(pw, ph, scale=4)
    print("Rendering Player Sprite Sheet...")

    for col in range(4):
        # Row 0: Walk
        draw_player_frame(p_canvas, col * 64, 0, 'walk', col)
        # Row 1: Attack
        draw_player_frame(p_canvas, col * 64, 64, 'attack', col)
        # Row 2: Dead
        if col < 2:
            draw_player_frame(p_canvas, col * 64, 128, 'dead', col)

    write_png(os.path.join(target_dir, 'player_spritesheet.png'), pw, ph, p_canvas.downsample_to_bytes())

    # 2. Guard Sprite Sheet (4 cols x 3 rows = 256 x 192)
    # Row 0: Patrol (0: idle, 1: walk1, 2: walk2, 3: walk3)
    # Row 1: Chase & Alert (0: alert, 1: sprint1, 2: sprint2, 3: sprint3)
    # Row 2: Attack & Dead (0: attack, 1: recoil, 2: fall, 3: dead corpse)
    gw, gh = 256, 192
    g_canvas = Canvas(gw, gh, scale=4)
    print("Rendering Guard Sprite Sheet...")

    for col in range(4):
        # Row 0: Patrol
        draw_guard_frame(g_canvas, col * 64, 0, 'patrol', col)
        # Row 1: Chase
        if col == 0:
            draw_guard_frame(g_canvas, 0, 64, 'alert', 0)
        else:
            draw_guard_frame(g_canvas, col * 64, 64, 'chase', col)
        # Row 2: Combat & Death
        if col == 0:
            draw_guard_frame(g_canvas, 0, 128, 'attack', 0)
        elif col == 1:
            draw_guard_frame(g_canvas, 64, 128, 'attack', 1)
        elif col == 2:
            draw_guard_frame(g_canvas, 128, 128, 'dead', 0)
        elif col == 3:
            draw_guard_frame(g_canvas, 192, 128, 'dead', 1)

    write_png(os.path.join(target_dir, 'guard_spritesheet.png'), gw, gh, g_canvas.downsample_to_bytes())

    # 3. Soft Shadow Decal (64x64)
    sw, sh = 64, 64
    s_canvas = Canvas(sw, sh, scale=4)
    for r in range(22, 0, -1):
        alpha = int((1.0 - (r / 22.0)) * 110)
        s_canvas.draw_ellipse(32, 32, r, int(r * 0.75), (0, 0, 0, alpha))
    write_png(os.path.join(target_dir, 'shadow.png'), sw, sh, s_canvas.downsample_to_bytes())

    # 4. Blood Splatter Decal (64x64)
    bw, bh = 64, 64
    b_canvas = Canvas(bw, bh, scale=4)
    b_canvas.draw_ellipse(32, 32, 16, 11, (150, 15, 15, 230))
    b_canvas.draw_ellipse(38, 28, 10, 7, (130, 10, 10, 210))
    b_canvas.draw_ellipse(24, 35, 9, 6, (120, 8, 8, 190))
    b_canvas.draw_ellipse(44, 38, 6, 4, (140, 12, 12, 180))
    b_canvas.draw_ellipse(20, 24, 5, 4, (135, 10, 10, 170))
    write_png(os.path.join(target_dir, 'blood_splatter.png'), bw, bh, b_canvas.downsample_to_bytes())

    print("All character visual assets successfully generated!")

if __name__ == '__main__':
    generate_all_assets()
