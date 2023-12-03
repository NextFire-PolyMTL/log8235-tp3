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
	ABT_SDTAIController(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	uint8 GetPlayerPositionBBKeyID() const { return m_playerPositionBBKeyID; }
	uint8 GetPlayerDetectedBBKeyID() const { return m_isPlayerDetectedBBKeyID; }
	uint8 GetPlayerPoweredUpBBKeyID() const { return m_isPlayerPoweredUpBBKeyID; }
	uint8 GetFleePositionBBKeyID() const { return m_fleePositionBBKeyID; }
	uint8 GetCollectiblePositionBBKeyID() const { return m_collectiblePositionBBKeyID; }

protected:
	UPROPERTY(EditAnywhere, category = Behavior)
	UBehaviorTree *m_aiBehaviorTree;

private:
	UPROPERTY(transient)
	UBehaviorTreeComponent *m_behaviorTreeComponent;

	UPROPERTY(transient)
	UBlackboardComponent *m_blackboardComponent;

	uint8 m_playerPositionBBKeyID;
	uint8 m_isPlayerDetectedBBKeyID;
	uint8 m_isPlayerPoweredUpBBKeyID;
	uint8 m_fleePositionBBKeyID;
	uint8 m_collectiblePositionBBKeyID;
};
