#include "TriggerComponent.h"

UTriggerComponent::UTriggerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTriggerComponent::BeginPlay()
{
    Super::BeginPlay();

    Mover = GetOwner()->FindComponentByClass<UMover>();
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor *Actor = GetAcceptableActor();
    if (Actor != nullptr)
    {
        Mover->SetShouldMove(true);
    }
}

void UTriggerComponent::SetMover(UMover *NewMover)
{
    Mover = NewMover;
}

AActor *UTriggerComponent::GetAcceptableActor() const
{
    TArray<AActor *> Actors;
    GetOverlappingActors(Actors);

    for (AActor *Actor : Actors)
    {
        if (Actor->ActorHasTag(AcceptableActorTag))
        {
            UE_LOG(LogTemp, Display, TEXT("Actor %s"), *Actor->GetActorNameOrLabel());
            return Actor;
        }
    }

    return nullptr;
}