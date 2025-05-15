#include "PlanetGeneratorExample.h"
#include "PlanetGeneratorBlueprintFunctionLibrary.h"
#include "PlanetMaterialGenerator.h"
#include "SimplexNoiseBPLibrary.h"

APlanetGeneratorExample::APlanetGeneratorExample()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlanetGeneratorExample::BeginPlay()
{
	Super::BeginPlay();

	if (GenerateOnBeginPlay)
	{
		GeneratePlanet();
	}
}

void APlanetGeneratorExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlanetGeneratorExample::GeneratePlanet()
{
	// Destroy existing planet if any
	if (Planet)
	{
		Planet->Destroy();
		Planet = nullptr;
	}

	// Set the noise seed
	USimplexNoiseBPLibrary::SetNoiseSeed(Seed);

	// Create the planet
	Planet = UPlanetGeneratorBlueprintFunctionLibrary::CreatePlanet(this, GetActorLocation(), GetActorRotation(), PlanetRadius, Resolution);

	if (Planet)
	{
		// Set climate parameters
		UPlanetGeneratorBlueprintFunctionLibrary::SetClimateParameters(Planet, EquatorTemperature, PoleTemperature, MoistureScale);

		// Add noise layers
		UPlanetGeneratorBlueprintFunctionLibrary::AddNoiseLayer(Planet, 1.0f, 4, 1.0f, 2.0f, 0.5f);
		UPlanetGeneratorBlueprintFunctionLibrary::AddNoiseLayer(Planet, 0.5f, 6, 2.0f, 2.0f, 0.5f);
		UPlanetGeneratorBlueprintFunctionLibrary::AddNoiseLayer(Planet, 0.25f, 2, 4.0f, 2.0f, 0.5f);

		// Set ocean parameters with the ocean material
		UMaterialInstanceDynamic* OceanMaterial = UPlanetMaterialGenerator::CreateOceanMaterial(
			this,
			FLinearColor(0.0f, 0.3f, 0.6f, 0.7f), // Water color
			0.2f,  // Roughness
			0.1f,  // Metallic
			0.7f,  // Opacity
			true   // Two-sided
		);

		// Set planet material
		UMaterialInstanceDynamic* PlanetMaterial = UPlanetMaterialGenerator::CreatePlanetMaterial(
			this,
			FLinearColor(0.2f, 0.5f, 0.2f, 1.0f), // Base color
			0.8f,  // Roughness
			0.0f,  // Metallic
			true,  // Two-sided
			0.5f,  // Ambient occlusion
			0.0f   // Emissive strength
		);
		Planet->PlanetMaterial = PlanetMaterial;

		// Add biomes with custom materials
		AddDefaultBiomes();

		// This will enable auto-rotation and tile selection for the example planet
		if (Planet)
		{
			// Set auto-rotation
			Planet->AutoRotate = true;
			Planet->RotationSpeed = 2.0f;
			Planet->RotationAxis = FVector(0.0f, 0.0f, 1.0f);

			// Enable tile selection
			Planet->EnableTileSelection = true;
			Planet->SelectedTileColor = FLinearColor(1.0f, 0.3f, 0.3f, 1.0f);
			Planet->SelectedTileHighlightIntensity = 1.5f;
		}

		// Regenerate the planet with all the settings
		UPlanetGeneratorBlueprintFunctionLibrary::RegeneratePlanet(Planet);

		// Make sure the planet has collision enabled
		if (Planet && Planet->PlanetMesh)
		{
			Planet->PlanetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Planet->PlanetMesh->SetCollisionResponseToAllChannels(ECR_Block);
		}
	}
}

void APlanetGeneratorExample::AddDefaultBiomes()
{
	if (!Planet)
	{
		return;
	}

	// Ocean biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::Ocean,
		FLinearColor(0.0f, 0.1f, 0.4f, 1.0f),
		0.0f, 0.3f, 0.0f, 1.0f, 0.0f, 1.0f
	);

	// Beach biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::Beach,
		FLinearColor(0.95f, 0.95f, 0.8f, 1.0f),
		0.3f, 0.35f, 0.0f, 1.0f, 0.0f, 1.0f
	);

	// Desert biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::Desert,
		FLinearColor(0.85f, 0.8f, 0.5f, 1.0f),
		0.35f, 0.6f, 0.6f, 1.0f, 0.0f, 0.3f
	);

	// Plains biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::Plains,
		FLinearColor(0.2f, 0.6f, 0.2f, 1.0f),
		0.35f, 0.6f, 0.3f, 0.7f, 0.3f, 0.6f
	);

	// Forest biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::Forest,
		FLinearColor(0.1f, 0.4f, 0.1f, 1.0f),
		0.4f, 0.7f, 0.3f, 0.7f, 0.6f, 1.0f
	);

	// Mountains biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::Mountains,
		FLinearColor(0.5f, 0.5f, 0.5f, 1.0f),
		0.7f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f
	);

	// Snow capped biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::SnowCapped,
		FLinearColor(0.95f, 0.95f, 0.95f, 1.0f),
		0.9f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
	);

	// Tundra biome
	UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(
		Planet,
		EBiomeType::Tundra,
		FLinearColor(0.8f, 0.8f, 0.9f, 1.0f),
		0.35f, 0.7f, 0.0f, 0.3f, 0.0f, 1.0f
	);
}
