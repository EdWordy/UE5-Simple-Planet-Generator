#include "PlanetGeneratorBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

APlanetActor* UPlanetGeneratorBlueprintLibrary::SpawnPlanetActor(UObject* WorldContextObject, FVector Location, FRotator Rotation, float Radius, int32 Resolution)
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

void UPlanetGeneratorBlueprintLibrary::SetPlanetNoiseParameters(APlanetActor* PlanetActor, int32 NoiseLayerIndex, float Strength, int32 NumLayers, float BaseRoughness, float Roughness, float Persistence)
{
	if (!PlanetActor || NoiseLayerIndex < 0 || NoiseLayerIndex >= PlanetActor->NoiseLayers.Num())
	{
		return;
	}
	
	FNoiseLayer& NoiseLayer = PlanetActor->NoiseLayers[NoiseLayerIndex];
	NoiseLayer.Strength = Strength;
	NoiseLayer.NumLayers = NumLayers;
	NoiseLayer.BaseRoughness = BaseRoughness;
	NoiseLayer.Roughness = Roughness;
	NoiseLayer.Persistence = Persistence;
}

void UPlanetGeneratorBlueprintLibrary::SetPlanetBiomeParameters(APlanetActor* PlanetActor, EBiomeType BiomeType, FLinearColor BiomeColor, float MinHeight, float MaxHeight, float MinTemperature, float MaxTemperature, float MinMoisture, float MaxMoisture)
{
	if (!PlanetActor)
	{
		return;
	}
	
	// Find existing biome or add new one
	FBiomeSettings* ExistingBiome = nullptr;
	for (FBiomeSettings& Biome : PlanetActor->Biomes)
	{
		if (Biome.BiomeType == BiomeType)
		{
			ExistingBiome = &Biome;
			break;
		}
	}
	
	if (ExistingBiome)
	{
		ExistingBiome->BiomeColor = BiomeColor;
		ExistingBiome->MinHeight = MinHeight;
		ExistingBiome->MaxHeight = MaxHeight;
		ExistingBiome->MinTemperature = MinTemperature;
		ExistingBiome->MaxTemperature = MaxTemperature;
		ExistingBiome->MinMoisture = MinMoisture;
		ExistingBiome->MaxMoisture = MaxMoisture;
	}
	else
	{
		FBiomeSettings NewBiome;
		NewBiome.BiomeType = BiomeType;
		NewBiome.BiomeColor = BiomeColor;
		NewBiome.MinHeight = MinHeight;
		NewBiome.MaxHeight = MaxHeight;
		NewBiome.MinTemperature = MinTemperature;
		NewBiome.MaxTemperature = MaxTemperature;
		NewBiome.MinMoisture = MinMoisture;
		NewBiome.MaxMoisture = MaxMoisture;
		PlanetActor->Biomes.Add(NewBiome);
	}
}

void UPlanetGeneratorBlueprintLibrary::SetPlanetClimateParameters(APlanetActor* PlanetActor, float EquatorTemperature, float PoleTemperature, float MoistureScale)
{
	if (!PlanetActor)
	{
		return;
	}
	
	PlanetActor->EquatorTemperature = EquatorTemperature;
	PlanetActor->PoleTemperature = PoleTemperature;
	PlanetActor->MoistureScale = MoistureScale;
}

void UPlanetGeneratorBlueprintLibrary::RegeneratePlanet(APlanetActor* PlanetActor)
{
	if (PlanetActor)
	{
		PlanetActor->GeneratePlanet();
	}
}
