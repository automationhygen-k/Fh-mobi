"""First-launch intro cinematic with airplane + parachute drop sequence."""
from __future__ import annotations

from dataclasses import dataclass

from Source.Cinematics.cinematic_camera import CinematicCamera
from Source.Core.Math.vector import Vec3
from Source.Vehicles.vehicle import Vehicle
from Source.World.RoadSystem.road_generator import RoadSegment


@dataclass
class IntroState:
    active: bool = False
    complete: bool = False
    timer: float = 0.0
    stage: int = 0
    airplane_pos: Vec3 = Vec3(0.0, -200.0, 420.0)
    parachute_open: bool = False


class IntroCinematic:
    def __init__(self, camera: CinematicCamera):
        self.camera = camera
        self.state = IntroState()

    def start(self, start_position: Vec3) -> None:
        self.state = IntroState(active=True, complete=False, timer=0.0, stage=0, airplane_pos=Vec3(start_position.x, start_position.y - 220.0, 420.0))

    @staticmethod
    def _nearest_road_point(start: Vec3, roads: list[RoadSegment]) -> Vec3:
        best = Vec3(start.x, start.y, start.z)
        best_d2 = float('inf')
        for road in roads:
            for p in road.points:
                d2 = (p.x - start.x) ** 2 + (p.y - start.y) ** 2
                if d2 < best_d2:
                    best_d2 = d2
                    best = Vec3(p.x, p.y, start.z)
        return best

    def update(self, dt: float, player_vehicle: Vehicle, terrain_height: float, roads: list[RoadSegment]) -> bool:
        if not self.state.active or self.state.complete:
            return True

        self.state.timer += dt
        t = self.state.timer

        if t < 2.5:
            self.state.stage = 0
            self.camera.set_stage('wide_sky_shot', Vec3(player_vehicle.position.x - 220.0, player_vehicle.position.y - 180.0, 500.0), player_vehicle.position)
        elif t < 5.0:
            self.state.stage = 1
            self.state.airplane_pos = Vec3(self.state.airplane_pos.x + 40.0 * dt, self.state.airplane_pos.y + 55.0 * dt, self.state.airplane_pos.z)
            self.camera.set_stage('cargo_plane_close', Vec3(self.state.airplane_pos.x - 30.0, self.state.airplane_pos.y - 18.0, self.state.airplane_pos.z + 12.0), self.state.airplane_pos)
            player_vehicle.position = Vec3(self.state.airplane_pos.x, self.state.airplane_pos.y, self.state.airplane_pos.z - 4.0)
        elif t < 8.0:
            self.state.stage = 2
            if t > 6.0:
                self.state.parachute_open = True
            fall_speed = 20.0 if self.state.parachute_open else 65.0
            player_vehicle.position = Vec3(player_vehicle.position.x + 8.0 * dt, player_vehicle.position.y + 5.0 * dt, player_vehicle.position.z - fall_speed * dt)
            self.camera.set_stage('falling_follow', Vec3(player_vehicle.position.x - 18.0, player_vehicle.position.y - 8.0, player_vehicle.position.z + 25.0), player_vehicle.position)
        elif t < 10.5:
            self.state.stage = 3
            target = self._nearest_road_point(player_vehicle.position, roads)
            landed_z = max(terrain_height + 0.8, player_vehicle.position.z - 18.0 * dt)
            player_vehicle.position = Vec3(
                player_vehicle.position.x + (target.x - player_vehicle.position.x) * 0.06,
                player_vehicle.position.y + (target.y - player_vehicle.position.y) * 0.06,
                landed_z,
            )
            self.camera.set_stage('landing_shot', Vec3(player_vehicle.position.x - 12.0, player_vehicle.position.y + 10.0, player_vehicle.position.z + 7.0), player_vehicle.position)
        else:
            self.state.stage = 4
            self.camera.set_stage('blend_to_gameplay', Vec3(player_vehicle.position.x - 10.0, player_vehicle.position.y - 10.0, player_vehicle.position.z + 8.0), player_vehicle.position, blend=1.0)
            self.state.complete = True
            self.state.active = False
            player_vehicle.position = Vec3(player_vehicle.position.x, player_vehicle.position.y, terrain_height + 0.8)

        return self.state.complete
