#include "PlanetActor.h"
#include "SimplexNoiseBPLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include <Kismet/GameplayStatics.h>

APlanetActor::APlanetActor()
{
	PrimaryActorTick.bCanEverTick = true;

	PlanetMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PlanetMesh"));
	RootComponent = PlanetMesh;
	PlanetMesh->bUseAsyncCooking = true;

	// Set up collision
	PlanetMesh->SetCollisionProfileName(TEXT("BlockAll"));
	PlanetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlanetMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PlanetMesh->SetGenerateOverlapEvents(true);

	// Add default noise layer
	FNoiseLayer DefaultLayer;
	NoiseLayers.Add(DefaultLayer);

	// Add default biomes
	FBiomeSettings OceanBiome;
	OceanBiome.BiomeType = EBiomeType::Ocean;
	OceanBiome.BiomeColor = FLinearColor(0.0f, 0.1f, 0.4f, 1.0f);
	OceanBiome.MinHeight = 0.0f;
	OceanBiome.MaxHeight = 0.3f;
	Biomes.Add(OceanBiome);

	FBiomeSettings BeachBiome;
	BeachBiome.BiomeType = EBiomeType::Beach;
	BeachBiome.BiomeColor = FLinearColor(0.95f, 0.95f, 0.8f, 1.0f);
	BeachBiome.MinHeight = 0.3f;
	BeachBiome.MaxHeight = 0.35f;
	Biomes.Add(BeachBiome);

	FBiomeSettings PlainsBiome;
	PlainsBiome.BiomeType = EBiomeType::Plains;
	PlainsBiome.BiomeColor = FLinearColor(0.2f, 0.6f, 0.2f, 1.0f);
	PlainsBiome.MinHeight = 0.35f;
	PlainsBiome.MaxHeight = 0.6f;
	PlainsBiome.MinMoisture = 0.3f;
	Biomes.Add(PlainsBiome);

	FBiomeSettings DesertBiome;
	DesertBiome.BiomeType = EBiomeType::Desert;
	DesertBiome.BiomeColor = FLinearColor(0.85f, 0.8f, 0.5f, 1.0f);
	DesertBiome.MinHeight = 0.35f;
	DesertBiome.MaxHeight = 0.6f;
	DesertBiome.MaxMoisture = 0.3f;
	DesertBiome.MinTemperature = 0.6f;
	Biomes.Add(DesertBiome);

	FBiomeSettings ForestBiome;
	ForestBiome.BiomeType = EBiomeType::Forest;
	ForestBiome.BiomeColor = FLinearColor(0.1f, 0.4f, 0.1f, 1.0f);
	ForestBiome.MinHeight = 0.4f;
	ForestBiome.MaxHeight = 0.7f;
	ForestBiome.MinMoisture = 0.5f;
	ForestBiome.MinTemperature = 0.3f;
	ForestBiome.MaxTemperature = 0.7f;
	Biomes.Add(ForestBiome);

	FBiomeSettings MountainBiome;
	MountainBiome.BiomeType = EBiomeType::Mountains;
	MountainBiome.BiomeColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	MountainBiome.MinHeight = 0.7f;
	MountainBiome.MaxHeight = 0.9f;
	Biomes.Add(MountainBiome);

	FBiomeSettings SnowBiome;
	SnowBiome.BiomeType = EBiomeType::SnowCapped;
	SnowBiome.BiomeColor = FLinearColor(0.95f, 0.95f, 0.95f, 1.0f);
	SnowBiome.MinHeight = 0.9f;
	SnowBiome.MaxHeight = 1.0f;
	Biomes.Add(SnowBiome);

	FBiomeSettings TundraBiome;
	TundraBiome.BiomeType = EBiomeType::Tundra;
	TundraBiome.BiomeColor = FLinearColor(0.8f, 0.8f, 0.9f, 1.0f);
	TundraBiome.MinHeight = 0.35f;
	TundraBiome.MaxHeight = 0.7f;
	TundraBiome.MaxTemperature = 0.3f;
	Biomes.Add(TundraBiome);
}

void APlanetActor::BeginPlay()
{
	Super::BeginPlay();

	// Make sure collision is enabled
	if (PlanetMesh)
	{
		PlanetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PlanetMesh->SetCollisionResponseToAllChannels(ECR_Block);
		PlanetMesh->SetGenerateOverlapEvents(true);

		// Log collision settings
		UE_LOG(LogTemp, Log, TEXT("Planet collision enabled: %s"),
			PlanetMesh->IsCollisionEnabled() ? TEXT("Yes") : TEXT("No"));
		UE_LOG(LogTemp, Log, TEXT("Planet collision profile: %s"),
			*PlanetMesh->GetCollisionProfileName().ToString());
	}
}

void APlanetActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (AutoUpdate)
	{
		GeneratePlanet();
	}
}

void APlanetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle auto rotation if enabled
	if (AutoRotate)
	{
		FRotator NewRotation = GetActorRotation();
		float RotationAmount = RotationSpeed * DeltaTime;
		FQuat QuatRotation = FQuat(RotationAxis, FMath::DegreesToRadians(RotationAmount));
		AddActorLocalRotation(QuatRotation);

		// If we have a selected tile, update its visual
		if (SelectedTileIndex >= 0 && SelectedTriangleVertices.Num() == 3)
		{
			// Draw debug lines around the selected triangle in its current position
			FVector V1 = GetActorTransform().TransformPosition(SelectedTriangleVertices[0]);
			FVector V2 = GetActorTransform().TransformPosition(SelectedTriangleVertices[1]);
			FVector V3 = GetActorTransform().TransformPosition(SelectedTriangleVertices[2]);

			DrawDebugLine(GetWorld(), V1, V2, FColor::Yellow, false, 0.0f, 0, 3.0f);
			DrawDebugLine(GetWorld(), V2, V3, FColor::Yellow, false, 0.0f, 0, 3.0f);
			DrawDebugLine(GetWorld(), V3, V1, FColor::Yellow, false, 0.0f, 0, 3.0f);
		}
	}
}

void APlanetActor::GeneratePlanet()
{
	ClearMesh();
	CreateIcosphere();
	SubdivideIcosphere(Resolution);

	// Calculate normals, UVs, and colors
	Normals.SetNum(Vertices.Num());
	UV0.SetNum(Vertices.Num());
	VertexColors.SetNum(Vertices.Num());

	// Calculate final positions with noise and biomes
	TArray<FVector> FinalVertices;
	FinalVertices.SetNum(Vertices.Num());

	for (int32 i = 0; i < Vertices.Num(); i++)
	{
		FVector PointOnUnitSphere = Vertices[i].GetSafeNormal();
		FVector PointOnPlanet = CalculatePointOnPlanet(PointOnUnitSphere);
		FinalVertices[i] = PointOnPlanet;

		// Calculate normal - FIXED: Ensure normals point outward from the center
		// For a planet, normals should point away from the center
		Normals[i] = (PointOnPlanet - GetActorLocation()).GetSafeNormal();

		// Calculate UV (simple spherical mapping)
		float U = 0.5f + FMath::Atan2(PointOnUnitSphere.Y, PointOnUnitSphere.X) / (2.0f * PI);
		float V = 0.5f - FMath::Asin(PointOnUnitSphere.Z) / PI;
		UV0[i] = FVector2D(U, V);

		// Calculate biome color
		float Height = (PointOnPlanet.Size() - PlanetRadius) / (PlanetRadius * 0.2f);
		Height = FMath::Clamp(Height, 0.0f, 1.0f);

		float Temperature = GetTemperature(PointOnUnitSphere);
		float Moisture = GetMoisture(PointOnUnitSphere);

		EBiomeType BiomeType = DetermineBiome(Height, Temperature, Moisture);
		VertexColors[i] = GetBiomeColor(BiomeType, Height, Temperature, Moisture);
	}

	// Create tangents
	Tangents.SetNum(Vertices.Num());
	for (int32 i = 0; i < Vertices.Num(); i++)
	{
		FVector Normal = Normals[i];
		FVector Tangent = FVector::CrossProduct(Normal, FVector::UpVector);
		if (Tangent.SizeSquared() < SMALL_NUMBER)
		{
			Tangent = FVector::CrossProduct(Normal, FVector::ForwardVector);
		}
		Tangent.Normalize();
		Tangents[i] = FProcMeshTangent(Tangent, false);
	}

	// Create the procedural mesh with correct winding order
	PlanetMesh->CreateMeshSection_LinearColor(0, FinalVertices, Triangles, Normals, UV0, VertexColors, Tangents, true);

	// Store the triangles for later use
	StoredTriangles = Triangles;

	// Apply material
	if (PlanetMaterial)
	{
		PlanetMesh->SetMaterial(0, PlanetMaterial);
	}

	// Debug: Show normals
	if (ShowNormals)
	{
		for (int32 i = 0; i < FinalVertices.Num(); i++)
		{
			DrawDebugLine(GetWorld(), FinalVertices[i], FinalVertices[i] + Normals[i] * NormalLength, FColor::Red, true, -1.0f, 0, 1.0f);
		}
	}

	// Store the final vertices for later use
	CachedVertices = FinalVertices;

	// Make sure collision is enabled
	PlanetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlanetMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PlanetMesh->SetGenerateOverlapEvents(true);

	// Add this after the collision setup
	PlanetMesh->bUseComplexAsSimpleCollision = true;

	// Log collision settings
	UE_LOG(LogTemp, Log, TEXT("Planet generated with %d vertices and %d triangles"), Vertices.Num(), Triangles.Num() / 3);
	UE_LOG(LogTemp, Log, TEXT("Planet collision enabled: %s"),
		PlanetMesh->IsCollisionEnabled() ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogTemp, Log, TEXT("Planet collision profile: %s"),
		*PlanetMesh->GetCollisionProfileName().ToString());
}

