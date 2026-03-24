"""Biome classification from temperature, humidity and elevation maps."""
from __future__ import annotations

from dataclasses import dataclass
from enum import Enum


class BiomeType(str, Enum):
    DESERT = 'desert'
    FOREST = 'forest'
    ROCKY_MOUNTAIN = 'rocky_mountain'
    COASTAL = 'coastal'
    PLAINS = 'plains'
    HILLS = 'hills'


@dataclass(frozen=True)
class BiomeSample:
    biome: BiomeType
    vegetation_density: float
    terrain_color: tuple[int, int, int]


def classify_biome(temperature: float, humidity: float, elevation: float, water_level: float) -> BiomeSample:
    if elevation < water_level + 10:
        return BiomeSample(BiomeType.COASTAL, 0.45, (210, 200, 140))
    if elevation > 420:
        return BiomeSample(BiomeType.ROCKY_MOUNTAIN, 0.15, (120, 120, 125))
    if temperature > 0.55 and humidity < 0.35:
        return BiomeSample(BiomeType.DESERT, 0.05, (200, 170, 90))
    if humidity > 0.6 and 120 < elevation < 300:
        return BiomeSample(BiomeType.FOREST, 0.9, (45, 120, 55))
    if elevation > 260:
        return BiomeSample(BiomeType.HILLS, 0.5, (90, 140, 75))
    return BiomeSample(BiomeType.PLAINS, 0.65, (110, 160, 80))
