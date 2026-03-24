"""Chunk data model and lifecycle states."""
from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Any


class ChunkState(Enum):
    UNLOADED = auto()
    LOADING = auto()
    ACTIVE = auto()
    UNLOADING = auto()


@dataclass
class Chunk:
    coord_x: int
    coord_y: int
    state: ChunkState = ChunkState.UNLOADED
    terrain: Any = None
    roads: list[Any] = field(default_factory=list)
    village: Any = None
    props: list[Any] = field(default_factory=list)

    @property
    def key(self) -> tuple[int, int]:
        return (self.coord_x, self.coord_y)
