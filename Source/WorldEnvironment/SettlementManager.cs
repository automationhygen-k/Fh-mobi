using System.Collections.Generic;
using UnityEngine;

namespace ProceduralWorldLife
{
    /// <summary>
    /// Manages procedural roadside settlements.
    /// Integrates with weather/day-night by toggling settlement visuals and pedestrian cadence.
    /// </summary>
    public sealed class SettlementManager : MonoBehaviour
    {
        private sealed class ActiveSettlement
        {
            public GameObject Root;
            public Light[] StreetLights;
            public Renderer[] ReflectiveRenderers;
            public Animator[] PedestrianAnimators;
        }

        [SerializeField] private SettlementSpawner spawner;
        [SerializeField] private MonoBehaviour roadProviderBehaviour;

        [Header("Spawn Rules")]
        [SerializeField] private float spawnMinDistance = 500f;
        [SerializeField] private float spawnMaxDistance = 1200f;
        [SerializeField] private float despawnDistance = 2400f;
        [SerializeField, Range(1, 3)] private int maxActiveSettlements = 3;
        [SerializeField] private float maxSlopeDegrees = 18f;

        [Header("Traffic Interaction")]
        [SerializeField, Range(0.4f, 1f)] private float trafficSpeedMultiplier = 0.8f;

        private IRoadAnchorProvider _roadProvider;
        private readonly List<ActiveSettlement> _active = new List<ActiveSettlement>();

        private void Awake()
        {
            _roadProvider = roadProviderBehaviour as IRoadAnchorProvider;
        }

        public void Step(EnvironmentRuntimeContext context)
        {
            if (_roadProvider == null || spawner == null) return;

            DespawnFar(context.PlayerPosition);
            if (_active.Count < maxActiveSettlements)
                TrySpawn(context);

            ApplyEnvironment(context);
        }

        public float GetTrafficSpeedMultiplier(Vector3 worldPosition)
        {
            for (int i = 0; i < _active.Count; i++)
            {
                if (_active[i].Root == null) continue;
                if ((_active[i].Root.transform.position - worldPosition).sqrMagnitude < 100f * 100f)
                    return trafficSpeedMultiplier;
            }
            return 1f;
        }

        private void TrySpawn(EnvironmentRuntimeContext context)
        {
            if (!_roadProvider.TrySampleRoadAhead(context.PlayerPosition, context.PlayerForward, spawnMinDistance, spawnMaxDistance, out RoadAnchor anchor))
                return;
            if (anchor.SlopeDegrees > maxSlopeDegrees)
                return;

            SettlementType type = (SettlementType)Random.Range(0, 5);
            GameObject root = spawner.Spawn(type, anchor, transform);
            if (root == null) return;

            ActiveSettlement settlement = new ActiveSettlement
            {
                Root = root,
                StreetLights = root.GetComponentsInChildren<Light>(true),
                ReflectiveRenderers = root.GetComponentsInChildren<Renderer>(true),
                PedestrianAnimators = root.GetComponentsInChildren<Animator>(true),
            };
            _active.Add(settlement);
        }

        private void DespawnFar(Vector3 playerPos)
        {
            for (int i = _active.Count - 1; i >= 0; i--)
            {
                ActiveSettlement s = _active[i];
                if (s.Root == null)
                {
                    _active.RemoveAt(i);
                    continue;
                }

                if ((s.Root.transform.position - playerPos).sqrMagnitude > despawnDistance * despawnDistance)
                {
                    Destroy(s.Root);
                    _active.RemoveAt(i);
                }
            }
        }

        private void ApplyEnvironment(EnvironmentRuntimeContext context)
        {
            float pedestrianAnimScale = context.IsRaining ? 0.75f : 1f;

            for (int i = 0; i < _active.Count; i++)
            {
                ActiveSettlement s = _active[i];
                if (s.Root == null) continue;

                if (s.StreetLights != null)
                {
                    for (int l = 0; l < s.StreetLights.Length; l++)
                    {
                        if (s.StreetLights[l] == null) continue;
                        s.StreetLights[l].enabled = context.IsNight;
                    }
                }

                if (s.ReflectiveRenderers != null)
                {
                    for (int r = 0; r < s.ReflectiveRenderers.Length; r++)
                    {
                        Renderer renderer = s.ReflectiveRenderers[r];
                        if (renderer == null || renderer.sharedMaterial == null) continue;
                        if (renderer.sharedMaterial.HasProperty("_Wetness"))
                            renderer.sharedMaterial.SetFloat("_Wetness", context.IsRaining ? 1f : 0f);
                    }
                }

                if (s.PedestrianAnimators != null)
                {
                    for (int p = 0; p < s.PedestrianAnimators.Length; p++)
                    {
                        if (s.PedestrianAnimators[p] == null) continue;
                        s.PedestrianAnimators[p].speed = pedestrianAnimScale;
                    }
                }

                // In fog, reduce visibility slightly for performance and atmosphere.
                if (context.IsFoggy)
                {
                    float dist = Vector3.Distance(context.PlayerPosition, s.Root.transform.position);
                    s.Root.SetActive(dist < 900f);
                }
                else if (!s.Root.activeSelf)
                {
                    s.Root.SetActive(true);
                }
            }
        }
    }
}
