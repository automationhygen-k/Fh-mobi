using UnityEngine;

namespace ProceduralWorldLife
{
    /// <summary>
    /// Runtime behavior for a spawned fuel station.
    /// Handles visuals and player refuel interaction.
    /// </summary>
    public sealed class FuelStation : MonoBehaviour
    {
        [Header("Structure")]
        [SerializeField] private Transform[] pumpPoints;
        [SerializeField] private GameObject[] pedestrianSpawnPrefabs;
        [SerializeField] private GameObject[] parkedCarPrefabs;
        [SerializeField] private Transform pedestrianRoot;
        [SerializeField] private Transform parkedCarRoot;

        [Header("Interaction")]
        [SerializeField] private float interactionRadius = 4f;
        [SerializeField] private float minRefuelSeconds = 5f;
        [SerializeField] private float maxRefuelSeconds = 8f;
        [SerializeField] private string holdPromptText = "Hold to Refuel";

        [Header("Environment Reaction")]
        [SerializeField] private Light[] stationLights;
        [SerializeField] private Renderer[] reflectiveGroundRenderers;

        private float _refuelDuration;
        private float _refuelProgress;
        private bool _isRefueling;

        public void InitializeRuntime(int pedestriansToSpawn, int parkedCarsToSpawn)
        {
            _refuelDuration = Random.Range(minRefuelSeconds, maxRefuelSeconds);
            SpawnDecor(pedestriansToSpawn, parkedCarsToSpawn);
        }

        public bool IsNearPump(Vector3 position)
        {
            if (pumpPoints == null || pumpPoints.Length == 0)
                return Vector3.Distance(transform.position, position) <= interactionRadius;

            for (int i = 0; i < pumpPoints.Length; i++)
            {
                if (pumpPoints[i] != null && Vector3.Distance(pumpPoints[i].position, position) <= interactionRadius)
                    return true;
            }
            return false;
        }

        public void TryRefuel(IPlayerFuelStatus fuelStatus, bool holdInput)
        {
            if (fuelStatus == null) return;

            // Placeholder UI hook (replace with real UI system binding).
            if (holdInput)
            {
                _isRefueling = true;
                _refuelProgress += Time.deltaTime / Mathf.Max(0.1f, _refuelDuration);
                fuelStatus.AddFuelNormalized(Time.deltaTime / Mathf.Max(0.1f, _refuelDuration));
            }
            else
            {
                _isRefueling = false;
                _refuelProgress = Mathf.Max(0f, _refuelProgress - Time.deltaTime * 0.25f);
            }

            if (_refuelProgress >= 1f)
            {
                _isRefueling = false;
                _refuelProgress = 0f;
            }
        }

        public string GetRefuelPrompt() => holdPromptText;

        public void ApplyEnvironmentState(bool isNight, bool isRaining, bool isFoggy)
        {
            if (stationLights != null)
            {
                for (int i = 0; i < stationLights.Length; i++)
                {
                    if (stationLights[i] == null) continue;
                    stationLights[i].enabled = isNight || isFoggy;
                    stationLights[i].intensity = isFoggy ? 1.25f : 1f;
                }
            }

            if (reflectiveGroundRenderers != null)
            {
                for (int i = 0; i < reflectiveGroundRenderers.Length; i++)
                {
                    Renderer r = reflectiveGroundRenderers[i];
                    if (r == null || r.sharedMaterial == null) continue;
                    if (r.sharedMaterial.HasProperty("_Wetness"))
                        r.sharedMaterial.SetFloat("_Wetness", isRaining ? 1f : 0f);
                }
            }
        }

        private void SpawnDecor(int pedestriansToSpawn, int parkedCarsToSpawn)
        {
            if (pedestrianSpawnPrefabs != null && pedestrianSpawnPrefabs.Length > 0 && pedestrianRoot != null)
            {
                for (int i = 0; i < pedestriansToSpawn; i++)
                {
                    GameObject prefab = pedestrianSpawnPrefabs[Random.Range(0, pedestrianSpawnPrefabs.Length)];
                    if (prefab == null) continue;
                    Vector3 jitter = new Vector3(Random.Range(-8f, 8f), 0f, Random.Range(-8f, 8f));
                    Instantiate(prefab, pedestrianRoot.position + jitter, Quaternion.identity, pedestrianRoot);
                }
            }

            if (parkedCarPrefabs != null && parkedCarPrefabs.Length > 0 && parkedCarRoot != null)
            {
                for (int i = 0; i < parkedCarsToSpawn; i++)
                {
                    GameObject prefab = parkedCarPrefabs[Random.Range(0, parkedCarPrefabs.Length)];
                    if (prefab == null) continue;
                    Vector3 jitter = new Vector3(Random.Range(-10f, 10f), 0f, Random.Range(-6f, 6f));
                    Instantiate(prefab, parkedCarRoot.position + jitter, parkedCarRoot.rotation, parkedCarRoot);
                }
            }
        }
    }
}
