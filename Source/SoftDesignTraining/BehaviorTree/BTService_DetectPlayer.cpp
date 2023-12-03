// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_DetectPlayer.h"
#include "../BT_SDTAIController.h"
#include "../SDTUtils.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
    bCreateNodeInstance = true;
}

void UBTService_DetectPlayer::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());
    auto selfPawn = ctrl->GetPawn();

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * ctrl->m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * ctrl->m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(ctrl->m_DetectionCapsuleRadius));

    DrawDebugCapsule(GetWorld(), detectionStartLocation + ctrl->m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), ctrl->m_DetectionCapsuleHalfLength, ctrl->m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);

    FHitResult detectionHit;
    ctrl->GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    if (detectionHit.GetComponent() && detectionHit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER)
    {
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetPlayerDetectedBBKeyID(), true);
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(ctrl->GetTargetPositionBBKeyID(), detectionHit.GetActor()->GetActorLocation());
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetPlayerDetectedBBKeyID(), false);
    }
    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetPlayerPoweredUpBBKeyID(), SDTUtils::IsPlayerPoweredUp(GetWorld()));
}
