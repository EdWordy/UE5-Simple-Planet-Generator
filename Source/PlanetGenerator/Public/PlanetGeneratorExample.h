#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanetActor.h"
#include "PlanetGeneratorExample.generated.h"

UCLASS()
class PLANETGENERATOR_API APlanetGeneratorExample : public AActor
{
	GENERATED_BODY()
	
public:	
	APlanetGeneratorExample();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example")
	bool GenerateOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example")
	float PlanetRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example", meta = (UIMin = "0", UIMax = "6"))
	int32 Resolution = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example")
	bool HasOcean = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example", meta = (UIMin = "0.0", UIMax = "1.0"))
	float OceanLevel = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example")
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example")
	float EquatorTemperature = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example")
	float PoleTemperature = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generator Example")
	float MoistureScale = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Planet Generator Example")
	void GeneratePlanet();

private:
	UPROPERTY()
	APlanetActor* Planet;
	
	void AddDefaultBiomes();
};
