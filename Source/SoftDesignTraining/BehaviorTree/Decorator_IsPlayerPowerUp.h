// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "Decorator_IsPlayerPowerUp.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UDecorator_IsPlayerPowerUp : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	UDecorator_IsPlayerPowerUp(const FObjectInitializer& ObjectInitializer);

	EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;
	
protected:
	bool CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const override;
};
