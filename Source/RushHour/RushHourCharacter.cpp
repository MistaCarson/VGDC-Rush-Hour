// Copyright Epic Games, Inc. All Rights Reserved.

#include "RushHourCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MovementComponents/WallRunComponent.h"
#include "MovementComponents/DashComponent.h"
#include "MovementComponents/SlideComponent.h"
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
	DashComp = CreateDefaultSubobject<UDashComponent>(TEXT("Dash Component"));
	SlideComp = CreateDefaultSubobject<USlideComponent>(TEXT("Slide Comp"));
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
	DashComp->Initialize(this);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARushHourCharacter::OnCapsuleHit);
	WallRunComp->WallRunStartEvent.BindUObject(DashComp, &UDashComponent::ResetDashes);
	DashComp->SetNumberOfDashes(AbilityData->NumberDashes);
	DefaultJumpVelocity = GetCharacterMovement()->JumpZVelocity;
	SlideComp->Initialize(this);
}

//////////////////////////////////////////////////////////////////////////// Input

void ARushHourCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, WallRunComp, &UWallRunComponent::Jump);
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ARushHourCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Look);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Crouch, true);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ARushHourCharacter::UnCrouch, true);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ARushHourCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARushHourCharacter::Sprint);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ARushHourCharacter::Dash);
	}
}


void ARushHourCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (SlideComp->IsSliding()) {
		MovementVector = FVector2D::Zero();
	}

	if (Controller != nullptr)
	{
		// add movement 
		if (!GetCharacterMovement()->IsFalling()) {
			AddMovementInput(GetActorForwardVector(), MovementVector.Y);
			if (Sprinting) {
				AddMovementInput(GetActorRightVector(), MovementVector.X / 3);
			}
			else {
				AddMovementInput(GetActorRightVector(), MovementVector.X);
			}
		}
		else {
			AddMovementInput(GetActorRightVector(), MovementVector.X);
			if (InAirMovementTime > 0) {
				AddMovementInput(GetActorForwardVector(), MovementVector.Y);
			} else if ((GetActorForwardVector() * MovementVector.Y).Dot(GetMovementComponent()->Velocity) < 0) {
				AddMovementInput(GetActorForwardVector(), MovementVector.Y);
			}
		}
		WallRunComp->SetForwardInput(FMath::Clamp(MovementVector.Y, 0, 1));

	}
	LastMovementVector = MovementVector;
}

void ARushHourCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		if (LookWhileDashing || !DashComp->IsDashing()) {
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

void ARushHourCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaSeconds);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;
	JumpMaxCount = AbilityData->NumberMidairJumps + 1;
	#if WITH_EDITOR
	if (PrintCharacterSpeedToScreen) {
		FVector2D LateralVelocity = FVector2D(GetMovementComponent()->Velocity.X, GetMovementComponent()->Velocity.Y);
		UKismetSystemLibrary::PrintString(GetWorld(), *FString::Printf(TEXT("Current Speed: %f"), LateralVelocity.Length()), true, false, FColor::Red);
	}
	DashComp->SetNumberOfDashes(AbilityData->NumberDashes);
	#endif
	if (InAirMovementTime > 0) {
		InAirMovementTime -= DeltaSeconds;
	}
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
	SlideComp->EndSlide();
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
	SlideComp->TryBeginSlide();
}

void ARushHourCharacter::Sprint(const FInputActionValue& Value)
{
	const bool val = Value.Get<bool>();
	if (GetCharacterMovement()->IsFalling()) {
		if (val) {
			SprintPressedInAir = true;
			SprintReleasedInAir = false;
		}
		else {
			SprintPressedInAir = false;
			SprintReleasedInAir = true;
		}
	}
	else {
		if (val) {
			GetCharacterMovement()->MaxWalkSpeed = 1200;
			Sprinting = true;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 600;
			Sprinting = false;
		}
	}
}

void ARushHourCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
	WallRunComp->Land();
	DashComp->ResetDashes();
	if (SprintPressedInAir) {
		SprintPressedInAir = false;
		GetCharacterMovement()->MaxWalkSpeed = 1200;
		Sprinting = true;
	}
	else if (SprintReleasedInAir) {
		SprintReleasedInAir = false;
		GetCharacterMovement()->MaxWalkSpeed = 600;
		Sprinting = false;
	}
}

void ARushHourCharacter::Jump() {
	if (!DashComp->IsDashing()) {
		if (WallRunComp->IsWallRunning()) {
			WallRunComp->Jump();
		}
		else {
			if (GetCharacterMovement()->IsFalling() && CanJump()) {
				FVector LateralVelocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0);
				float Speed = LateralVelocity.Length();
				LateralVelocity.Normalize();
				FVector NewVelocity = LateralVelocity * Speed * .7;
				NewVelocity.Z = GetCharacterMovement()->Velocity.Z;
				GetCharacterMovement()->Velocity = NewVelocity;
				GetCharacterMovement()->JumpZVelocity = AirJumpVelocity;
				Super::Jump();
				GetCharacterMovement()->JumpZVelocity = DefaultJumpVelocity; 
				AddMovementInput(GetActorForwardVector(), LastMovementVector.Y);
				InAirMovementTime = 1;
			}
			else {
				Super::Jump();
			}
		}
	}

}

bool ARushHourCharacter::IsSprinting() {
	return Sprinting;
}

void ARushHourCharacter::Dash(const FInputActionValue& Value) {
	if (Value.Get<bool>() && AbilityData->MadDash) {
		DashComp->Dash(AbilityData->MadDashDebugType);
	}
}

void ARushHourCharacter::OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& hit) {
	DashComp->CancelDash();
}