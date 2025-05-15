#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlanetActor.h"
#include "PlanetGeneratorBlueprintLibrary.generated.h"

UCLASS()
class PLANETGENERATOR_API UPlanetGeneratorBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static APlanetActor* SpawnPlanetActor(UObject* WorldContextObject, FVector Location, FRotator Rotation, float Radius = 1000.0f, int32 Resolution = 4);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void SetPlanetNoiseParameters(APlanetActor* PlanetActor, int32 NoiseLayerIndex, float Strength, int32 NumLayers, float BaseRoughness, float Roughness, float Persistence);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void SetPlanetBiomeParameters(APlanetActor* PlanetActor, EBiomeType BiomeType, FLinearColor BiomeColor, float MinHeight, float MaxHeight, float MinTemperature, float MaxTemperature, float MinMoisture, float MaxMoisture);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void SetPlanetClimateParameters(APlanetActor* PlanetActor, float EquatorTemperature, float PoleTemperature, float MoistureScale);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void RegeneratePlanet(APlanetActor* PlanetActor);
};
