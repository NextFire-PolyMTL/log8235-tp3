// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_EndInvestigations.h"
#include "GroupManager.h"
#include "../BT_SDTAIController.h"

EBTNodeResult::Type UBTTask_EndInvestigations::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
    auto groupManager = GroupManager::GetInstance();
    groupManager->SetShouldInvestigate(false);
    return EBTNodeResult::Succeeded;
}
