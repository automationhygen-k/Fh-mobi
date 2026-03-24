"""Debug visualization state manager with keyboard toggles abstraction."""
from dataclasses import dataclass


@dataclass
class DebugOverlay:
    show_chunk_grid: bool = False
    show_biome_colors: bool = False
    show_road_network: bool = False
    show_prop_points: bool = False

    def handle_key(self, key: str) -> None:
        key = key.upper()
        if key == 'F1':
            self.show_chunk_grid = not self.show_chunk_grid
        elif key == 'F2':
            self.show_biome_colors = not self.show_biome_colors
        elif key == 'F3':
            self.show_road_network = not self.show_road_network
        elif key == 'F4':
            self.show_prop_points = not self.show_prop_points
