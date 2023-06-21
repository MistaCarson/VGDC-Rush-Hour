// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Data/LoadoutData.h"
#include "RushHourCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;


UCLASS(config=Game)
class ARushHourCharacter : public ACharacter
{
	GENERATED_BODY()
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
		FVector CrouchEyeOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
		float CrouchSpeed;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
		float SlideSpeedThreshold = 20;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementComponents", meta = (AllowPrivateAccess = "true"))
		class UWallRunComponent* WallRunComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementComponents", meta = (AllowPrivateAccess = "true"))
		class UDashComponent* DashComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Jumping / Falling", meta = (AllowPrivateAccess = "true"))
		float SpeedScaleOnAirJump = .7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Jumping / Falling", meta = (AllowPrivateAccess = "true"))
		float AirJumpVelocity = 800;

public:
	ARushHourCharacter();
	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	bool IsSprinting();

	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& hit);


protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void Jump() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected = "true"))
	ULoadoutData* AbilityData;

private:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Sprint(const FInputActionValue& Value);

	void Dash(const FInputActionValue& Value);
	bool Sprinting = false;
	bool SprintPressedInAir = false;
	bool SprintReleasedInAir = false;

	// Can the player look around while dashing?
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	bool LookWhileDashing = false;

	float DefaultJumpVelocity;

	FVector2D LastMovementVector;

	// Used after a double jump for air control
	float InAirMovementTime = 0;
	
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditAnywhere, Category = "Debug")
		bool PrintCharacterSpeedToScreen;
#endif

};

