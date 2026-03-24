using System.Collections.Generic;
using UnityEngine;

namespace ProceduralWorldLife
{
    /// <summary>
    /// Manages fuel station spawn/despawn and refuel interactions.
    /// </summary>
    public sealed class FuelStationManager : MonoBehaviour
    {
        [SerializeField] private FuelStationSpawner spawner;
        [SerializeField] private MonoBehaviour roadProviderBehaviour;

        [Header("Spawn Rules")]
        [SerializeField] private float spawnMinAheadDistance = 300f;
        [SerializeField] private float spawnMaxAheadDistance = 800f;
        [SerializeField] private float despawnBehindDistance = 2000f;
        [SerializeField, Range(1, 2)] private int maxActiveStations = 2;
        [SerializeField] private float lowFuelThreshold = 0.2f;
        [SerializeField] private float longRoadSpawnInterval = 95f;

        private IRoadAnchorProvider _roadProvider;
        private readonly List<FuelStation> _activeStations = new List<FuelStation>();
        private float _distanceSinceLastSpawn;

        private void Awake()
        {
            _roadProvider = roadProviderBehaviour as IRoadAnchorProvider;
        }

        public void Step(EnvironmentRuntimeContext context, IPlayerFuelStatus fuelStatus)
        {
            if (_roadProvider == null || spawner == null) return;

            _distanceSinceLastSpawn += context.DeltaTime * 30f; // approximate covered distance
            DespawnBehind(context.PlayerPosition, context.PlayerForward);
            UpdateStationVisuals(context);
            HandleRefuelInteraction(context, fuelStatus);

            bool needFuelSpawn = context.PlayerFuel01 <= lowFuelThreshold;
            bool longRoadSpawn = _distanceSinceLastSpawn >= longRoadSpawnInterval;

            if ((needFuelSpawn || longRoadSpawn) && _activeStations.Count < maxActiveStations)
            {
                TrySpawn(context);
            }
        }

        private void TrySpawn(EnvironmentRuntimeContext context)
        {
            if (!_roadProvider.TrySampleRoadAhead(context.PlayerPosition, context.PlayerForward, spawnMinAheadDistance, spawnMaxAheadDistance, out RoadAnchor anchor))
                return;

            FuelStation station = spawner.SpawnStation(anchor, transform);
            if (station == null) return;

            _activeStations.Add(station);
            _distanceSinceLastSpawn = 0f;
        }

        private void DespawnBehind(Vector3 playerPos, Vector3 playerForward)
        {
            for (int i = _activeStations.Count - 1; i >= 0; i--)
            {
                FuelStation station = _activeStations[i];
                if (station == null)
                {
                    _activeStations.RemoveAt(i);
                    continue;
                }

                Vector3 toStation = station.transform.position - playerPos;
                bool behind = Vector3.Dot(playerForward, toStation) < 0f;
                if (behind && toStation.magnitude > despawnBehindDistance)
                {
                    Destroy(station.gameObject);
                    _activeStations.RemoveAt(i);
                }
            }
        }

        private void UpdateStationVisuals(EnvironmentRuntimeContext context)
        {
            for (int i = 0; i < _activeStations.Count; i++)
            {
                if (_activeStations[i] == null) continue;
                _activeStations[i].ApplyEnvironmentState(context.IsNight, context.IsRaining, context.IsFoggy);
            }
        }

        private void HandleRefuelInteraction(EnvironmentRuntimeContext context, IPlayerFuelStatus fuelStatus)
        {
            if (fuelStatus == null) return;

            bool holdRefuel = Input.GetKey(KeyCode.E);
            bool playerStopped = fuelStatus.GetSpeedMps() < 0.6f;

            for (int i = 0; i < _activeStations.Count; i++)
            {
                FuelStation station = _activeStations[i];
                if (station == null) continue;

                if (playerStopped && station.IsNearPump(context.PlayerPosition))
                {
                    station.TryRefuel(fuelStatus, holdRefuel);
                }
            }
        }
    }
}