void APlanetActor::ClearMesh()
{
	// Store triangles before clearing
	if (Triangles.Num() > 0)
	{
		StoredTriangles = Triangles;
	}

	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UV0.Empty();
	VertexColors.Empty();
	Tangents.Empty();
	CachedVertices.Empty();

	PlanetMesh->ClearAllMeshSections();
}

void APlanetActor::CreateIcosphere()
{
	// Create an icosahedron (20-sided polyhedron)
	const float t = (1.0f + FMath::Sqrt(5.0f)) / 2.0f;

	// Add vertices
	Vertices.Add(FVector(-1, t, 0).GetSafeNormal());
	Vertices.Add(FVector(1, t, 0).GetSafeNormal());
	Vertices.Add(FVector(-1, -t, 0).GetSafeNormal());
	Vertices.Add(FVector(1, -t, 0).GetSafeNormal());

	Vertices.Add(FVector(0, -1, t).GetSafeNormal());
	Vertices.Add(FVector(0, 1, t).GetSafeNormal());
	Vertices.Add(FVector(0, -1, -t).GetSafeNormal());
	Vertices.Add(FVector(0, 1, -t).GetSafeNormal());

	Vertices.Add(FVector(t, 0, -1).GetSafeNormal());
	Vertices.Add(FVector(t, 0, 1).GetSafeNormal());
	Vertices.Add(FVector(-t, 0, -1).GetSafeNormal());
	Vertices.Add(FVector(-t, 0, 1).GetSafeNormal());

	// FIXED: Ensure consistent winding order for all triangles (clockwise)
	// 5 faces around point 0
	Triangles.Add(0); Triangles.Add(5); Triangles.Add(11);
	Triangles.Add(0); Triangles.Add(1); Triangles.Add(5);
	Triangles.Add(0); Triangles.Add(7); Triangles.Add(1);
	Triangles.Add(0); Triangles.Add(10); Triangles.Add(7);
	Triangles.Add(0); Triangles.Add(11); Triangles.Add(10);

	// 5 adjacent faces
	Triangles.Add(1); Triangles.Add(9); Triangles.Add(5);
	Triangles.Add(5); Triangles.Add(4); Triangles.Add(11);
	Triangles.Add(11); Triangles.Add(2); Triangles.Add(10);
	Triangles.Add(10); Triangles.Add(6); Triangles.Add(7);
	Triangles.Add(7); Triangles.Add(8); Triangles.Add(1);

	// 5 faces around point 3
	Triangles.Add(3); Triangles.Add(4); Triangles.Add(9);
	Triangles.Add(3); Triangles.Add(2); Triangles.Add(4);
	Triangles.Add(3); Triangles.Add(6); Triangles.Add(2);
	Triangles.Add(3); Triangles.Add(8); Triangles.Add(6);
	Triangles.Add(3); Triangles.Add(9); Triangles.Add(8);

	// 5 adjacent faces
	Triangles.Add(4); Triangles.Add(5); Triangles.Add(9);
	Triangles.Add(2); Triangles.Add(11); Triangles.Add(4);
	Triangles.Add(6); Triangles.Add(10); Triangles.Add(2);
	Triangles.Add(8); Triangles.Add(7); Triangles.Add(6);
	Triangles.Add(9); Triangles.Add(1); Triangles.Add(8);

	UE_LOG(LogTemp, Log, TEXT("Icosphere created with %d vertices and %d triangles"), Vertices.Num(), Triangles.Num() / 3);
}

