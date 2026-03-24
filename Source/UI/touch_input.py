"""Touch input data model and fail-safe parsing helpers."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass
class TouchPoint:
    finger_id: int
    x_norm: float  # normalized screen coordinates [0..1]
    y_norm: float
    pressed: bool


@dataclass
class TouchState:
    touches: list[TouchPoint]

    @classmethod
    def empty(cls) -> "TouchState":
        return cls(touches=[])

    def active_touches(self) -> list[TouchPoint]:
        return [t for t in self.touches if t.pressed]
