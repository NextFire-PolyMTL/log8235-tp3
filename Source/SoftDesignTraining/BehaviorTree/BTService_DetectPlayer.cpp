// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_DetectPlayer.h"
#include "../BT_SDTAIController.h"
#include "../SDTUtils.h"
#include "../SoftDesignTraining.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
    bCreateNodeInstance = true;
}

void UBTService_DetectPlayer::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
    /*
    double timeElapsed = SDTUtils::MeasureExecutionTime(&UBTService_DetectPlayer::PerformDetectPlayer, this, OwnerComp, NodeMemory, DeltaSeconds);
    OwnerComp.GetBlackboardComponent()->SetValueAsFloat("TimeSpentFindPlayer", timeElapsed);
    */
}

void UBTService_DetectPlayer::PerformDetectPlayer(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) const
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());
    auto selfPawn = ctrl->GetPawn();

    auto movcomp = ctrl->GetCharacter()->GetMovementComponent();
    if (movcomp->IsFlying() || movcomp->IsFalling())
    {
        return;
    }

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * ctrl->m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * ctrl->m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(ctrl->m_DetectionCapsuleRadius));

    DrawDebugCapsule(GetWorld(), detectionStartLocation + ctrl->m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), ctrl->m_DetectionCapsuleHalfLength, ctrl->m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);

    FHitResult detectionHit;
    ctrl->GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    auto playerDetected = detectionHit.GetComponent() && (detectionHit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER);

    if (playerDetected)
    {
        // Direct raycast to check for walls
        FHitResult hit;
        GetWorld()->LineTraceSingleByChannel(hit, detectionStartLocation, detectionHit.ImpactPoint, ECollisionChannel::ECC_WorldStatic);
        playerDetected = hit.GetComponent() && (hit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER);
    }

    if (playerDetected)
    {
        DrawDebugLine(GetWorld(), detectionStartLocation, detectionHit.ImpactPoint, FColor::Red);
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetPlayerDetectedBBKeyID(), true);
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(ctrl->GetPlayerPositionBBKeyID(), detectionHit.GetActor()->GetActorLocation());
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Float>(ctrl->GetLastSeenBBKeyID(), UGameplayStatics::GetRealTimeSeconds(GetWorld()));
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetPlayerDetectedBBKeyID(), false);
    }
}