void APlanetActor::SubdivideIcosphere(int32 Subdivisions)
{
	if (Subdivisions <= 0)
	{
		return;
	}

	TMap<FVector2D, int32> MiddlePointIndexCache;

	for (int32 i = 0; i < Subdivisions; i++)
	{
		TArray<int32> NewTriangles;

		// Subdivide each triangle into 4 triangles
		for (int32 j = 0; j < Triangles.Num(); j += 3)
		{
			int32 v1 = Triangles[j];
			int32 v2 = Triangles[j + 1];
			int32 v3 = Triangles[j + 2];

			// Get or create mid points
			int32 a = GetMiddlePoint(v1, v2, MiddlePointIndexCache);
			int32 b = GetMiddlePoint(v2, v3, MiddlePointIndexCache);
			int32 c = GetMiddlePoint(v3, v1, MiddlePointIndexCache);

			// Create 4 new triangles
			NewTriangles.Add(v1); NewTriangles.Add(a); NewTriangles.Add(c);
			NewTriangles.Add(v2); NewTriangles.Add(b); NewTriangles.Add(a);
			NewTriangles.Add(v3); NewTriangles.Add(c); NewTriangles.Add(b);
			NewTriangles.Add(a); NewTriangles.Add(b); NewTriangles.Add(c);
		}

		Triangles = NewTriangles;
	}

	UE_LOG(LogTemp, Log, TEXT("Subdivided icosphere to %d vertices and %d triangles"), Vertices.Num(), Triangles.Num() / 3);
}

int32 APlanetActor::GetMiddlePoint(int32 p1, int32 p2, TMap<FVector2D, int32>& Cache)
{
	// First check if we already have it
	bool FirstIsSmaller = p1 < p2;
	int64 SmallerIndex = FirstIsSmaller ? p1 : p2;
	int64 GreaterIndex = FirstIsSmaller ? p2 : p1;
	FVector2D Key(SmallerIndex, GreaterIndex);

	int32* CachedIndex = Cache.Find(Key);
	if (CachedIndex)
	{
		return *CachedIndex;
	}

	// Not in cache, calculate it
	FVector Point1 = Vertices[p1];
	FVector Point2 = Vertices[p2];
	FVector Middle = (Point1 + Point2) * 0.5f;

	// Add vertex makes sure point is on unit sphere
	int32 Index = Vertices.Add(Middle.GetSafeNormal());

	// Add to cache
	Cache.Add(Key, Index);

	return Index;
}

float APlanetActor::EvaluateNoise(const FVector& PointOnUnitSphere)
{
	float FirstLayerValue = 0;
	float Elevation = 0;
	float Weight = 1;

	for (int32 i = 0; i < NoiseLayers.Num(); i++)
	{
		const FNoiseLayer& NoiseLayer = NoiseLayers[i];
		if (!NoiseLayer.Enabled)
		{
			continue;
		}

		float Amplitude = 1;
		float Frequency = NoiseLayer.BaseRoughness;
		float NoiseValue = 0;

		for (int32 j = 0; j < NoiseLayer.NumLayers; j++)
		{
			FVector SamplePoint = PointOnUnitSphere * Frequency + NoiseLayer.Center;

			// Use SimplexNoise from the SimplexNoiseBPLibrary
			float Noise = USimplexNoiseBPLibrary::SimplexNoise3D(SamplePoint.X, SamplePoint.Y, SamplePoint.Z);
			NoiseValue += (Noise + 1) * 0.5f * Amplitude;

			Frequency *= NoiseLayer.Roughness;
			Amplitude *= NoiseLayer.Persistence;
		}

		if (NoiseLayer.MinValue > 0)
		{
			NoiseValue = FMath::Max(0.0f, NoiseValue - NoiseLayer.MinValue);
		}

		NoiseValue *= NoiseLayer.Strength;

		if (i == 0)
		{
			FirstLayerValue = NoiseValue;
		}
		else
		{
			float Mask = FirstLayerValue;
			NoiseValue *= Mask;
		}

		Elevation += NoiseValue * Weight;
		Weight *= 0.5f;
	}

	return Elevation;
}

