// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bird Mesh"));
	BirdMesh->SetupAttachment(GetRootComponent());

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
}

void ABird::BeginPlay()
{
	Super::BeginPlay(); 

	if ( APlayerController* PlayerController = Cast<APlayerController>(GetController()) ) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
	
}

void ABird::MoveForward(float Value)
{
	if ( Controller && Value != 0.f) {
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value); 
	}
}

void ABird::Move(const FInputActionValue& Value)
{
	const FVector CurrentValue = Value.Get<FVector>();
	if (Controller && CurrentValue != FVector::ZeroVector) {

		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		
		//FVector Right = GetActorRightVector();
		//FVector Forward = GetActorForwardVector();  
		AddMovementInput(RightDir, CurrentValue.X);
		AddMovementInput(ForwardDir, CurrentValue.Y);
	}
}

void ABird::Look(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	if (Controller && CurrentValue != FVector2D::ZeroVector) {
		AddControllerYawInput(CurrentValue.X);
		AddControllerPitchInput(CurrentValue.Y); 
	}
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward); // Old Input
	if (UEnhancedInputComponent* EInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		EInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}

