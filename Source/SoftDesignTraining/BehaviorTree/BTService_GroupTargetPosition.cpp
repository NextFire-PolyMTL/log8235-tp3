// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_GroupTargetPosition.h"
#include "GroupManager.h"
#include "../BT_SDTAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTService_GroupTargetPosition::UBTService_GroupTargetPosition()
{
    bCreateNodeInstance = true;
}

void UBTService_GroupTargetPosition::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());

    auto groupManager = GroupManager::GetInstance();

    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetShouldInvestigateBBKeyID(), groupManager->ShouldInvestigate());

    auto playerFound = false;
    auto playerLKP = groupManager->GetLKPFromGroup(playerFound);

    if (!playerFound)
    {
        return; // FIXME: hmmmm
    }

    DrawDebugSphere(GetWorld(), playerLKP, 30.0f, 32, FColor::Green);

    FVector targetPos;
    if ((playerLKP - ctrl->GetPawn()->GetActorLocation()).Size2D() > ctrl->m_DetectionCapsuleRadius)
    {
        // surround player
        auto controllers = groupManager->GetRegisteredControllers();
        auto numControllers = controllers.Num();
        auto currentControllerIndex = controllers.Find(ctrl);
        auto radius = ctrl->m_DetectionCapsuleRadius;
        auto angle = 2 * PI * currentControllerIndex / numControllers;
        targetPos = playerLKP + FVector(radius * cos(angle), radius * sin(angle), 0.0f);
    }
    else
    {
        // just attack
        targetPos = playerLKP;
    }

    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(ctrl->GetGroupTargetPositionBBKeyID(), targetPos);
    DrawDebugSphere(GetWorld(), targetPos, 30.0f, 32, FColor::Orange);
    DrawDebugLine(GetWorld(), ctrl->GetPawn()->GetActorLocation(), targetPos, FColor::Orange);
}