FVector APlanetActor::CalculatePointOnPlanet(const FVector& PointOnUnitSphere)
{
	float Elevation = EvaluateNoise(PointOnUnitSphere);
	float FinalElevation = PlanetRadius * (1 + Elevation * 0.2f);
	return PointOnUnitSphere * FinalElevation;
}

float APlanetActor::GetTemperature(const FVector& PointOnUnitSphere)
{
	// Temperature decreases from equator to poles
	float LatitudeFactor = 1.0f - FMath::Abs(PointOnUnitSphere.Z);
	float Temperature = FMath::Lerp(PoleTemperature, EquatorTemperature, LatitudeFactor);

	// Add some noise for more natural temperature distribution
	FVector SamplePoint = PointOnUnitSphere * 3.7f;
	float TemperatureNoise = USimplexNoiseBPLibrary::SimplexNoise3D(SamplePoint.X, SamplePoint.Y, SamplePoint.Z) * 0.1f;

	return FMath::Clamp(Temperature + TemperatureNoise, 0.0f, 1.0f);
}

float APlanetActor::GetMoisture(const FVector& PointOnUnitSphere)
{
	// Base moisture with noise
	FVector SamplePoint = PointOnUnitSphere * 5.3f * MoistureScale;
	float Moisture = (USimplexNoiseBPLibrary::SimplexNoise3D(SamplePoint.X, SamplePoint.Y, SamplePoint.Z) + 1.0f) * 0.5f;

	// Moisture tends to be higher near the equator and lower near the poles
	float LatitudeFactor = 1.0f - FMath::Abs(PointOnUnitSphere.Z);
	Moisture *= FMath::Lerp(0.7f, 1.0f, LatitudeFactor);

	return FMath::Clamp(Moisture, 0.0f, 1.0f);
}

EBiomeType APlanetActor::DetermineBiome(float Height, float Temperature, float Moisture)
{
	// Then check all other biomes
	for (const FBiomeSettings& Biome : Biomes)
	{
		if (Height >= Biome.MinHeight && Height <= Biome.MaxHeight &&
			Temperature >= Biome.MinTemperature && Temperature <= Biome.MaxTemperature &&
			Moisture >= Biome.MinMoisture && Moisture <= Biome.MaxMoisture)
		{
			return Biome.BiomeType;
		}
	}

	// Default to plains if no match
	return EBiomeType::Plains;
}

