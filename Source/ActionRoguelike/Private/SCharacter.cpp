// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"


#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"



// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
	
	AttackAnimDelay = 0.2f;

	InteractionComponent = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
}



void ASCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(), value);
}

void ASCharacter::MoveRight(float value)
{

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	FVector RightVector= FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	
	//UE_LOG(LogTemp,Warning,TEXT("%f"),value);
	AddMovementInput(RightVector,value);
}



void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackMontage);


	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack,this,&ASCharacter::PrimaryAttack_TimeElapsed,AttackAnimDelay);
	//GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
	
	
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	SpawnProjectile(ProjectileClass);
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(AttackMontage);
	GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASCharacter::BlackHoleAttack_TimeElapsed,AttackAnimDelay);
}

void ASCharacter::BlackHoleAttack_TimeElapsed()
{
	SpawnProjectile(BlackHoleProjectileClass);
}

void ASCharacter::Dash()
{
	PlayAnimMontage(AttackMontage);
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::Dash_TimeElapsed,AttackAnimDelay);
}

void ASCharacter::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if(ensure(ClassToSpawn))
	{
		FVector HandRLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FHitResult Hit;
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		
		FVector TrackStart = CameraComponent->GetComponentLocation();
		FVector TrackEnd = CameraComponent->GetComponentLocation() + (GetControlRotation().Vector() * 5000.0f);
		
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(20.0f);
		
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FRotator ProjRotation;
		if(GetWorld()->SweepSingleByObjectType(Hit, TrackStart, TrackEnd, FQuat::Identity, ObjectQueryParams, CollisionShape, Params))
		{
			TrackEnd = Hit.ImpactPoint;
			
		}
		ProjRotation = FRotationMatrix::MakeFromX(TrackEnd - HandRLocation).Rotator();
		
		FTransform SpawnTM = FTransform(ProjRotation, HandRLocation);
		GetWorld()->SpawnActor<AActor>(ClassToSpawn,SpawnTM,SpawnParams);
	}
}

void ASCharacter::PrimaryInteract()
{
	if(InteractionComponent)
	{
		InteractionComponent->PrimaryInteract();
	}
	
}



// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup",this,&APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack",IE_Pressed,this,&ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);


	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
	
	PlayerInputComponent->BindAction("PrimaryInteract",IE_Pressed,this,&ASCharacter::PrimaryInteract);


}

