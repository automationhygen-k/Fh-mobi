"""Main game engine composition root with world life systems and cinematic first launch."""
from __future__ import annotations

from dataclasses import dataclass, field

from Source.Camera.third_person_camera import ThirdPersonCamera
from Source.Cinematics.cinematic_camera import CinematicCamera
from Source.Cinematics.intro_cinematic import IntroCinematic
from Source.Core.Debug.debug_overlay import DebugOverlay
from Source.Core.Debug.logger import logger
from Source.Core.Math.vector import Vec3
from Source.Garage.garage_system import GarageSystem
from Source.Player.player_controller import PlayerController
from Source.SaveSystem.save_manager import FirstLaunchManager, SaveManager
from Source.Traffic.traffic_manager import TrafficManager
from Source.UI.driving_hud import DrivingHUD
from Source.UI.touch_controls import TouchDriveCommand
from Source.UI.touch_input import TouchState
from Source.UI.ui_renderer import UIDrawCommand, UIRenderer
from Source.World.Environment.lod import get_lod_level
from Source.World.RoadSystem.road_generator import RoadSegment
from Source.World.Streaming.chunk_streaming_manager import ChunkStreamingManager


@dataclass
class FrameInput:
    touch_state: TouchState | None = None
    toggle_key: str | None = None


@dataclass
class FrameOutput:
    draw_commands: list[UIDrawCommand] = field(default_factory=list)
    drive_command: TouchDriveCommand = field(default_factory=TouchDriveCommand)


class OpenWorldEngine:
    def __init__(self, seed: int):
        self.seed = seed
        self.streaming = ChunkStreamingManager(seed)

        self.save_manager = SaveManager('save_data.json')
        self.first_launch = FirstLaunchManager(self.save_manager)

        spawn_x, spawn_y = 0.0, 0.0
        spawn_height = self.streaming.terrain_gen.sample_height(spawn_x, spawn_y)
        self.player = PlayerController.spawn(Vec3(spawn_x, spawn_y, spawn_height + 0.8))

        self.garage = GarageSystem.create(self.first_launch, Vec3(spawn_x, spawn_y, spawn_height + 0.8))
        if not self.first_launch.is_first_launch:
            self.garage.spawn_vehicle(self.player.vehicle)

        self.camera = ThirdPersonCamera(position=Vec3(-10.0, -10.0, spawn_height + 8.0))
        self.cinematic_camera = CinematicCamera()
        self.intro_cinematic = IntroCinematic(self.cinematic_camera)
        if self.first_launch.is_first_launch:
            self.intro_cinematic.start(self.player.position)

        self.traffic = TrafficManager(seed + 404)

        self.debug = DebugOverlay()
        self.driving_hud = DrivingHUD()
        self.ui_renderer = UIRenderer()
        self.last_output = FrameOutput()

    def _nearby_roads(self) -> list[RoadSegment]:
        roads: list[RoadSegment] = []
        for chunk in self.streaming.active_chunks():
            roads.extend(chunk.roads)
        return roads

    def _terrain_height_safe(self) -> float | None:
        try:
            return self.streaming.terrain_gen.sample_height(self.player.position.x, self.player.position.y)
        except Exception:
            return None

    def tick(self, frame_input: FrameInput, delta_time: float) -> None:
        self.streaming.update(self.player.position)
        roads = self._nearby_roads()
        terrain_h = self._terrain_height_safe()

        cinematic_done = True
        if self.first_launch.is_first_launch:
            cinematic_done = self.intro_cinematic.update(
                delta_time,
                self.player.vehicle,
                terrain_h if terrain_h is not None else self.player.vehicle.last_valid_ground_height,
                roads,
            )
            if cinematic_done:
                self.first_launch.mark_completed()
                # Land and set garage location at final cinematic spot.
                self.garage.garage_position = self.player.position
                self.first_launch.set_last_garage_position(self.player.position.x, self.player.position.y, self.player.position.z)

        if cinematic_done:
            self.player.update(delta_time, frame_input.touch_state, terrain_h, roads)
            self.camera.update(self.player.vehicle, self.streaming.terrain_gen, delta_time)
            self.traffic.update(delta_time, self.player.position, roads)

            # Autosave when entering garage.
            self.garage.update(self.player.vehicle)

        if frame_input.toggle_key:
            self.debug.handle_key(frame_input.toggle_key)

        for chunk in self.streaming.active_chunks():
            for prop in chunk.props:
                dist = ((prop.position.x - self.player.position.x) ** 2 + (prop.position.y - self.player.position.y) ** 2) ** 0.5
                _ = get_lod_level(dist)

        drive_cmd = self.player.touch_controls.resolve(frame_input.touch_state)
        hud_state = self.driving_hud.build(self.player.vehicle, drive_cmd.throttle)
        draws = self.ui_renderer.render(hud_state, drive_cmd)
        self.last_output = FrameOutput(draw_commands=draws, drive_command=drive_cmd)

    def shutdown(self) -> None:
        self.first_launch.set_player_position(self.player.position.x, self.player.position.y, self.player.position.z)

    def debug_snapshot(self) -> dict[str, int | float | str | tuple[float, float, float]]:
        active = self.streaming.active_chunks()
        terrain_sample = self.streaming.terrain_gen.sample_biome(
            self.player.position.x,
            self.player.position.y,
            self.streaming.terrain_gen.sample_height(self.player.position.x, self.player.position.y),
        )
        return {
            'vehicle_pos': (self.player.position.x, self.player.position.y, self.player.position.z),
            'vehicle_speed_kmh': round(self.player.vehicle.speed_kmh, 2),
            'vehicle_steering_angle': round(self.player.vehicle.steering_angle, 3),
            'vehicle_engine_power': round(self.player.vehicle.engine_force, 2),
            'terrain_type': terrain_sample.biome.value,
            'traffic_active': len(self.traffic.active_vehicles()),
            'cinematic_active': int(self.intro_cinematic.state.active),
            'cinematic_stage': self.intro_cinematic.state.stage,
            'camera_pos': (self.camera.position.x, self.camera.position.y, self.camera.position.z),
            'active_chunks': len(active),
            'pending_tasks': self.streaming.executor.pending,
            'chunk_grid_visible': int(self.debug.show_chunk_grid),
            'biome_overlay_visible': int(self.debug.show_biome_colors),
            'road_overlay_visible': int(self.debug.show_road_network),
            'draw_commands': len(self.last_output.draw_commands),
        }


def run_demo() -> None:
    from Source.UI.touch_input import TouchPoint

    engine = OpenWorldEngine(seed=202501)
    for i in range(360):
        steer_x = 0.22 + (0.08 if i % 50 < 25 else -0.08)
        touch = TouchState(touches=[
            TouchPoint(finger_id=1, x_norm=steer_x, y_norm=0.84, pressed=True),
            TouchPoint(finger_id=2, x_norm=0.86, y_norm=0.88, pressed=True),
        ])
        frame = FrameInput(touch_state=touch)
        if i == 8:
            frame.toggle_key = 'F1'
        if i == 16:
            frame.toggle_key = 'F2'
        if i == 24:
            frame.toggle_key = 'F3'
        engine.tick(frame, 1 / 60)

    engine.shutdown()
    logger.info('Engine snapshot: %s', engine.debug_snapshot())


if __name__ == '__main__':
    run_demo()
