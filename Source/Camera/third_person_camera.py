"""Third-person chase camera with speed-adaptive distance and turn tilt."""
from __future__ import annotations

from dataclasses import dataclass
import math

from Source.Core.Math.vector import Vec3
from Source.Vehicles.vehicle import Vehicle
from Source.World.Terrain.terrain_generator import TerrainGenerator


@dataclass
class ThirdPersonCamera:
    position: Vec3
    distance: float = 12.0
    max_distance: float = 19.0
    height: float = 5.0
    smooth_factor: float = 8.5
    turn_tilt_strength: float = 0.9

    def update(self, vehicle: Vehicle, terrain: TerrainGenerator, delta_time: float) -> None:
        speed_ratio = max(0.0, min(1.0, vehicle.speed_mps / max(vehicle.max_speed, 0.1)))
        dynamic_distance = self.distance + (self.max_distance - self.distance) * speed_ratio

        backward = Vec3(-math.cos(vehicle.rotation_yaw), -math.sin(vehicle.rotation_yaw), 0.0)
        lateral_tilt = vehicle.steering_angle * self.turn_tilt_strength

        desired = Vec3(
            vehicle.position.x + backward.x * dynamic_distance,
            vehicle.position.y + backward.y * dynamic_distance,
            vehicle.position.z + self.height + lateral_tilt,
        )

        ground_height = terrain.sample_height(desired.x, desired.y)
        if desired.z < ground_height + 1.6:
            desired = Vec3(desired.x, desired.y, ground_height + 1.6)

        alpha = min(1.0, self.smooth_factor * delta_time)
        self.position = Vec3(
            self.position.x + (desired.x - self.position.x) * alpha,
            self.position.y + (desired.y - self.position.y) * alpha,
            self.position.z + (desired.z - self.position.z) * alpha,
        )
