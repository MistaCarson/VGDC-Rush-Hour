// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUSHHOUR_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	bool Dashing = false;

	// Base speed for a dash
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashSpeed;

	// How long a dash will take
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashTime;

	// What the character speed will by multiplied by upon completion of a dash
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashSpeedPenalty;

	// Edit with the loadout data class
	int NumberOfDashes = 1;

	int NumDashesRemaining;

	float CurrentDashSpeed = 0;

	float DashTimeLeft = 0;

	class ARushHourCharacter* Character;

	class UCharacterMovementComponent* CharacterMovement;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Call this from the character this is attached to
	void Initialize(ARushHourCharacter* player);

	void Dash(int DashType);

	bool IsDashing();

	// For when the dash is cancelled early (timer does not run out)
	void CancelDash();

	void SetNumberOfDashes(int NumDashes);

	void ResetDashes();
		
private:
	float PreDashSpeed;
	FVector PreDashDirection;
};
