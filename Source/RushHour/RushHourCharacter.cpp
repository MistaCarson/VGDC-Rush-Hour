// Copyright Epic Games, Inc. All Rights Reserved.

#include "RushHourCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MovementComponents/WallRunComponent.h"
#if WITH_EDITOR
	#include "Kismet/KismetSystemLibrary.h"
#endif


//////////////////////////////////////////////////////////////////////////
// ARushHourCharacter

ARushHourCharacter::ARushHourCharacter()
{	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	CrouchEyeOffset = FVector(0);
	CrouchSpeed = 13;

	WallRunComp = CreateDefaultSubobject<UWallRunComponent>(TEXT("Wall Run Component"));
}

void ARushHourCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	WallRunComp->Initialize(this);
}

//////////////////////////////////////////////////////////////////////////// Input

void ARushHourCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, WallRunComp, &UWallRunComponent::Jump);
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ARushHourCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Look);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Crouch, true);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ARushHourCharacter::UnCrouch, true);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARushHourCharacter::Sprint);
	}
}


void ARushHourCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
		WallRunComp->SetForwardInput(FMath::Clamp(MovementVector.Y, 0, 1));
	}
}

void ARushHourCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARushHourCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaSeconds);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;

	#if WITH_EDITOR
	if (PrintCharacterSpeedToScreen) {
		UKismetSystemLibrary::PrintString(GetWorld(), *FString::Printf(TEXT("Current Speed: %f"), GetMovementComponent()->Velocity.Length()), true, false, FColor::Red);
	}
	#endif
		
}

void ARushHourCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

void ARushHourCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}
	const float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);

}

void ARushHourCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}
	const float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void ARushHourCharacter::Sprint(const FInputActionValue& Value)
{
	const bool val = Value.Get<bool>();
	if (val) {
		GetCharacterMovement()->MaxWalkSpeed = 1200;
	} else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600;
	}
}

void ARushHourCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
	WallRunComp->Land();
}