// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BTService.h"
#include "Service_DetectTarget.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UDetectTarget : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector ChassingTargetKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector LKPKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector ChassingCollectibleKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector FollowLKPKey;

public:
	UDetectTarget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickNode(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds) override;
};
