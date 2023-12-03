// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_BoolCheck.generated.h"

/**
 *
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTDecorator_BoolCheck : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) const override;
};
