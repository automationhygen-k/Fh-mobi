"""Vehicle state container used by physics and controller systems."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.vector import Vec3


@dataclass
class Vehicle:
    # Kinematic state
    position: Vec3
    velocity: Vec3
    rotation_yaw: float = 0.0
    steering_angle: float = 0.0

    # Input -> force state
    engine_force: float = 0.0
    brake_force: float = 0.0

    # Physical parameters
    mass: float = 1200.0
    max_speed: float = 40.0
    friction: float = 0.98
    traction: float = 1.0

    # Fail-safe terrain grounding
    last_valid_ground_height: float = 0.0

    @property
    def speed_mps(self) -> float:
        return self.velocity.length()

    @property
    def speed_kmh(self) -> float:
        return self.speed_mps * 3.6
