"""Persistent save and first-launch state management."""
from __future__ import annotations

import json
from pathlib import Path
from typing import Any


DEFAULT_SAVE_DATA: dict[str, Any] = {
    'first_launch': True,
    'last_garage_position': None,
    'player_vehicle': 'default',
    'player_position': None,
}


class SaveManager:
    def __init__(self, file_path: str = 'save_data.json'):
        self.file_path = Path(file_path)

    def load(self) -> dict[str, Any]:
        if not self.file_path.exists():
            return dict(DEFAULT_SAVE_DATA)
        try:
            data = json.loads(self.file_path.read_text(encoding='utf-8'))
        except Exception:
            return dict(DEFAULT_SAVE_DATA)

        merged = dict(DEFAULT_SAVE_DATA)
        merged.update(data)
        return merged

    def save(self, data: dict[str, Any]) -> None:
        merged = dict(DEFAULT_SAVE_DATA)
        merged.update(data)
        self.file_path.write_text(json.dumps(merged, indent=2), encoding='utf-8')


class FirstLaunchManager:
    def __init__(self, save_manager: SaveManager):
        self.save_manager = save_manager
        self.data = self.save_manager.load()

    @property
    def is_first_launch(self) -> bool:
        return bool(self.data.get('first_launch', True))

    def mark_completed(self) -> None:
        self.data['first_launch'] = False
        self.save_manager.save(self.data)

    def set_player_position(self, x: float, y: float, z: float) -> None:
        self.data['player_position'] = [x, y, z]
        self.save_manager.save(self.data)

    def set_last_garage_position(self, x: float, y: float, z: float) -> None:
        self.data['last_garage_position'] = [x, y, z]
        self.save_manager.save(self.data)

    def get_last_garage_position(self) -> tuple[float, float, float] | None:
        pos = self.data.get('last_garage_position')
        if isinstance(pos, list) and len(pos) == 3:
            return float(pos[0]), float(pos[1]), float(pos[2])
        return None
