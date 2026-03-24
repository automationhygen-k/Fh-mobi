"""Deterministic integer hash utilities for reproducible procedural generation."""


def hash32(value: int) -> int:
    value = (value ^ 61) ^ (value >> 16)
    value = value + (value << 3)
    value = value ^ (value >> 4)
    value = value * 0x27D4EB2D
    value = value ^ (value >> 15)
    return value & 0xFFFFFFFF


def hash_combine(*values: int) -> int:
    seed = 0x811C9DC5
    for value in values:
        seed = hash32(seed ^ (value & 0xFFFFFFFF))
    return seed


def hash_to_unit_float(value: int) -> float:
    return (hash32(value) & 0xFFFFFF) / float(0xFFFFFF)
