// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "Decorator_IsCollectibleCooldown.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UDecorator_IsCollectibleCooldown : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	UDecorator_IsCollectibleCooldown(const FObjectInitializer& ObjectInitializer);

	EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;

protected:
	bool CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const override;
	
};