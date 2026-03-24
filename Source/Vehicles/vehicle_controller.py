"""Input-facing vehicle controller that wraps physics and command state."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Vehicles.vehicle import Vehicle
from Source.Vehicles.vehicle_physics import VehiclePhysics
from Source.World.RoadSystem.road_generator import RoadSegment


@dataclass
class VehicleCommand:
    throttle: float = 0.0
    brake: float = 0.0
    steering: float = 0.0


class VehicleController:
    def __init__(self, vehicle: Vehicle):
        self.vehicle = vehicle
        self.command = VehicleCommand()
        self.physics = VehiclePhysics()

    def set_command(self, throttle: float, brake: float, steering: float) -> None:
        self.command = VehicleCommand(throttle=throttle, brake=brake, steering=steering)

    def update(self, dt: float, terrain_height: float | None, nearby_roads: list[RoadSegment]) -> None:
        self.physics.step(
            self.vehicle,
            throttle=self.command.throttle,
            brake=self.command.brake,
            steering_input=self.command.steering,
            dt=dt,
            terrain_height=terrain_height,
            nearby_roads=nearby_roads,
        )
