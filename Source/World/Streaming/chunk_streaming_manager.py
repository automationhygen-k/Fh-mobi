"""Chunk streaming manager with async generation budget and fail-safe fallback generation."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Debug.logger import logger
from Source.Core.Math.vector import Vec3
from Source.Core.Utilities.async_budget import FrameBudgetExecutor
from Source.World.Environment.prop_spawner import PropSpawner
from Source.World.Environment.village_generator import VillageGenerator
from Source.World.RoadSystem.road_generator import RoadGenerator
from Source.World.Streaming.chunk import Chunk, ChunkState
from Source.World.Terrain.terrain_generator import TerrainChunkData, TerrainGenerator


@dataclass
class ChunkRuntimeConfig:
    chunk_size: int = 512
    active_radius: int = 3


class ChunkStreamingManager:
    def __init__(self, seed: int, config: ChunkRuntimeConfig | None = None):
        self.seed = seed
        self.config = config or ChunkRuntimeConfig()

        self.terrain_gen = TerrainGenerator(seed)
        self.road_gen = RoadGenerator(seed)
        self.village_gen = VillageGenerator(seed)
        self.prop_spawner = PropSpawner(seed, self.terrain_gen)

        self._chunks: dict[tuple[int, int], Chunk] = {}
        self.executor = FrameBudgetExecutor(budget_ms=10.0)

    def _player_chunk(self, position: Vec3) -> tuple[int, int]:
        cs = self.config.chunk_size
        return int(position.x // cs), int(position.y // cs)

    def _desired_coords(self, center: tuple[int, int]) -> set[tuple[int, int]]:
        cx, cy = center
        r = self.config.active_radius
        return {(x, y) for x in range(cx - r, cx + r + 1) for y in range(cy - r, cy + r + 1)}

    def _schedule_load(self, coord: tuple[int, int]) -> None:
        chunk = self._chunks.setdefault(coord, Chunk(*coord))
        if chunk.state in (ChunkState.LOADING, ChunkState.ACTIVE):
            return
        chunk.state = ChunkState.LOADING

        def terrain_task() -> None:
            try:
                chunk.terrain = self.terrain_gen.generate_chunk(chunk.coord_x, chunk.coord_y, self.config.chunk_size)
            except Exception as exc:
                logger.error('Terrain generation failed for %s: %s', coord, exc)
                chunk.terrain = TerrainChunkData(samples=[], water_level=self.terrain_gen.water_level)

        def road_task() -> None:
            try:
                chunk.roads = self.road_gen.generate_chunk_roads(chunk.coord_x, chunk.coord_y, self.config.chunk_size)
            except Exception as exc:
                logger.error('Road generation failed for %s: %s', coord, exc)
                chunk.roads = []

        def village_task() -> None:
            try:
                chunk.village = self.village_gen.generate(chunk.coord_x, chunk.coord_y, chunk.roads)
            except Exception as exc:
                logger.error('Village generation failed for %s: %s', coord, exc)
                chunk.village = None

        def props_finalize_task() -> None:
            try:
                chunk.props = self.prop_spawner.generate_chunk_props(chunk.coord_x, chunk.coord_y, self.config.chunk_size, chunk.roads)
            except Exception as exc:
                logger.error('Prop generation failed for %s: %s', coord, exc)
                chunk.props = []
            chunk.state = ChunkState.ACTIVE

        for task in (terrain_task, road_task, village_task, props_finalize_task):
            self.executor.schedule(task)

    def _schedule_unload(self, coord: tuple[int, int]) -> None:
        chunk = self._chunks.get(coord)
        if not chunk or chunk.state in (ChunkState.UNLOADING, ChunkState.UNLOADED):
            return
        chunk.state = ChunkState.UNLOADING

        def task() -> None:
            chunk.terrain = None
            chunk.roads = []
            chunk.village = None
            chunk.props = []
            chunk.state = ChunkState.UNLOADED
            del self._chunks[coord]

        self.executor.schedule(task)

    def update(self, player_position: Vec3) -> None:
        center = self._player_chunk(player_position)
        desired = self._desired_coords(center)
        current = set(self._chunks.keys())

        for coord in desired - current:
            self._schedule_load(coord)
        for coord in current - desired:
            self._schedule_unload(coord)

        self.executor.update()

    def active_chunks(self) -> list[Chunk]:
        return [c for c in self._chunks.values() if c.state == ChunkState.ACTIVE]
