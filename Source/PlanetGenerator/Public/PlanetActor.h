#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "SimplexNoiseBPLibrary.h"
#include "PlanetActor.generated.h"

UENUM(BlueprintType)
enum class EBiomeType : uint8
{
	Ocean UMETA(DisplayName = "Ocean"),
	Beach UMETA(DisplayName = "Beach"),
	Desert UMETA(DisplayName = "Desert"),
	Plains UMETA(DisplayName = "Plains"),
	Forest UMETA(DisplayName = "Forest"),
	Mountains UMETA(DisplayName = "Mountains"),
	SnowCapped UMETA(DisplayName = "Snow Capped"),
	Tundra UMETA(DisplayName = "Tundra")
};

USTRUCT(BlueprintType)
struct FBiomeSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	EBiomeType BiomeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	FLinearColor BiomeColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	float MinHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	float MaxHeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	float MinTemperature = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	float MaxTemperature = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	float MinMoisture = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	float MaxMoisture = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	UMaterialInterface* BiomeMaterial = nullptr;
};

USTRUCT(BlueprintType)
struct FNoiseLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise")
	bool Enabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise", meta = (UIMin = "0.0", UIMax = "10.0"))
	float Strength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise", meta = (UIMin = "1", UIMax = "8"))
	int32 NumLayers = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise", meta = (UIMin = "0.0", UIMax = "5.0"))
	float BaseRoughness = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise", meta = (UIMin = "0.0", UIMax = "5.0"))
	float Roughness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise", meta = (UIMin = "0.0", UIMax = "5.0"))
	float Persistence = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise")
	FVector Center = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise", meta = (UIMin = "0.1", UIMax = "10.0"))
	float MinValue = 1.0f;
};

UCLASS(BlueprintType, Blueprintable)
class PLANETGENERATOR_API APlanetActor : public AActor
{
	GENERATED_BODY()

public:
	APlanetActor();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Planet")
	UProceduralMeshComponent* PlanetMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Generation", meta = (UIMin = "1.0", UIMax = "10000.0"))
	float PlanetRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Generation", meta = (UIMin = "0", UIMax = "6"))
	int32 Resolution = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Generation")
	bool AutoUpdate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Generation")
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Noise")
	TArray<FNoiseLayer> NoiseLayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Biomes")
	TArray<FBiomeSettings> Biomes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Climate")
	float EquatorTemperature = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Climate")
	float PoleTemperature = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Climate")
	float MoistureScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Materials")
	UMaterialInterface* PlanetMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Materials")
	bool TwoSidedMaterial = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Materials")
	float AmbientOcclusion = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Materials")
	float EmissiveStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Materials")
	bool GenerateUVs = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Materials")
	bool GenerateVertexColors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Debug")
	bool ShowNormals = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Debug")
	float NormalLength = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Rotation")
	bool AutoRotate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Rotation", meta = (UIMin = "0.0", UIMax = "10.0", EditCondition = "AutoRotate"))
	float RotationSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|Rotation", meta = (EditCondition = "AutoRotate"))
	FVector RotationAxis = FVector(0.0f, 0.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|TileSelection")
	bool EnableTileSelection = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|TileSelection", meta = (EditCondition = "EnableTileSelection"))
	FLinearColor SelectedTileColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet|TileSelection", meta = (EditCondition = "EnableTileSelection"))
	float SelectedTileHighlightIntensity = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Planet|TileSelection")
	int32 SelectedTileIndex = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Planet|TileSelection")
	FVector SelectedTileLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Planet|TileSelection")
	EBiomeType SelectedTileBiome;

	UFUNCTION(BlueprintCallable, Category = "Planet|TileSelection")
	bool SelectTileAtScreenPosition(APlayerController* PlayerController, FVector2D ScreenPosition);

	UFUNCTION(BlueprintCallable, Category = "Planet|TileSelection")
	void ClearSelectedTile();

	UFUNCTION(BlueprintImplementableEvent, Category = "Planet|TileSelection")
	void OnTileSelected(int32 TileIndex, FVector TileLocation, EBiomeType TileBiome);

	UFUNCTION(BlueprintCallable, Category = "Planet")
	void GeneratePlanet();

	UFUNCTION(BlueprintCallable, Category = "Planet")
	void ClearMesh();

private:
	void CreateIcosphere();
	void SubdivideIcosphere(int32 Subdivisions);
	float EvaluateNoise(const FVector& PointOnUnitSphere);
	FVector CalculatePointOnPlanet(const FVector& PointOnUnitSphere);
	float GetTemperature(const FVector& PointOnUnitSphere);
	float GetMoisture(const FVector& PointOnUnitSphere);
	EBiomeType DetermineBiome(float Height, float Temperature, float Moisture);
	FLinearColor GetBiomeColor(EBiomeType BiomeType, float Height, float Temperature, float Moisture);
	int32 GetMiddlePoint(int32 p1, int32 p2, TMap<FVector2D, int32>& Cache);

	TArray<FLinearColor> OriginalVertexColors;
	bool UpdateSelectedTileVisual();
	int32 FindTriangleIndexFromHitLocation(const FVector& HitLocation);

	UPROPERTY()
	TArray<FVector> Vertices;
	
	UPROPERTY()
	TArray<int32> Triangles;
	
	UPROPERTY()
	TArray<FVector> Normals;
	
	UPROPERTY()
	TArray<FVector2D> UV0;

	UPROPERTY()
	TArray<FLinearColor> VertexColors;

	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;

	UPROPERTY()
	// Cache the final vertices for later use in tile selection
	TArray<FVector> CachedVertices;

	UPROPERTY()
	// Store the selected triangle vertices in local space
	TArray<FVector> SelectedTriangleVertices;

	// Store triangles separately to ensure they're preserved
	UPROPERTY()
	TArray<int32> StoredTriangles;
};
