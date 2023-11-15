// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SDTAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BT_SDTAIController.generated.h"

/**
 *
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ABT_SDTAIController : public ASDTAIController
{
	GENERATED_BODY()

public:
	ABT_SDTAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

protected:
	UPROPERTY(EditAnywhere, category = Behavior)
	UBehaviorTree *m_aiBehaviorTree;

private:
	UPROPERTY(transient)
	UBehaviorTreeComponent *m_behaviorTreeComponent;

	UPROPERTY(transient)
	UBlackboardComponent *m_blackboardComponent;
};
