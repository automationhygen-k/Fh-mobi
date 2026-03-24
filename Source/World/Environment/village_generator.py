"""Village generator aligned to roads with primitive-friendly structure definitions."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.vector import Vec2
from Source.Core.Utilities.hash_utils import hash_combine, hash_to_unit_float
from Source.World.RoadSystem.road_generator import RoadSegment


@dataclass
class VillageBuilding:
    position: Vec2
    width: float
    depth: float
    height: float
    style: str


@dataclass
class VillageData:
    center: Vec2
    central_road: list[Vec2]
    houses: list[VillageBuilding]


class VillageGenerator:
    def __init__(self, seed: int):
        self.seed = seed

    def generate(self, chunk_x: int, chunk_y: int, roads: list[RoadSegment]) -> VillageData | None:
        if not roads:
            return None
        spawn_val = hash_to_unit_float(hash_combine(self.seed, chunk_x, chunk_y, 991))
        if spawn_val < 0.55:
            return None

        road = roads[0]
        center_index = len(road.points) // 2
        center = road.points[center_index]
        styles = ('clay_house', 'cement_home', 'wooden_hut')
        houses: list[VillageBuilding] = []

        for i in range(-6, 7):
            road_idx = max(0, min(len(road.points) - 1, center_index + i))
            p = road.points[road_idx]
            offset = 16.0
            side = -1 if i % 2 == 0 else 1
            style = styles[(road_idx + i) % len(styles)]
            jitter = (hash_to_unit_float(hash_combine(self.seed, chunk_x, chunk_y, road_idx, i)) - 0.5) * 6.0

            houses.append(VillageBuilding(
                position=Vec2(p.x + side * offset, p.y + jitter),
                width=8.0,
                depth=8.0,
                height=4.0 + (i % 3),
                style=style,
            ))

        return VillageData(center=center, central_road=road.points, houses=houses)
