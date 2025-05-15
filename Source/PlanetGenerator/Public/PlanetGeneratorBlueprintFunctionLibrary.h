#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlanetActor.h"
#include "PlanetGeneratorBlueprintFunctionLibrary.generated.h"

UCLASS()
class PLANETGENERATOR_API UPlanetGeneratorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static APlanetActor* CreatePlanet(UObject* WorldContextObject, FVector Location, FRotator Rotation, float Radius = 1000.0f, int32 Resolution = 4);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void AddNoiseLayer(APlanetActor* Planet, float Strength = 1.0f, int32 NumLayers = 4, float BaseRoughness = 1.0f, float Roughness = 2.0f, float Persistence = 0.5f);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void SetClimateParameters(APlanetActor* Planet, float EquatorTemperature = 1.0f, float PoleTemperature = 0.0f, float MoistureScale = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void AddBiome(APlanetActor* Planet, EBiomeType BiomeType, FLinearColor BiomeColor, float MinHeight = 0.0f, float MaxHeight = 1.0f, float MinTemperature = 0.0f, float MaxTemperature = 1.0f, float MinMoisture = 0.0f, float MaxMoisture = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator")
	static void RegeneratePlanet(APlanetActor* Planet);
};
