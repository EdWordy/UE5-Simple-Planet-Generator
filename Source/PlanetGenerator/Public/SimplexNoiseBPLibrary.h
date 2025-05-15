#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimplexNoiseBPLibrary.generated.h"

UCLASS()
class PLANETGENERATOR_API USimplexNoiseBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 1D Simplex Noise
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise1D(float X);

	// 2D Simplex Noise
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise2D(float X, float Y);

	// 3D Simplex Noise
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise3D(float X, float Y, float Z);

	// 4D Simplex Noise
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise4D(float X, float Y, float Z, float W);

	// Fractal Brownian Motion (FBM) noise
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseFBM(float X, float Y, float Z, int32 Octaves, float Persistence, float Lacunarity);

	// Set the seed for the noise
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static void SetNoiseSeed(int32 NewSeed);
};
