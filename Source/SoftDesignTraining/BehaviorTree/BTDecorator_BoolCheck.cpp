// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_BoolCheck.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UBTDecorator_BoolCheck::CalculateRawConditionValue(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) const
{
    return OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
}
