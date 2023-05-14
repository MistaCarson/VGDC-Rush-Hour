// Copyright Epic Games, Inc. All Rights Reserved.

#include "RushHourGameMode.h"
#include "RushHourCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARushHourGameMode::ARushHourGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_RushHourCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
