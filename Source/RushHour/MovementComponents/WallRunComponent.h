// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Components/ActorComponent.h"
#include "WallRunComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUSHHOUR_API UWallRunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallRunComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Initialize this component with a reference to the player character, call in RushHourCharacter::BeginPlay
	void Initialize(ACharacter* player);

	// Call this when character jumps
	void Jump();

	// Call this when character lands
	void Land();

	void SetForwardInput(float forwardInput);
	
	bool IsWallRunning();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Multiplier for the forace applied to the character while wall running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	float WallRunForceMultiplier = 70000;
	// Max Velocity Wall running can bring the character up to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	float MaxWallRunVelocity = 1500;
	// Time after stopping a wall run before another should start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	float WallRunCooldown = .25;
	//Amount of tilt the camera should have while running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	float CameraTiltAmount = 15;
	// Should gravity be applied while wall running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	bool UseGravity = false;
	//Scalar for wall running gravity if on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	float WallRunTargetGravity = .4;
	// Height of jump after exiting a wall run
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	float WallRunJumpHeight = 500;
	// How much force should be applied off the wall when jumping off
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BlueprintProtected = "true"))
	float WallRunJumpForce = 300;
private:
	// Reference to player character
	ACharacter* Character;
	// Reference to player's characterMovementComponent
	class UCharacterMovementComponent* CharacterMovement;
	// Reference to standard gravity
	float DefaultGravity;
	// Timer for calling WallRunUpdate
	FTimerHandle WallRunUpdateTimer;
	// Timer for the cooldown of the wall run
	FTimerHandle WallRunCooldownTimer;
	// Storage for the normal vector of the wall being run on
	FVector WallRunNormal;
	bool WallRunning = false;
	bool WallRunningRight = false;
	bool WallRunningLeft = false;
	bool WallRunOnCooldown = false;
	// Called when wall run is over to reset stuff
	void WallRunEnd();
	// Called when the wall run cooldown expires
	void WallRunCooldownExpire();
	// Tilts the camera by the given ammount
	void CameraTilt(float roll);
	// Retrieves the vectors for casting rays for wall running
	TArray<FVector> GetRightLeftVectors();
	// Carries out the wall running movement
	bool WallRunMovement(FVector ActionVector, float WallRunDirection);
	// Checks if a given wall is valid
	bool IsValidWall(FVector HitVector);
	// Gets vector of player to wall for applying force
	FVector GetPlayerToWallVector();
	// Amount of current forward input
	float ForwardInput;

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditAnywhere, Category = "Debug")
		bool DrawRayCasts;
#endif
};
