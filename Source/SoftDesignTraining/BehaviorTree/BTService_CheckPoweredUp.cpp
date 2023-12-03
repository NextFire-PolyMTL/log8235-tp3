// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_CheckPoweredUp.h"
#include "../BT_SDTAIController.h"
#include "../SDTUtils.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTService_CheckPoweredUp::UBTService_CheckPoweredUp()
{
    bCreateNodeInstance = true;
}

void UBTService_CheckPoweredUp::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());
    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetPlayerPoweredUpBBKeyID(), SDTUtils::IsPlayerPoweredUp(GetWorld()));
}
