// Fill out your copyright notice in the Description page of Project Settings.


#include "Service_DetectTarget.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../SDTAIController.h"
#include "../SDTUtils.h"


void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit)
{
    for (const FHitResult& hit : hits)
    {
        if (UPrimitiveComponent* component = hit.GetComponent())
        {
            if (component->GetCollisionObjectType() == COLLISION_PLAYER)
            {
                //we can't get more important than the player
                outDetectionHit = hit;
                return;
            }
            else if (component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
            {
                outDetectionHit = hit;
            }
        }
    }
}

void UDetectTarget::TickNode(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
    UBlackboardComponent* myBlackboard = ownerComp.GetBlackboardComponent();
    if (myBlackboard == nullptr)
    {
        return;
    }

    ASDTAIController* aiController = Cast<ASDTAIController>(ownerComp.GetAIOwner());
    if (aiController == nullptr)
    {
        return;
    }

    const APawn* selfPawn = aiController->GetPawn();
    if (selfPawn == nullptr)
    {
        return;
    }

    const ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (playerCharacter == nullptr)
    {
        return;
    }


    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * aiController->m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * aiController->m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(aiController->m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    // Use the focus array inside the AAIController class to put the detected player or the collectible to go to.
    //AActor* focusedActor = aiController->GetFocusActor();
    auto newHit = detectionHit.GetActor();
    // If we had a player in focus, but we don't have it anymore in LOS, keep only the latest position of the player as a focal point,
    // otherwise set the actor found (or clear the focus if there is no actor in LOS).
    //if (Cast<ACharacter>(focusedActor) != nullptr && Cast<ACharacter>(newHit) == nullptr)
    //{
    //    aiController->SetFocalPoint(focusedActor->GetActorLocation());
    //}
    //else
    if (newHit != nullptr)
    {
        if (Cast<ACharacter>(newHit) != nullptr)
        {
            FName lastKnownPositionName("LastKnownPosition");
            if (myBlackboard->GetKeyType(myBlackboard->GetKeyID(lastKnownPositionName)) == UBlackboardKeyType_Object::StaticClass())
            {
                myBlackboard->SetValueAsVector(lastKnownPositionName, newHit->GetActorLocation());
            }
        }
        aiController->SetFocus(newHit);
    }
    else
    {
        aiController->ClearFocus(EAIFocusPriority::Gameplay);
    }
}
