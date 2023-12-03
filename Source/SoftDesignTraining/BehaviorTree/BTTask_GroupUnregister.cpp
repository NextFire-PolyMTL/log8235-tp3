// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_GroupUnregister.h"
#include "GroupManager.h"
#include "../BT_SDTAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

EBTNodeResult::Type UBTTask_GroupUnregister::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());
    auto groupManager = GroupManager::GetInstance();
    groupManager->UnregisterController(ctrl);
    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetShouldInvestigateBBKeyID(), false);
    return EBTNodeResult::Succeeded;
}
