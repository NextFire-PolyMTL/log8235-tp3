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
    ABT_SDTAIController();
    ~ABT_SDTAIController();

    void BeginPlay() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return m_behaviorTreeComponent; }
    UBlackboardComponent*   GetBlackBoardComponent() const { return m_blackboardComponent; }

    uint8                   GetPlayerPosBBKeyID() const { return m_playerPosBBKeyID; }
    uint8                   GetPlayerSeenKeyID() const { return m_isPlayerSeenBBKeyID; }
    uint8                   GetCollectiblePosBBKeyID() const { return m_collectiblePosBBKeyID; }
    uint8                   GetCollectibleSeenKeyID() const { return m_isCollectibleSeenBBKeyID; }
    uint8                   GetFleePosBBKeyID() const { return m_FleePosBBKeyID; }
    uint8                   GetIsPlayerPowerUpKeyID() const { return m_isPlayerPowerUpBBKeyID; }

    void                    StartBehaviorTree(APawn* pawn);
    void                    StopBehaviorTree(APawn* pawn);

    void Tick(float deltaTime) override;

    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
    virtual void OnPossess(APawn* pawn) override;

private:
    UPROPERTY(transient)
    UBehaviorTreeComponent* m_behaviorTreeComponent;

    UPROPERTY(transient)
    UBlackboardComponent* m_blackboardComponent;

    uint8   m_playerPosBBKeyID;
    uint8   m_isPlayerSeenBBKeyID;
    uint8   m_collectiblePosBBKeyID;
    uint8   m_isCollectibleSeenBBKeyID;
    uint8   m_FleePosBBKeyID;
    uint8   m_isPlayerPowerUpBBKeyID;

    FBlackboard::FKey TimeSpentFindPlayer;
    FBlackboard::FKey TimeSpentFindCollectible;
    FBlackboard::FKey TimeSpentFindFleePoint;
};
