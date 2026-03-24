"""LOD selector for environment object rendering tiers."""
from enum import IntEnum


class LODLevel(IntEnum):
    LOD0 = 0
    LOD1 = 1
    LOD2 = 2
    LOD3 = 3
    BILLBOARD = 4


def get_lod_level(distance: float) -> LODLevel:
    if distance <= 30:
        return LODLevel.LOD0
    if distance <= 120:
        return LODLevel.LOD1
    if distance <= 300:
        return LODLevel.LOD2
    if distance <= 600:
        return LODLevel.LOD3
    return LODLevel.BILLBOARD
