"""UI renderer abstraction producing styled draw commands for a mobile driving HUD."""
from __future__ import annotations

from dataclasses import dataclass

from Source.UI.driving_hud import DrivingHUDState
from Source.UI.touch_controls import TouchDriveCommand


@dataclass
class UIDrawCommand:
    element: str
    style: dict[str, float | str]
    payload: dict[str, float | str]


class UIRenderer:
    def render(self, hud: DrivingHUDState, touch: TouchDriveCommand) -> list[UIDrawCommand]:
        # Minimal, semi-transparent, softly glowing style inspired by brief.
        base_style = {'alpha': 0.68, 'glow': 0.14, 'theme': 'clean_modern'}

        speedo = UIDrawCommand(
            element='speedometer_dial',
            style={**base_style, 'x': 0.5, 'y': 0.9, 'size': 0.22},
            payload={'speed_kmh': float(hud.speed_kmh), 'gear': hud.gear},
        )
        throttle = UIDrawCommand(
            element='throttle_bar',
            style={**base_style, 'x': 0.62, 'y': 0.84, 'width': 0.018, 'height': 0.13},
            payload={'fill': hud.throttle_ratio},
        )
        minimap = UIDrawCommand(
            element='minimap_placeholder',
            style={**base_style, 'x': 0.87, 'y': 0.14, 'size': 0.16},
            payload={'label': hud.minimap_label},
        )
        gas = UIDrawCommand(
            element='gas_button',
            style={**base_style, 'x': 0.86, 'y': 0.88, 'size': 0.11, 'pressed': float(touch.throttle > 0.0)},
            payload={'animation': 'pulse' if touch.throttle > 0.0 else 'idle'},
        )
        brake = UIDrawCommand(
            element='brake_button',
            style={**base_style, 'x': 0.84, 'y': 0.72, 'size': 0.085, 'pressed': float(touch.brake > 0.0)},
            payload={'animation': 'pulse' if touch.brake > 0.0 else 'idle'},
        )
        steering = UIDrawCommand(
            element='steering_wheel_indicator',
            style={**base_style, 'x': 0.18, 'y': 0.82, 'size': 0.2},
            payload={'steering': touch.steering},
        )
        return [speedo, throttle, minimap, gas, brake, steering]
