// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetPhysicsHandle();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle == nullptr)
		return;

	if (!IsGrabbing())
		return;

	FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
}

bool UGrabber::IsGrabbing()
{
	UPrimitiveComponent *GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
	if (GrabbedComponent != nullptr)
	{
		GrabbedComponent->WakeAllRigidBodies();
		return true;
	}

	return false;
}

void UGrabber::Grab()
{
	if (PhysicsHandle == nullptr)
		return;

	FHitResult HitResult;
	if (GetGrabblableInReach(HitResult))
	{
		UPrimitiveComponent *HitComponent = HitResult.GetComponent();
		HitComponent->WakeAllRigidBodies();

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation());
	}
}

void UGrabber::Release()
{
	if (PhysicsHandle == nullptr)
		return;

	if (!IsGrabbing())
		return;

	PhysicsHandle->ReleaseComponent();
}

UPhysicsHandleComponent *UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent *Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (Result == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}

	return Result;
}

bool UGrabber::GetGrabblableInReach(FHitResult &OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	return GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		End,
		FQuat::Identity,

		// O valor de collision channel precisa ser verificado no arquivo /config/DefaultEngine.ini
		ECC_GameTraceChannel2,
		Sphere);
}