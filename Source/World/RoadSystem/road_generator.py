"""Procedural road generation using constrained spline-like sampled paths."""
from __future__ import annotations

from dataclasses import dataclass
from enum import Enum
import math

from Source.Core.Math.vector import Vec2
from Source.Core.Utilities.hash_utils import hash_combine, hash_to_unit_float


class RoadType(str, Enum):
    HIGHWAY = 'highway'
    VILLAGE = 'village'
    DIRT = 'dirt'
    COASTAL = 'coastal'


@dataclass
class RoadSegment:
    road_type: RoadType
    points: list[Vec2]


class RoadGenerator:
    max_slope_degrees = 12.0
    min_curve_radius = 20.0
    segment_length = 40.0

    def __init__(self, seed: int):
        self.seed = seed

    def _pick_road_type(self, chunk_x: int, chunk_y: int) -> RoadType:
        value = hash_to_unit_float(hash_combine(self.seed, chunk_x, chunk_y, 77))
        if value < 0.25:
            return RoadType.HIGHWAY
        if value < 0.5:
            return RoadType.VILLAGE
        if value < 0.75:
            return RoadType.DIRT
        return RoadType.COASTAL

    def generate_chunk_roads(self, chunk_x: int, chunk_y: int, chunk_size: int) -> list[RoadSegment]:
        road_type = self._pick_road_type(chunk_x, chunk_y)
        start = Vec2(chunk_x * chunk_size, chunk_y * chunk_size + chunk_size * 0.5)
        points = [start]

        heading = hash_to_unit_float(hash_combine(self.seed, chunk_x, chunk_y, 12)) * math.tau
        curvature = (hash_to_unit_float(hash_combine(self.seed, chunk_x, chunk_y, 13)) - 0.5) * 0.08

        steps = max(4, int(chunk_size / self.segment_length) + 4)
        for _ in range(steps):
            heading += curvature
            dx = math.cos(heading) * self.segment_length
            dy = math.sin(heading) * self.segment_length
            next_point = Vec2(points[-1].x + dx, points[-1].y + dy)
            points.append(next_point)

        return [RoadSegment(road_type=road_type, points=points)]
