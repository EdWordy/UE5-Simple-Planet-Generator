// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlanetGenerator.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FPlanetGeneratorModule"

void FPlanetGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
	UE_LOG(LogTemp, Log, TEXT("Planet Generator Plugin started"));
}

void FPlanetGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
	UE_LOG(LogTemp, Log, TEXT("Planet Generator Plugin shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPlanetGeneratorModule, PlanetGenerator)
