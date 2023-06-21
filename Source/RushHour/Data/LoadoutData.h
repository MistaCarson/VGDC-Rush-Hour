// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LoadoutData.generated.h"

/**
 * 
 */
UCLASS()
class RUSHHOUR_API ULoadoutData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int NumberMidairJumps;

	UPROPERTY(EditAnywhere)
	bool MadDash;

	UPROPERTY(EditAnywhere)
	int NumberDashes;

	UPROPERTY(EditAnywhere)
	int MadDashDebugType;

	UPROPERTY(EditAnywhere)
	bool SpringShoes;

	UPROPERTY(EditAnywhere)
	bool AirStomp;
};
