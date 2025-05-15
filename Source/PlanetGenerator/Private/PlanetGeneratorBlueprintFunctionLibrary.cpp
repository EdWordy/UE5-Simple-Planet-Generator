#include "PlanetGeneratorBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

APlanetActor* UPlanetGeneratorBlueprintFunctionLibrary::CreatePlanet(UObject* WorldContextObject, FVector Location, FRotator Rotation, float Radius, int32 Resolution)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	APlanetActor* Planet = World->SpawnActor<APlanetActor>(APlanetActor::StaticClass(), Location, Rotation, SpawnParams);
	if (Planet)
	{
		Planet->PlanetRadius = Radius;
		Planet->Resolution = Resolution;
		Planet->GeneratePlanet();
	}
	
	return Planet;
}

void UPlanetGeneratorBlueprintFunctionLibrary::AddNoiseLayer(APlanetActor* Planet, float Strength, int32 NumLayers, float BaseRoughness, float Roughness, float Persistence)
{
	if (!Planet)
	{
		return;
	}
	
	FNoiseLayer NoiseLayer;
	NoiseLayer.Enabled = true;
	NoiseLayer.Strength = Strength;
	NoiseLayer.NumLayers = NumLayers;
	NoiseLayer.BaseRoughness = BaseRoughness;
	NoiseLayer.Roughness = Roughness;
	NoiseLayer.Persistence = Persistence;
	NoiseLayer.Center = FVector(FMath::FRand() * 2.0f - 1.0f, FMath::FRand() * 2.0f - 1.0f, FMath::FRand() * 2.0f - 1.0f) * 100.0f;
	
	Planet->NoiseLayers.Add(NoiseLayer);
}

void UPlanetGeneratorBlueprintFunctionLibrary::SetClimateParameters(APlanetActor* Planet, float EquatorTemperature, float PoleTemperature, float MoistureScale)
{
	if (!Planet)
	{
		return;
	}
	
	Planet->EquatorTemperature = EquatorTemperature;
	Planet->PoleTemperature = PoleTemperature;
	Planet->MoistureScale = MoistureScale;
}

void UPlanetGeneratorBlueprintFunctionLibrary::AddBiome(APlanetActor* Planet, EBiomeType BiomeType, FLinearColor BiomeColor, float MinHeight, float MaxHeight, float MinTemperature, float MaxTemperature, float MinMoisture, float MaxMoisture)
{
	if (!Planet)
	{
		return;
	}
	
	// Check if biome already exists
	for (FBiomeSettings& Biome : Planet->Biomes)
	{
		if (Biome.BiomeType == BiomeType)
		{
			// Update existing biome
			Biome.BiomeColor = BiomeColor;
			Biome.MinHeight = MinHeight;
			Biome.MaxHeight = MaxHeight;
			Biome.MinTemperature = MinTemperature;
			Biome.MaxTemperature = MaxTemperature;
			Biome.MinMoisture = MinMoisture;
			Biome.MaxMoisture = MaxMoisture;
			return;
		}
	}
	
	// Add new biome
	FBiomeSettings NewBiome;
	NewBiome.BiomeType = BiomeType;
	NewBiome.BiomeColor = BiomeColor;
	NewBiome.MinHeight = MinHeight;
	NewBiome.MaxHeight = MaxHeight;
	NewBiome.MinTemperature = MinTemperature;
	NewBiome.MaxTemperature = MaxTemperature;
	NewBiome.MinMoisture = MinMoisture;
	NewBiome.MaxMoisture = MaxMoisture;
	
	Planet->Biomes.Add(NewBiome);
}

void UPlanetGeneratorBlueprintFunctionLibrary::RegeneratePlanet(APlanetActor* Planet)
{
	if (Planet)
	{
		Planet->GeneratePlanet();
	}
}
