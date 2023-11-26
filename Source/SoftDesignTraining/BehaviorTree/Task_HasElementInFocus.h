// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_HasElementInFocus.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UHasElementInFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(Category = Config, EditAnywhere)
	bool MustBeCharacterActor = true;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) override;
};
