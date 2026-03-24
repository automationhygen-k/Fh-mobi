"""AI traffic manager with pooling, spawn/despawn, and safe-distance behavior."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Core.Math.vector import Vec3
from Source.Core.Utilities.hash_utils import hash_combine, hash_to_unit_float
from Source.Traffic.road_navigation import RoadNavigation
from Source.Traffic.traffic_vehicle import TrafficVehicle
from Source.World.RoadSystem.road_generator import RoadSegment, RoadType


@dataclass
class TrafficConfig:
    max_vehicles: int = 20
    spawn_radius: float = 300.0
    despawn_radius: float = 500.0
    safe_distance: float = 18.0


class TrafficManager:
    def __init__(self, seed: int, config: TrafficConfig | None = None):
        self.seed = seed
        self.config = config or TrafficConfig()
        self.nav = RoadNavigation(seed)
        self._pool = [
            TrafficVehicle(
                vehicle_id=i,
                position=Vec3(0.0, 0.0, 0.0),
                yaw=0.0,
                speed_mps=0.0,
                target_speed_mps=10.0,
                road_type=RoadType.VILLAGE,
                road_index=0,
                spline_t=0.0,
                lane_offset=(-1.8 if i % 2 == 0 else 1.8),
                active=False,
            )
            for i in range(self.config.max_vehicles)
        ]

    @staticmethod
    def _dist2(a: Vec3, b: Vec3) -> float:
        return (a.x - b.x) ** 2 + (a.y - b.y) ** 2

    def _speed_range(self, road_type: RoadType) -> tuple[float, float]:
        if road_type == RoadType.HIGHWAY:
            return (16.67, 27.78)  # 60-100 km/h
        if road_type == RoadType.VILLAGE:
            return (5.56, 11.11)  # 20-40 km/h
        if road_type == RoadType.COASTAL:
            return (8.33, 18.06)
        return (7.0, 14.0)

    def _pick_spawn(self, player_pos: Vec3, roads: list[RoadSegment], entropy: int) -> tuple[int, float] | None:
        if not roads:
            return None
        candidates: list[int] = []
        forward_bias_x = 1.0
        for idx, road in enumerate(roads):
            if not road.points:
                continue
            mid = road.points[len(road.points) // 2]
            dx = mid.x - player_pos.x
            dy = mid.y - player_pos.y
            d2 = dx * dx + dy * dy
            if d2 > self.config.spawn_radius * self.config.spawn_radius:
                continue
            # Ahead of player approximation: positive world-x forward bias.
            if dx < -30.0 * forward_bias_x:
                continue
            candidates.append(idx)

        if not candidates:
            return None

        pick = int(hash_to_unit_float(hash_combine(self.seed, entropy, len(candidates))) * len(candidates)) % len(candidates)
        road_idx = candidates[pick]
        spline_t = hash_to_unit_float(hash_combine(self.seed, entropy, road_idx, 99))
        return road_idx, spline_t

    def _spawn_if_needed(self, player_pos: Vec3, roads: list[RoadSegment]) -> None:
        active_count = sum(1 for v in self._pool if v.active)
        if active_count >= self.config.max_vehicles:
            return

        budget = min(2, self.config.max_vehicles - active_count)
        for i in range(budget):
            vehicle = next((v for v in self._pool if not v.active), None)
            if vehicle is None:
                break
            spawn = self._pick_spawn(player_pos, roads, entropy=vehicle.vehicle_id + i)
            if spawn is None:
                return
            road_idx, spline_t = spawn
            road = roads[road_idx]
            lo, hi = self._speed_range(road.road_type)
            t = hash_to_unit_float(hash_combine(self.seed, vehicle.vehicle_id, road_idx, 123))
            speed = lo + (hi - lo) * t

            vehicle.active = True
            vehicle.road_index = road_idx
            vehicle.spline_t = spline_t
            vehicle.road_type = road.road_type
            vehicle.speed_mps = speed * 0.7
            vehicle.target_speed_mps = speed
            vehicle.despawn_alpha = 1.0
            p = road.points[min(len(road.points) - 1, int(spline_t * max(1, len(road.points)-1)))]
            vehicle.position = Vec3(p.x, p.y, player_pos.z)

    def _apply_safe_distance(self, active: list[TrafficVehicle], player_vehicle_pos: Vec3) -> None:
        for vehicle in active:
            # Slow behind player.
            d_player2 = self._dist2(vehicle.position, player_vehicle_pos)
            if d_player2 < (self.config.safe_distance * 1.4) ** 2:
                vehicle.target_speed_mps *= 0.65
            else:
                vehicle.target_speed_mps = min(vehicle.target_speed_mps * 1.01, vehicle.target_speed_mps + 0.01)

        for i, v in enumerate(active):
            for j, other in enumerate(active):
                if i == j:
                    continue
                if self._dist2(v.position, other.position) < self.config.safe_distance ** 2:
                    v.target_speed_mps *= 0.7

    def _despawn_far(self, player_pos: Vec3, dt: float) -> None:
        max_d2 = self.config.despawn_radius ** 2
        for v in self._pool:
            if not v.active:
                continue
            if self._dist2(v.position, player_pos) > max_d2:
                v.despawn_alpha -= dt * 2.0
                if v.despawn_alpha <= 0.0:
                    v.active = False
                    v.speed_mps = 0.0

    def update(self, dt: float, player_pos: Vec3, roads: list[RoadSegment]) -> None:
        self._spawn_if_needed(player_pos, roads)

        active = [v for v in self._pool if v.active]
        self._apply_safe_distance(active, player_pos)

        for v in active:
            # Overtake approximation: if too slow, small lane offset nudge.
            if v.speed_mps < v.target_speed_mps * 0.6:
                v.lane_offset = min(2.4, v.lane_offset + 0.15 * dt)
            else:
                if v.lane_offset > 1.8:
                    v.lane_offset = max(1.8, v.lane_offset - 0.2 * dt)
                elif v.lane_offset < -1.8:
                    v.lane_offset = min(-1.8, v.lane_offset + 0.2 * dt)

            accel = (v.target_speed_mps - v.speed_mps) * 0.9
            v.speed_mps = max(0.0, v.speed_mps + accel * dt)
            nav = self.nav.step(v, roads, dt)
            v.road_index = nav.road_index
            v.spline_t = nav.spline_t

        self._despawn_far(player_pos, dt)

    def active_vehicles(self) -> list[TrafficVehicle]:
        return [v for v in self._pool if v.active]
