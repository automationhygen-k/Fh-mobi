"""Player controller now drives a spawned vehicle through touch commands."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.vector import Vec3
from Source.UI.touch_controls import TouchControls
from Source.UI.touch_input import TouchState
from Source.Vehicles.vehicle import Vehicle
from Source.Vehicles.vehicle_controller import VehicleController
from Source.World.RoadSystem.road_generator import RoadSegment


@dataclass
class PlayerController:
    vehicle_controller: VehicleController
    touch_controls: TouchControls

    @classmethod
    def spawn(cls, start_position: Vec3) -> "PlayerController":
        vehicle = Vehicle(position=start_position, velocity=Vec3(0.0, 0.0, 0.0))
        return cls(vehicle_controller=VehicleController(vehicle), touch_controls=TouchControls())

    @property
    def position(self) -> Vec3:
        return self.vehicle_controller.vehicle.position

    @property
    def vehicle(self) -> Vehicle:
        return self.vehicle_controller.vehicle

    def update(self, dt: float, touch_state: TouchState | None, terrain_height: float | None, nearby_roads: list[RoadSegment]) -> None:
        command = self.touch_controls.resolve(touch_state)
        self.vehicle_controller.set_command(
            throttle=command.throttle,
            brake=command.brake,
            steering=command.steering,
        )
        self.vehicle_controller.update(dt, terrain_height=terrain_height, nearby_roads=nearby_roads)
