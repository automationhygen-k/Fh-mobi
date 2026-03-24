"""Vehicle physics integrator with traction/friction and terrain/road interaction."""
from __future__ import annotations

import math

from Source.Core.Math.vector import Vec2, Vec3
from Source.Vehicles.vehicle import Vehicle
from Source.World.RoadSystem.road_generator import RoadSegment


class VehiclePhysics:
    def __init__(self) -> None:
        self.base_engine_accel = 9000.0
        self.base_brake_accel = 12000.0

    @staticmethod
    def _clamp(value: float, lo: float, hi: float) -> float:
        return max(lo, min(hi, value))

    @staticmethod
    def _distance_to_segment(p: Vec2, a: Vec2, b: Vec2) -> float:
        ab = Vec2(b.x - a.x, b.y - a.y)
        ap = Vec2(p.x - a.x, p.y - a.y)
        denom = ab.x * ab.x + ab.y * ab.y
        if denom == 0:
            return ap.length()
        t = max(0.0, min(1.0, (ap.x * ab.x + ap.y * ab.y) / denom))
        closest = Vec2(a.x + t * ab.x, a.y + t * ab.y)
        return (p - closest).length()

    def is_on_road(self, position: Vec3, roads: list[RoadSegment]) -> bool:
        point = Vec2(position.x, position.y)
        for road in roads:
            for i in range(len(road.points) - 1):
                if self._distance_to_segment(point, road.points[i], road.points[i + 1]) <= 7.0:
                    return True
        return False

    def step(
        self,
        vehicle: Vehicle,
        throttle: float,
        brake: float,
        steering_input: float,
        dt: float,
        terrain_height: float | None,
        nearby_roads: list[RoadSegment],
    ) -> None:
        # Fail-safe if terrain height sampling fails.
        if terrain_height is None:
            terrain_height = vehicle.last_valid_ground_height
        else:
            vehicle.last_valid_ground_height = terrain_height

        road_contact = self.is_on_road(vehicle.position, nearby_roads)
        if road_contact:
            traction = 1.15
            friction = 0.992
            max_speed = vehicle.max_speed * 1.08
        else:
            traction = 0.82
            friction = 0.965
            max_speed = vehicle.max_speed

        vehicle.traction = traction
        vehicle.friction = friction

        throttle = self._clamp(throttle, 0.0, 1.0)
        brake = self._clamp(brake, 0.0, 1.0)
        steering_input = self._clamp(steering_input, -1.0, 1.0)

        vehicle.engine_force = throttle * self.base_engine_accel
        vehicle.brake_force = brake * self.base_brake_accel

        forward = Vec2(math.cos(vehicle.rotation_yaw), math.sin(vehicle.rotation_yaw))
        accel_scalar = (vehicle.engine_force - vehicle.brake_force) / max(vehicle.mass, 1.0)

        acceleration = Vec3(forward.x * accel_scalar * traction, forward.y * accel_scalar * traction, 0.0)
        vehicle.velocity = vehicle.velocity + acceleration * dt

        # Speed-sensitive steering: tight at low speed, wider at high speed.
        speed_ratio = self._clamp(vehicle.speed_mps / max(vehicle.max_speed, 0.1), 0.0, 1.0)
        steer_limit = (0.75 - speed_ratio * 0.5)  # radians normalized factor
        vehicle.steering_angle = steering_input * steer_limit
        yaw_rate = vehicle.steering_angle * (0.9 - speed_ratio * 0.5)
        vehicle.rotation_yaw += yaw_rate * dt

        # Drag/friction and speed clamp.
        vehicle.velocity = vehicle.velocity * friction
        speed = vehicle.speed_mps
        if speed > max_speed:
            vehicle.velocity = vehicle.velocity.normalized() * max_speed

        # Integrate position and lock to terrain.
        vehicle.position = vehicle.position + vehicle.velocity * dt
        if vehicle.position.z < terrain_height + 0.8:
            vehicle.position = Vec3(vehicle.position.x, vehicle.position.y, terrain_height + 0.8)
