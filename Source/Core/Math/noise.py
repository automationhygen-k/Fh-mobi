"""Deterministic 2D Perlin/fractal noise implementation."""
from __future__ import annotations

import math
from dataclasses import dataclass

from Source.Core.Utilities.hash_utils import hash_combine, hash_to_unit_float


_GRADIENTS = (
    (1.0, 0.0), (-1.0, 0.0), (0.0, 1.0), (0.0, -1.0),
    (0.707, 0.707), (-0.707, 0.707), (0.707, -0.707), (-0.707, -0.707),
)


def _fade(t: float) -> float:
    return t * t * t * (t * (t * 6 - 15) + 10)


def _lerp(a: float, b: float, t: float) -> float:
    return a + t * (b - a)


def _gradient(seed: int, ix: int, iy: int) -> tuple[float, float]:
    idx = int(hash_to_unit_float(hash_combine(seed, ix, iy)) * len(_GRADIENTS)) % len(_GRADIENTS)
    return _GRADIENTS[idx]


@dataclass(frozen=True)
class NoiseLayer:
    frequency: float
    amplitude: float
    octaves: int = 1
    lacunarity: float = 2.0
    persistence: float = 0.5


class Noise2D:
    def __init__(self, seed: int):
        self.seed = seed

    def perlin(self, x: float, y: float) -> float:
        x0, y0 = math.floor(x), math.floor(y)
        x1, y1 = x0 + 1, y0 + 1

        sx, sy = x - x0, y - y0

        g00 = _gradient(self.seed, x0, y0)
        g10 = _gradient(self.seed, x1, y0)
        g01 = _gradient(self.seed, x0, y1)
        g11 = _gradient(self.seed, x1, y1)

        n00 = g00[0] * (x - x0) + g00[1] * (y - y0)
        n10 = g10[0] * (x - x1) + g10[1] * (y - y0)
        n01 = g01[0] * (x - x0) + g01[1] * (y - y1)
        n11 = g11[0] * (x - x1) + g11[1] * (y - y1)

        u = _fade(sx)
        v = _fade(sy)

        nx0 = _lerp(n00, n10, u)
        nx1 = _lerp(n01, n11, u)
        return _lerp(nx0, nx1, v)

    def fractal(self, x: float, y: float, layer: NoiseLayer) -> float:
        total = 0.0
        frequency = layer.frequency
        amplitude = layer.amplitude
        max_amp = 0.0

        for _ in range(layer.octaves):
            total += self.perlin(x * frequency, y * frequency) * amplitude
            max_amp += amplitude
            frequency *= layer.lacunarity
            amplitude *= layer.persistence

        if max_amp == 0:
            return 0.0
        return total / max_amp
