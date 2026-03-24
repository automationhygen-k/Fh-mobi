using UnityEngine;

namespace ProceduralWorldLife
{
    /// <summary>
    /// Shared runtime context broadcast to environment subsystems.
    /// </summary>
    public struct EnvironmentRuntimeContext
    {
        public Vector3 PlayerPosition;
        public Vector3 PlayerForward;
        public float PlayerFuel01;
        public bool IsNight;
        public bool IsRaining;
        public bool IsFoggy;
        public float DeltaTime;
    }

    /// <summary>
    /// Optional adapter for player fuel/speed without hard coupling to specific vehicle implementation.
    /// </summary>
    public interface IPlayerFuelStatus
    {
        float GetFuelNormalized();
        void AddFuelNormalized(float delta);
        float GetSpeedMps();
    }

    /// <summary>
    /// Central manager for environment gameplay structures.
    /// Integrates FuelStationManager + SettlementManager with weather/day-night systems.
    /// </summary>
    public sealed class WorldEnvironmentManager : MonoBehaviour
    {
        [Header("Dependencies")]
        [SerializeField] private Transform playerVehicle;
        [SerializeField] private MonoBehaviour playerFuelProvider;
        [SerializeField] private MonoBehaviour weatherSystemProvider;
        [SerializeField] private MonoBehaviour dayNightProvider;
        [SerializeField] private FuelStationManager fuelStationManager;
        [SerializeField] private SettlementManager settlementManager;

        [Header("Performance")]
        [SerializeField, Range(0.1f, 0.75f)] private float managerUpdateInterval = 0.25f;

        private IPlayerFuelStatus _fuelStatus;
        private float _timeAccumulator;

        private void Awake()
        {
            _fuelStatus = playerFuelProvider as IPlayerFuelStatus;
            if (fuelStationManager == null) fuelStationManager = GetComponentInChildren<FuelStationManager>();
            if (settlementManager == null) settlementManager = GetComponentInChildren<SettlementManager>();
        }

        private void Update()
        {
            if (playerVehicle == null) return;

            _timeAccumulator += Time.deltaTime;
            if (_timeAccumulator < managerUpdateInterval) return;

            float dt = _timeAccumulator;
            _timeAccumulator = 0f;

            EnvironmentRuntimeContext context = BuildContext(dt);
            if (fuelStationManager != null) fuelStationManager.Step(context, _fuelStatus);
            if (settlementManager != null) settlementManager.Step(context);
        }

        private EnvironmentRuntimeContext BuildContext(float dt)
        {
            EnvironmentRuntimeContext context = new EnvironmentRuntimeContext
            {
                PlayerPosition = playerVehicle.position,
                PlayerForward = playerVehicle.forward,
                PlayerFuel01 = _fuelStatus != null ? Mathf.Clamp01(_fuelStatus.GetFuelNormalized()) : 1f,
                IsNight = QueryBool(dayNightProvider, "IsNight", false),
                IsRaining = QueryBool(weatherSystemProvider, "IsRaining", false),
                IsFoggy = QueryBool(weatherSystemProvider, "IsFoggy", false),
                DeltaTime = dt,
            };
            return context;
        }

        private static bool QueryBool(MonoBehaviour provider, string propertyName, bool fallback)
        {
            if (provider == null) return fallback;

            var property = provider.GetType().GetProperty(propertyName);
            if (property != null && property.PropertyType == typeof(bool))
                return (bool)property.GetValue(provider, null);

            var method = provider.GetType().GetMethod(propertyName);
            if (method != null && method.ReturnType == typeof(bool) && method.GetParameters().Length == 0)
                return (bool)method.Invoke(provider, null);

            return fallback;
        }
    }
}
