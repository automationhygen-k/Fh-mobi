using UnityEngine;

namespace ProceduralWorldLife
{
    /// <summary>
    /// Generic road anchor response for environment spawners.
    /// </summary>
    public struct RoadAnchor
    {
        public Vector3 Position;
        public Vector3 Forward;
        public float SlopeDegrees;
    }

    /// <summary>
    /// Procedural road query abstraction for spawning near existing roads.
    /// </summary>
    public interface IRoadAnchorProvider
    {
        bool TrySampleRoadAhead(Vector3 playerPosition, Vector3 playerForward, float minDistance, float maxDistance, out RoadAnchor anchor);
    }

    /// <summary>
    /// Creates fuel station gameplay hubs beside road anchors.
    /// </summary>
    public sealed class FuelStationSpawner : MonoBehaviour
    {
        [SerializeField] private FuelStation stationPrefab;
        [SerializeField] private float sideOffset = 12f;

        public FuelStation SpawnStation(RoadAnchor anchor, Transform parent)
        {
            if (stationPrefab == null) return null;

            Vector3 side = Vector3.Cross(Vector3.up, anchor.Forward).normalized;
            if (side.sqrMagnitude < 0.01f) side = Vector3.right;

            Vector3 spawnPos = anchor.Position + side * sideOffset;
            Quaternion spawnRot = Quaternion.LookRotation(anchor.Forward.sqrMagnitude > 0.01f ? anchor.Forward : Vector3.forward, Vector3.up);

            FuelStation station = Instantiate(stationPrefab, spawnPos, spawnRot, parent);
            station.InitializeRuntime(
                pedestriansToSpawn: Random.Range(3, 6),
                parkedCarsToSpawn: Random.Range(1, 3)
            );
            return station;
        }
    }
}
