"""Traffic vehicle model used by AI traffic manager and road navigation."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.vector import Vec3
from Source.World.RoadSystem.road_generator import RoadType


@dataclass
class TrafficVehicle:
    vehicle_id: int
    position: Vec3
    yaw: float
    speed_mps: float
    target_speed_mps: float
    road_type: RoadType
    road_index: int
    spline_t: float
    lane_offset: float
    active: bool = False
    despawn_alpha: float = 1.0

    @property
    def speed_kmh(self) -> float:
        return self.speed_mps * 3.6
