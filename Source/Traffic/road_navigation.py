"""Road spline navigation for AI traffic vehicles."""
from __future__ import annotations

from dataclasses import dataclass
import math

from Source.Core.Math.vector import Vec2, Vec3
from Source.Core.Utilities.hash_utils import hash_combine, hash_to_unit_float
from Source.Traffic.traffic_vehicle import TrafficVehicle
from Source.World.RoadSystem.road_generator import RoadSegment


@dataclass
class NavigationSnapshot:
    forward: Vec2
    road_index: int
    spline_t: float


class RoadNavigation:
    def __init__(self, seed: int):
        self.seed = seed

    @staticmethod
    def _lerp(a: Vec2, b: Vec2, t: float) -> Vec2:
        return Vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t)

    @staticmethod
    def _segment_point_and_dir(points: list[Vec2], t: float) -> tuple[Vec2, Vec2]:
        if len(points) < 2:
            p = points[0] if points else Vec2(0.0, 0.0)
            return p, Vec2(1.0, 0.0)

        max_seg = len(points) - 1
        seg_float = max(0.0, min(0.9999, t)) * max_seg
        seg_idx = min(max_seg - 1, int(seg_float))
        local_t = seg_float - seg_idx

        p0 = points[seg_idx]
        p1 = points[seg_idx + 1]
        pos = RoadNavigation._lerp(p0, p1, local_t)
        forward = (p1 - p0).normalized()
        return pos, forward

    def _pick_branch(self, position: Vec2, roads: list[RoadSegment], entropy: int) -> int:
        # Simple intersection-like branch pick: select among roads whose endpoints are near current position.
        candidates: list[int] = []
        for i, road in enumerate(roads):
            if not road.points:
                continue
            if (road.points[0] - position).length() < 35.0 or (road.points[-1] - position).length() < 35.0:
                candidates.append(i)
        if not candidates:
            return -1
        pick = int(hash_to_unit_float(hash_combine(self.seed, entropy, len(candidates))) * len(candidates)) % len(candidates)
        return candidates[pick]

    def step(self, traffic: TrafficVehicle, roads: list[RoadSegment], dt: float) -> NavigationSnapshot:
        if not roads:
            return NavigationSnapshot(forward=Vec2(1.0, 0.0), road_index=traffic.road_index, spline_t=traffic.spline_t)

        road_idx = max(0, min(len(roads) - 1, traffic.road_index))
        road = roads[road_idx]
        _, forward = self._segment_point_and_dir(road.points, traffic.spline_t)

        step_t = 0.0
        if road.points:
            approx_len = max(40.0, (len(road.points) - 1) * 40.0)
            step_t = (traffic.speed_mps * dt) / approx_len

        next_t = traffic.spline_t + step_t
        if next_t >= 1.0:
            branch_idx = self._pick_branch(Vec2(traffic.position.x, traffic.position.y), roads, traffic.vehicle_id)
            if branch_idx >= 0:
                road_idx = branch_idx
            next_t = next_t % 1.0
            road = roads[road_idx]
            _, forward = self._segment_point_and_dir(road.points, next_t)

        pos2d, forward = self._segment_point_and_dir(road.points, next_t)
        side = Vec2(-forward.y, forward.x)
        pos2d = pos2d + side * traffic.lane_offset

        yaw = math.atan2(forward.y, forward.x)
        traffic.position = Vec3(pos2d.x, pos2d.y, traffic.position.z)
        traffic.yaw = yaw

        return NavigationSnapshot(forward=forward, road_index=road_idx, spline_t=next_t)
