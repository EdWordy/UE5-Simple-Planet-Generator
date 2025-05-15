#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlanetMaterialGenerator.generated.h"

UCLASS()
class PLANETGENERATOR_API UPlanetMaterialGenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Planet Generator|Materials")
	static UMaterialInstanceDynamic* CreatePlanetMaterial(UObject* WorldContextObject, 
                                                         FLinearColor BaseColor = FLinearColor(0.2f, 0.5f, 0.2f, 1.0f), 
                                                         float Roughness = 0.8f, 
                                                         float Metallic = 0.0f,
                                                         bool TwoSided = true,
                                                         float AmbientOcclusion = 0.5f,
                                                         float EmissiveStrength = 0.0f);
	
	UFUNCTION(BlueprintCallable, Category = "Planet Generator|Materials")
	static UMaterialInstanceDynamic* CreateOceanMaterial(UObject* WorldContextObject, 
                                                        FLinearColor WaterColor = FLinearColor(0.0f, 0.3f, 0.6f, 0.7f), 
                                                        float Roughness = 0.2f, 
                                                        float Metallic = 0.1f, 
                                                        float Opacity = 0.7f,
                                                        bool TwoSided = true);

	UFUNCTION(BlueprintCallable, Category = "Planet Generator|Materials")
	static UMaterialInstanceDynamic* CreateBiomeMaterial(UObject* WorldContextObject, 
                                                        FLinearColor BaseColor, 
                                                        float Roughness = 0.8f, 
                                                        float Metallic = 0.0f,
                                                        bool TwoSided = true,
                                                        float AmbientOcclusion = 0.5f,
                                                        float EmissiveStrength = 0.0f);
};
