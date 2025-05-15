#include "PlanetMaterialGenerator.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include <MaterialDomain.h>


UMaterialInstanceDynamic* UPlanetMaterialGenerator::CreatePlanetMaterial(UObject* WorldContextObject, 
                                                                        FLinearColor BaseColor, 
                                                                        float Roughness, 
                                                                        float Metallic,
                                                                        bool TwoSided,
                                                                        float AmbientOcclusion,
                                                                        float EmissiveStrength)
{
    // Load the base material from your project's content
    UMaterial* BaseMaterial = LoadObject<UMaterial>(nullptr, TEXT("/PlanetGenerator/Materials/M_PlanetBase"));
    
    if (!BaseMaterial)
    {
        // Fallback to default material
        UE_LOG(LogTemp, Warning, TEXT("M_PlanetBase material not found. Using default material instead."));
        BaseMaterial = UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface);
    }
    
    UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, WorldContextObject);
    
    if (MaterialInstance)
    {
        // Set parameters
        MaterialInstance->SetVectorParameterValue(FName("BaseColor"), BaseColor);
        MaterialInstance->SetScalarParameterValue(FName("Roughness"), Roughness);
        MaterialInstance->SetScalarParameterValue(FName("Metallic"), Metallic);
        
        // Set two-sided parameter if it exists
        MaterialInstance->SetScalarParameterValue(FName("TwoSided"), TwoSided ? 1.0f : 0.0f);
        
        // Set ambient occlusion if it exists
        MaterialInstance->SetScalarParameterValue(FName("AmbientOcclusion"), AmbientOcclusion);
        
        // Set emissive strength if it exists
        MaterialInstance->SetScalarParameterValue(FName("EmissiveStrength"), EmissiveStrength);
        
        // Force two-sided rendering
        if (TwoSided)
        {
            MaterialInstance->TwoSided = 1;
        }
    }
    
    return MaterialInstance;
}

UMaterialInstanceDynamic* UPlanetMaterialGenerator::CreateOceanMaterial(UObject* WorldContextObject, 
                                                                       FLinearColor WaterColor, 
                                                                       float Roughness, 
                                                                       float Metallic, 
                                                                       float Opacity,
                                                                       bool TwoSided)
{
    // Load the base material from your project's content
    UMaterial* BaseMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/Materials/M_OceanBase"));
    
    if (!BaseMaterial)
    {
        // Fallback to default material
        UE_LOG(LogTemp, Warning, TEXT("M_OceanBase material not found. Using default translucent material instead."));
        BaseMaterial = UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface);
    }
    
    UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, WorldContextObject);
    
    if (MaterialInstance)
    {
        // Set parameters
        MaterialInstance->SetVectorParameterValue(FName("BaseColor"), WaterColor);
        MaterialInstance->SetScalarParameterValue(FName("Roughness"), Roughness);
        MaterialInstance->SetScalarParameterValue(FName("Metallic"), Metallic);
        MaterialInstance->SetScalarParameterValue(FName("Opacity"), Opacity);
        
        // Set two-sided parameter if it exists
        MaterialInstance->SetScalarParameterValue(FName("TwoSided"), TwoSided ? 1.0f : 0.0f);
        
        // Force two-sided rendering
        if (TwoSided)
        {
            MaterialInstance->TwoSided = 1;
        }
    }
    
    return MaterialInstance;
}

UMaterialInstanceDynamic* UPlanetMaterialGenerator::CreateBiomeMaterial(UObject* WorldContextObject, 
                                                                       FLinearColor BaseColor, 
                                                                       float Roughness, 
                                                                       float Metallic,
                                                                       bool TwoSided,
                                                                       float AmbientOcclusion,
                                                                       float EmissiveStrength)
{
    // Reuse the planet material function but with different default parameters
    return CreatePlanetMaterial(WorldContextObject, BaseColor, Roughness, Metallic, TwoSided, AmbientOcclusion, EmissiveStrength);
}
