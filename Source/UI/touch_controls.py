"""Touch controls mapper: left drag steering + right gas/brake buttons."""
from __future__ import annotations

from dataclasses import dataclass

from Source.UI.touch_input import TouchState


@dataclass
class TouchDriveCommand:
    steering: float = 0.0
    throttle: float = 0.0
    brake: float = 0.0


class TouchControls:
    # UI layout in normalized coordinates.
    steering_zone_x_max = 0.5
    gas_center = (0.86, 0.88)
    gas_radius = 0.11
    brake_center = (0.84, 0.72)
    brake_radius = 0.085

    def __init__(self) -> None:
        self._steer_anchor_x: float | None = None

    @staticmethod
    def _inside_circle(x: float, y: float, cx: float, cy: float, r: float) -> bool:
        return (x - cx) ** 2 + (y - cy) ** 2 <= r ** 2

    @staticmethod
    def _clamp(v: float, lo: float, hi: float) -> float:
        return max(lo, min(hi, v))

    def resolve(self, state: TouchState | None) -> TouchDriveCommand:
        if state is None:
            return TouchDriveCommand()

        cmd = TouchDriveCommand()
        left_touch = None

        for touch in state.active_touches():
            x, y = touch.x_norm, touch.y_norm
            if x <= self.steering_zone_x_max and left_touch is None:
                left_touch = touch
            if self._inside_circle(x, y, *self.gas_center, self.gas_radius):
                cmd.throttle = 1.0
            if self._inside_circle(x, y, *self.brake_center, self.brake_radius):
                cmd.brake = 1.0

        # Steering from horizontal drag delta on the left half.
        if left_touch is not None:
            if self._steer_anchor_x is None:
                self._steer_anchor_x = left_touch.x_norm
            drag = left_touch.x_norm - self._steer_anchor_x
            cmd.steering = self._clamp(drag * 5.0, -1.0, 1.0)
        else:
            self._steer_anchor_x = None

        # Ensure conflicting pedals still keep braking precedence.
        if cmd.brake > 0.0:
            cmd.throttle *= 0.25

        return cmd