FLinearColor APlanetActor::GetBiomeColor(EBiomeType BiomeType, float Height, float Temperature, float Moisture)
{
	for (const FBiomeSettings& Biome : Biomes)
	{
		if (Biome.BiomeType == BiomeType)
		{
			return Biome.BiomeColor;
		}
	}

	// Default color if biome not found
	return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

bool APlanetActor::SelectTileAtScreenPosition(APlayerController* PlayerController, FVector2D ScreenPosition)
{
	if (!EnableTileSelection)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tile selection is disabled. EnableTileSelection = false"));
		return false;
	}

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectTileAtScreenPosition: PlayerController is null"));
		return false;
	}

	// Check if we have triangles data
	if (Triangles.Num() == 0)
	{
		// Try to use stored triangles if available
		if (StoredTriangles.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Using stored triangles as fallback"));
			Triangles = StoredTriangles;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SelectTileAtScreenPosition: Triangles array is empty. Planet may not be properly generated."));
			return false;
		}
	}

	// First clear any existing selection
	ClearSelectedTile();

	// Get the player's view
	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// Log the player's view for debugging
	UE_LOG(LogTemp, Log, TEXT("Player view location: %s, rotation: %s"),
		*ViewLocation.ToString(), *ViewRotation.ToString());

	// Try multiple trace channels to ensure we hit something
	TArray<ECollisionChannel> ChannelsToTry = {
		ECC_Visibility,
		ECC_WorldStatic,
		ECC_WorldDynamic,
		ECC_Pawn,
		ECC_PhysicsBody,
		ECC_GameTraceChannel1
	};

	FHitResult HitResult;
	bool bHitDetected = false;

	// Try each channel until we get a hit
	for (ECollisionChannel Channel : ChannelsToTry)
	{
		// Perform trace to find what's under the cursor
		bool bTraceHit = PlayerController->GetHitResultAtScreenPosition(
			ScreenPosition,
			Channel,
			true, // Trace complex to ensure we hit the mesh
			HitResult
		);

		if (bTraceHit)
		{
			bHitDetected = true;
			UE_LOG(LogTemp, Log, TEXT("Hit detected on channel %d"), (int32)Channel);
			break;
		}
	}

	// If we still don't have a hit, try a direct line trace
	if (!bHitDetected)
	{
		UE_LOG(LogTemp, Warning, TEXT("No hit detected with standard methods, trying direct line trace"));

		// Convert screen position to world space
		FVector WorldDirection;
		if (UGameplayStatics::DeprojectScreenToWorld(
			PlayerController,
			ScreenPosition,
			/*out*/ ViewLocation,
			/*out*/ WorldDirection))
		{
			// Draw debug line to show the trace
			DrawDebugLine(
				GetWorld(),
				ViewLocation,
				ViewLocation + WorldDirection * 10000.0f,
				FColor::Red,
				false,
				5.0f,
				0,
				2.0f
			);

			// Perform line trace
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(PlayerController->GetPawn());

			bHitDetected = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				ViewLocation,
				ViewLocation + WorldDirection * 10000.0f,
				ECC_Visibility,
				QueryParams
			);
		}
	}

	if (!bHitDetected)
	{
		UE_LOG(LogTemp, Warning, TEXT("No hit detected at screen position (%f, %f)"), ScreenPosition.X, ScreenPosition.Y);

		// Draw a debug sphere at the planet's location to help visualize
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			PlanetRadius,
			32,
			FColor::Green,
			false,
			5.0f,
			0,
			2.0f
		);

		return false;
	}

	// Check if we hit this planet
	if (HitResult.GetActor() != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit detected but not on this planet. Hit Actor: %s"),
			HitResult.GetActor() ? *HitResult.GetActor()->GetName() : TEXT("None"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Hit detected on planet at location: %s"), *HitResult.Location.ToString());

	// Find the triangle index from the hit location
	SelectedTileIndex = FindTriangleIndexFromHitLocation(HitResult.Location);

	if (SelectedTileIndex < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find triangle at hit location"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Selected triangle index: %d"), SelectedTileIndex);

	// Store the hit location
	SelectedTileLocation = HitResult.Location;

	// Determine the biome at this location
	FVector PointOnUnitSphere = (HitResult.Location - GetActorLocation()).GetSafeNormal();
	float Height = (HitResult.Location - GetActorLocation()).Size() / PlanetRadius - 1.0f;
	Height = FMath::Clamp(Height * 5.0f, 0.0f, 1.0f); // Scale height to 0-1 range
	float Temperature = GetTemperature(PointOnUnitSphere);
	float Moisture = GetMoisture(PointOnUnitSphere);
	SelectedTileBiome = DetermineBiome(Height, Temperature, Moisture);

	UE_LOG(LogTemp, Log, TEXT("Selected tile biome: %s"), *UEnum::GetValueAsString(SelectedTileBiome));

	// Update the visual representation
	bool bVisualUpdated = UpdateSelectedTileVisual();
	if (!bVisualUpdated)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to update visual for selected tile"));
	}

	// Trigger the blueprint event
	OnTileSelected(SelectedTileIndex, SelectedTileLocation, SelectedTileBiome);

	return true;
}

void APlanetActor::ClearSelectedTile()
{
	if (SelectedTileIndex >= 0)
	{
		SelectedTileIndex = -1;
		SelectedTriangleVertices.Empty();

		// Restore original colors if we have them
		if (OriginalVertexColors.Num() > 0 && VertexColors.Num() == OriginalVertexColors.Num())
		{
			// Restore the original colors
			VertexColors = OriginalVertexColors;

			// Update the mesh section with the original colors
			FProcMeshSection* MeshSection = PlanetMesh->GetProcMeshSection(0);
			if (MeshSection && MeshSection->ProcVertexBuffer.Num() > 0)
			{
				// Get the vertices from the mesh section
				TArray<FVector> Positions;
				TArray<FVector> MeshNormals;
				TArray<FVector2D> MeshUVs;
				TArray<FProcMeshTangent> MeshTangents;

				Positions.SetNum(MeshSection->ProcVertexBuffer.Num());
				MeshNormals.SetNum(MeshSection->ProcVertexBuffer.Num());
				MeshUVs.SetNum(MeshSection->ProcVertexBuffer.Num());
				MeshTangents.SetNum(MeshSection->ProcVertexBuffer.Num());

				for (int32 i = 0; i < MeshSection->ProcVertexBuffer.Num(); i++)
				{
					Positions[i] = MeshSection->ProcVertexBuffer[i].Position;
					MeshNormals[i] = MeshSection->ProcVertexBuffer[i].Normal;
					MeshUVs[i] = MeshSection->ProcVertexBuffer[i].UV0;
					MeshTangents[i] = MeshSection->ProcVertexBuffer[i].Tangent;
				}

				// Create a new mesh section with the updated colors
				PlanetMesh->ClearMeshSection(0);
				PlanetMesh->CreateMeshSection_LinearColor(0, Positions, Triangles, MeshNormals, MeshUVs, VertexColors, MeshTangents, true);

				// Reapply the material
				if (PlanetMaterial)
				{
					PlanetMesh->SetMaterial(0, PlanetMaterial);
				}
			}

			// Clear the original colors cache
			OriginalVertexColors.Empty();
		}
	}

	// Make sure we have triangles data
	if (Triangles.Num() == 0 && StoredTriangles.Num() > 0)
	{
		Triangles = StoredTriangles;
	}
}

