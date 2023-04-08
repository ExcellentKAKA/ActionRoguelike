// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"




#include "SGameplayInterface.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	
	
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner= GetOwner();
	
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation,EyeRotation);
	
	FVector EndLocation =  EyeLocation + (EyeRotation.Vector() * 1000);

	
	
	// FHitResult Hit;
	// bool bBlockHit = GetWorld()->LineTraceSingleByObjectType(Hit,EyeLocation,EndLocation,QueryParams);

	TArray<FHitResult> Hits;

	float Radius = 30.0f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	bool bBlockHit = GetWorld()->SweepMultiByObjectType(Hits,EyeLocation,EndLocation,FQuat::Identity,QueryParams,Shape);
	FColor DebugColor = bBlockHit ? FColor::Green : FColor::Red;

	for(FHitResult Hit : Hits){
		AActor* HitActor = Hit.GetActor();
		if(HitActor)
		{
			if(HitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);

				ISGameplayInterface::Execute_Interact(HitActor,MyPawn);
				break;
			}
		}
		DrawDebugSphere(GetWorld(),Hit.ImpactPoint,Radius,32,DebugColor,false,2.0f);
	}
	DrawDebugLine(GetWorld(),EyeLocation,EndLocation,DebugColor,true,4.0f,0,1);
}


