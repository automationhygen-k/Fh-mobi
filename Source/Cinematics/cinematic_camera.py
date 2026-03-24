"""Cinematic camera sequencing for first-launch intro."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.vector import Vec3


@dataclass
class CinematicCameraState:
    position: Vec3
    look_at: Vec3
    blend_to_gameplay: float
    stage_name: str


class CinematicCamera:
    def __init__(self) -> None:
        self.state = CinematicCameraState(
            position=Vec3(0.0, 0.0, 300.0),
            look_at=Vec3(0.0, 0.0, 0.0),
            blend_to_gameplay=0.0,
            stage_name='sky_wide',
        )

    def set_stage(self, stage: str, position: Vec3, look_at: Vec3, blend: float = 0.0) -> None:
        self.state = CinematicCameraState(position=position, look_at=look_at, blend_to_gameplay=blend, stage_name=stage)
