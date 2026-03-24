"""Driving HUD view-model for minimal immersive mobile UI."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Vehicles.vehicle import Vehicle


@dataclass
class DrivingHUDState:
    speed_kmh: int
    gear: str
    throttle_ratio: float
    minimap_label: str


class DrivingHUD:
    def build(self, vehicle: Vehicle, throttle: float) -> DrivingHUDState:
        speed = int(max(0.0, vehicle.speed_kmh))
        gear = 'D' if speed > 1 else 'N'
        return DrivingHUDState(
            speed_kmh=speed,
            gear=gear,
            throttle_ratio=max(0.0, min(1.0, throttle)),
            minimap_label='MINIMAP',
        )
