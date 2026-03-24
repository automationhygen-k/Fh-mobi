"""Terrain generation using layered deterministic noise."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.noise import Noise2D, NoiseLayer
from Source.World.Terrain.biome import BiomeSample, classify_biome


@dataclass
class TerrainPoint:
    world_x: float
    world_y: float
    height: float
    biome: BiomeSample


@dataclass
class TerrainChunkData:
    samples: list[TerrainPoint]
    water_level: float


class TerrainGenerator:
    def __init__(self, seed: int):
        self.noise = Noise2D(seed)
        self.temperature_noise = Noise2D(seed + 1_001)
        self.humidity_noise = Noise2D(seed + 2_003)
        self.water_level = 30.0

        self.base_height = NoiseLayer(frequency=0.0018, amplitude=1.0, octaves=4)
        self.mountain = NoiseLayer(frequency=0.0045, amplitude=1.0, octaves=5)
        self.detail = NoiseLayer(frequency=0.015, amplitude=1.0, octaves=3)
        self.erosion = NoiseLayer(frequency=0.007, amplitude=1.0, octaves=4)

    def sample_height(self, x: float, y: float) -> float:
        base = self.noise.fractal(x, y, self.base_height)
        mountains = max(0.0, self.noise.fractal(x + 113.7, y - 88.1, self.mountain))
        detail = self.noise.fractal(x - 74.2, y + 240.3, self.detail)
        erosion = self.noise.fractal(x + 980.3, y - 666.0, self.erosion)

        height = base * 200 + mountains * 400 + detail * 30 - abs(erosion) * 25
        if height < self.water_level + 25:
            coastal_blend = max(0.0, min(1.0, (height - self.water_level) / 25.0))
            height = self.water_level - 2 + (height - (self.water_level - 2)) * (coastal_blend ** 2)
        return height

    def sample_biome(self, x: float, y: float, elevation: float) -> BiomeSample:
        temp = self.temperature_noise.fractal(x, y, NoiseLayer(0.001, 1.0, octaves=3)) * 0.5 + 0.5
        humidity = self.humidity_noise.fractal(x + 400, y - 900, NoiseLayer(0.0012, 1.0, octaves=3)) * 0.5 + 0.5
        return classify_biome(temp, humidity, elevation, self.water_level)

    def generate_chunk(self, chunk_x: int, chunk_y: int, chunk_size: int, step: int = 64) -> TerrainChunkData:
        samples: list[TerrainPoint] = []
        base_x = chunk_x * chunk_size
        base_y = chunk_y * chunk_size

        for lx in range(0, chunk_size + 1, step):
            for ly in range(0, chunk_size + 1, step):
                wx = float(base_x + lx)
                wy = float(base_y + ly)
                h = self.sample_height(wx, wy)
                biome = self.sample_biome(wx, wy, h)
                samples.append(TerrainPoint(wx, wy, h, biome))

        return TerrainChunkData(samples=samples, water_level=self.water_level)
