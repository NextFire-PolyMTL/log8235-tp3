// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BT_SDTAIController.h"
#include "AgentBudgetManager.generated.h"

UCLASS()
class SOFTDESIGNTRAINING_API AAgentBudgetManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AAgentBudgetManager();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RegisterAgent(ABT_SDTAIController* agent);
	void UnregisterAgent(ABT_SDTAIController* agent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Budget", meta = (ClampMin = 0.0))
	float TimeBudget = 1.0f / 120.0f;

private:
	TArray<ABT_SDTAIController*> RegisteredAgents;
	int CurrentIdx = 0;
	//TArray<ABT_SDTAIController*>::TConstIterator CurrentAgentExecution;
};
