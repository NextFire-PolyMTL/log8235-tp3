// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_GroupRegister.h"
#include "GroupManager.h"
#include "../BT_SDTAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UBTTask_GroupRegister::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());
    auto groupManager = GroupManager::GetInstance();
    groupManager->RegisterController(ctrl);
    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(ctrl->GetShouldInvestigateBBKeyID(), true);
    return EBTNodeResult::Succeeded;
}