bool APlanetActor::UpdateSelectedTileVisual()
{
	// Make sure we have triangles data
	if (Triangles.Num() == 0 && StoredTriangles.Num() > 0)
	{
		Triangles = StoredTriangles;
	}

	if (SelectedTileIndex < 0 || SelectedTileIndex >= Triangles.Num() / 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSelectedTileVisual: Invalid selected tile index: %d"), SelectedTileIndex);
		return false;
	}

	// Store original colors if we haven't already
	if (OriginalVertexColors.Num() == 0)
	{
		OriginalVertexColors = VertexColors;
		UE_LOG(LogTemp, Log, TEXT("Stored original vertex colors: %d"), OriginalVertexColors.Num());
	}

	// Get the indices of the vertices for this triangle
	int32 Index1 = Triangles[SelectedTileIndex * 3];
	int32 Index2 = Triangles[SelectedTileIndex * 3 + 1];
	int32 Index3 = Triangles[SelectedTileIndex * 3 + 2];

	UE_LOG(LogTemp, Log, TEXT("Selected triangle vertices: %d, %d, %d"), Index1, Index2, Index3);

	// Highlight these vertices
	if (Index1 < VertexColors.Num() && Index2 < VertexColors.Num() && Index3 < VertexColors.Num())
	{
		// Make a copy of the current colors
		TArray<FLinearColor> NewColors = VertexColors;

		// Use a very bright, distinct color for highlighting
		FLinearColor HighlightColor = SelectedTileColor * SelectedTileHighlightIntensity;
		HighlightColor.A = 1.0f; // Ensure full opacity

		UE_LOG(LogTemp, Log, TEXT("Highlight color: R=%f, G=%f, B=%f"),
			HighlightColor.R, HighlightColor.G, HighlightColor.B);

		// Modify the colors for the selected triangle
		NewColors[Index1] = HighlightColor;
		NewColors[Index2] = HighlightColor;
		NewColors[Index3] = HighlightColor;

		// Update the vertex colors
		VertexColors = NewColors;

		// Get the mesh section
		FProcMeshSection* MeshSection = PlanetMesh->GetProcMeshSection(0);
		if (MeshSection && MeshSection->ProcVertexBuffer.Num() > 0)
		{
			// Get the vertices from the mesh section
			TArray<FVector> Positions;
			TArray<FVector> MeshNormals;
			TArray<FVector2D> MeshUVs;
			TArray<FProcMeshTangent> MeshTangents;

			Positions.SetNum(MeshSection->ProcVertexBuffer.Num());
			MeshNormals.SetNum(MeshSection->ProcVertexBuffer.Num());
			MeshUVs.SetNum(MeshSection->ProcVertexBuffer.Num());
			MeshTangents.SetNum(MeshSection->ProcVertexBuffer.Num());

			for (int32 i = 0; i < MeshSection->ProcVertexBuffer.Num(); i++)
			{
				Positions[i] = MeshSection->ProcVertexBuffer[i].Position;
				MeshNormals[i] = MeshSection->ProcVertexBuffer[i].Normal;
				MeshUVs[i] = MeshSection->ProcVertexBuffer[i].UV0;
				MeshTangents[i] = MeshSection->ProcVertexBuffer[i].Tangent;
			}

			// Create a new mesh section with the updated colors
			PlanetMesh->ClearMeshSection(0);
			PlanetMesh->CreateMeshSection_LinearColor(0, Positions, Triangles, MeshNormals, MeshUVs, VertexColors, MeshTangents, true);

			// Reapply the material
			if (PlanetMaterial)
			{
				PlanetMesh->SetMaterial(0, PlanetMaterial);
			}

			// Make sure the material uses vertex colors
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(PlanetMesh->GetMaterial(0));
			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(FName("UseVertexColors"), 1.0f);
			}

			UE_LOG(LogTemp, Log, TEXT("Updated mesh section with new vertex colors"));

			// Store the selected triangle world positions for later use
			FVector V1 = GetActorTransform().TransformPosition(Positions[Index1]);
			FVector V2 = GetActorTransform().TransformPosition(Positions[Index2]);
			FVector V3 = GetActorTransform().TransformPosition(Positions[Index3]);

			SelectedTriangleVertices.Empty();
			SelectedTriangleVertices.Add(Positions[Index1]);
			SelectedTriangleVertices.Add(Positions[Index2]);
			SelectedTriangleVertices.Add(Positions[Index3]);

			// Draw debug lines around the selected triangle to make it more visible
			DrawDebugLine(GetWorld(), V1, V2, FColor::Yellow, false, 0.0f, 0, 3.0f);
			DrawDebugLine(GetWorld(), V2, V3, FColor::Yellow, false, 0.0f, 0, 3.0f);
			DrawDebugLine(GetWorld(), V3, V1, FColor::Yellow, false, 0.0f, 0, 3.0f);

			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UpdateSelectedTileVisual: Invalid mesh section or empty vertex buffer"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateSelectedTileVisual: Vertex indices out of range"));
	}

	return false;
}

