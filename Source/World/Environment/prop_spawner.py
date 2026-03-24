"""Biome-aware deterministic environment prop spawning with collision and slope filtering."""
from __future__ import annotations

from dataclasses import dataclass
import math

from Source.Core.Math.vector import Vec2
from Source.Core.Utilities.hash_utils import hash_combine, hash_to_unit_float
from Source.World.Terrain.terrain_generator import TerrainGenerator
from Source.World.RoadSystem.road_generator import RoadSegment


@dataclass
class PropInstance:
    prop_type: str
    position: Vec2
    yaw_degrees: float


class PropSpawner:
    cell_size = 10
    max_trees_per_chunk = 200
    max_props_per_chunk = 150

    def __init__(self, seed: int, terrain: TerrainGenerator):
        self.seed = seed
        self.terrain = terrain

    @staticmethod
    def _distance_point_to_segment(p: Vec2, a: Vec2, b: Vec2) -> float:
        abx, aby = b.x - a.x, b.y - a.y
        apx, apy = p.x - a.x, p.y - a.y
        denom = abx * abx + aby * aby
        if denom == 0:
            return math.hypot(apx, apy)
        t = max(0.0, min(1.0, (apx * abx + apy * aby) / denom))
        cx, cy = a.x + t * abx, a.y + t * aby
        return math.hypot(p.x - cx, p.y - cy)

    def _on_road(self, p: Vec2, roads: list[RoadSegment], width: float = 8.0) -> bool:
        for road in roads:
            for i in range(len(road.points) - 1):
                if self._distance_point_to_segment(p, road.points[i], road.points[i + 1]) < width:
                    return True
        return False

    def generate_chunk_props(self, chunk_x: int, chunk_y: int, chunk_size: int, roads: list[RoadSegment]) -> list[PropInstance]:
        props: list[PropInstance] = []
        occupied: list[Vec2] = []
        tree_count = 0

        base_x = chunk_x * chunk_size
        base_y = chunk_y * chunk_size
        cells = chunk_size // self.cell_size

        for gx in range(0, cells, 2):
            for gy in range(0, cells, 2):
                wx = base_x + gx * self.cell_size + self.cell_size * 0.5
                wy = base_y + gy * self.cell_size + self.cell_size * 0.5
                p = Vec2(wx, wy)

                h = self.terrain.sample_height(wx, wy)
                h_dx = self.terrain.sample_height(wx + 4, wy)
                h_dy = self.terrain.sample_height(wx, wy + 4)
                slope = math.degrees(math.atan2(max(abs(h_dx - h), abs(h_dy - h)), 2.0))

                if slope > 28.0 or self._on_road(p, roads):
                    continue

                biome = self.terrain.sample_biome(wx, wy, h)
                roll = hash_to_unit_float(hash_combine(self.seed, chunk_x, chunk_y, gx, gy))

                if biome.vegetation_density > 0.6 and roll < 0.08 and tree_count < self.max_trees_per_chunk:
                    prop_type = 'tree_cylinder'
                    tree_count += 1
                elif roll < 0.06 and len(props) < self.max_props_per_chunk:
                    prop_type = 'rock' if biome.biome.value != 'desert' else 'sign_board'
                else:
                    continue

                if any((p.x - o.x) ** 2 + (p.y - o.y) ** 2 < 16 for o in occupied):
                    continue

                occupied.append(p)
                props.append(PropInstance(
                    prop_type=prop_type,
                    position=p,
                    yaw_degrees=hash_to_unit_float(hash_combine(self.seed, gx, gy, 55)) * 360,
                ))

        return props
