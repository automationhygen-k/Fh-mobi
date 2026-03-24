"""Garage system for persistent spawn/resume and autosave trigger zones."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.vector import Vec3
from Source.SaveSystem.save_manager import FirstLaunchManager
from Source.Vehicles.vehicle import Vehicle


@dataclass
class GarageSystem:
    first_launch_manager: FirstLaunchManager
    garage_position: Vec3
    garage_radius: float = 22.0

    @classmethod
    def create(cls, first_launch_manager: FirstLaunchManager, fallback_position: Vec3) -> "GarageSystem":
        saved = first_launch_manager.get_last_garage_position()
        if saved is None:
            garage_position = fallback_position
            first_launch_manager.set_last_garage_position(garage_position.x, garage_position.y, garage_position.z)
        else:
            garage_position = Vec3(saved[0], saved[1], saved[2])
        return cls(first_launch_manager=first_launch_manager, garage_position=garage_position)

    def spawn_vehicle(self, vehicle: Vehicle) -> None:
        vehicle.position = Vec3(self.garage_position.x, self.garage_position.y, self.garage_position.z)

    def update(self, vehicle: Vehicle) -> bool:
        dx = vehicle.position.x - self.garage_position.x
        dy = vehicle.position.y - self.garage_position.y
        inside = dx * dx + dy * dy <= self.garage_radius ** 2
        if inside:
            self.first_launch_manager.set_player_position(vehicle.position.x, vehicle.position.y, vehicle.position.z)
        return inside
