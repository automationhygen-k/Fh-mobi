using UnityEngine;

namespace ProceduralWorldLife
{
    public enum SettlementType
    {
        VillageCluster,
        RoadsideMotel,
        RestStop,
        ParkingPlaza,
        ScenicViewpoint
    }

    /// <summary>
    /// Spawns settlement variants near roads using lightweight prefabs.
    /// </summary>
    public sealed class SettlementSpawner : MonoBehaviour
    {
        [SerializeField] private GameObject[] settlementPrefabs;
        [SerializeField] private float sideOffset = 18f;

        public GameObject Spawn(SettlementType type, RoadAnchor anchor, Transform parent)
        {
            if (settlementPrefabs == null || settlementPrefabs.Length == 0) return null;

            int idx = Mathf.Clamp((int)type, 0, settlementPrefabs.Length - 1);
            GameObject prefab = settlementPrefabs[idx] != null ? settlementPrefabs[idx] : settlementPrefabs[0];
            if (prefab == null) return null;

            Vector3 side = Vector3.Cross(Vector3.up, anchor.Forward).normalized;
            if (side.sqrMagnitude < 0.01f) side = Vector3.right;

            Vector3 spawnPos = anchor.Position + side * sideOffset;
            Quaternion rot = Quaternion.LookRotation(anchor.Forward.sqrMagnitude > 0.01f ? anchor.Forward : Vector3.forward, Vector3.up);

            return Instantiate(prefab, spawnPos, rot, parent);
        }
    }
}