int32 APlanetActor::FindTriangleIndexFromHitLocation(const FVector& HitLocation)
{
	// First check if we have triangles data
	if (Triangles.Num() == 0)
	{
		// Try to use stored triangles if available
		if (StoredTriangles.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Using stored triangles as fallback"));
			Triangles = StoredTriangles;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FindTriangleIndexFromHitLocation: Triangles array is empty"));
			return -1;
		}
	}

	// Check if we have cached vertices
	if (CachedVertices.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("FindTriangleIndexFromHitLocation: CachedVertices array is empty"));

		// Try to get vertices from the mesh section as a fallback
		FProcMeshSection* MeshSection = PlanetMesh->GetProcMeshSection(0);
		if (MeshSection && MeshSection->ProcVertexBuffer.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Using mesh section vertices as fallback"));
			CachedVertices.SetNum(MeshSection->ProcVertexBuffer.Num());
			for (int32 i = 0; i < MeshSection->ProcVertexBuffer.Num(); i++)
			{
				CachedVertices[i] = MeshSection->ProcVertexBuffer[i].Position;
			}
		}
		else
		{
			return -1;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("FindTriangleIndexFromHitLocation: Vertex count: %d, Triangle count: %d"),
		CachedVertices.Num(), Triangles.Num() / 3);

	// Find the closest triangle
	float ClosestDistanceSq = MAX_FLT;
	int32 ClosestTriangleIndex = -1;

	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		int32 Index1 = Triangles[i];
		int32 Index2 = Triangles[i + 1];
		int32 Index3 = Triangles[i + 2];

		if (Index1 < CachedVertices.Num() && Index2 < CachedVertices.Num() && Index3 < CachedVertices.Num())
		{
			FVector V1 = GetActorTransform().TransformPosition(CachedVertices[Index1]);
			FVector V2 = GetActorTransform().TransformPosition(CachedVertices[Index2]);
			FVector V3 = GetActorTransform().TransformPosition(CachedVertices[Index3]);

			// Calculate the center of the triangle
			FVector Center = (V1 + V2 + V3) / 3.0f;

			// Check distance to the center
			float DistSq = FVector::DistSquared(HitLocation, Center);
			if (DistSq < ClosestDistanceSq)
			{
				ClosestDistanceSq = DistSq;
				ClosestTriangleIndex = i / 3;
			}
		}
	}

	if (ClosestTriangleIndex >= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("FindTriangleIndexFromHitLocation: Found closest triangle at index %d, distance: %f"),
			ClosestTriangleIndex, FMath::Sqrt(ClosestDistanceSq));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTriangleIndexFromHitLocation: No valid triangle found"));
	}

	return ClosestTriangleIndex;
}