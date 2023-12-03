// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_SDTAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/GroupManager.h"

ABT_SDTAIController::ABT_SDTAIController(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer),
      m_playerPositionBBKeyID(0),
      m_isPlayerDetectedBBKeyID(0),
      m_lastSeenBBKeyID(0),
      m_isPlayerPoweredUpBBKeyID(0),
      m_fleePositionBBKeyID(0),
      m_collectiblePositionBBKeyID(0),
      m_shouldInvestigateBBKeyID(0),
      m_groupTargetPositionBBKeyID(0)
{
    m_blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    m_behaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void ABT_SDTAIController::BeginPlay()
{
    AAIController::BeginPlay();

    m_blackboardComponent->InitializeBlackboard(*(m_aiBehaviorTree->BlackboardAsset));

    m_playerPositionBBKeyID = m_blackboardComponent->GetKeyID("PlayerPosition");
    m_isPlayerDetectedBBKeyID = m_blackboardComponent->GetKeyID("PlayerDetected");
    m_lastSeenBBKeyID = m_blackboardComponent->GetKeyID("LastSeen");

    m_isPlayerPoweredUpBBKeyID = m_blackboardComponent->GetKeyID("PlayerPoweredUp");
    m_fleePositionBBKeyID = m_blackboardComponent->GetKeyID("FleePosition");

    m_collectiblePositionBBKeyID = m_blackboardComponent->GetKeyID("CollectiblePosition");

    m_shouldInvestigateBBKeyID = m_blackboardComponent->GetKeyID("ShouldInvestigate");
    m_groupTargetPositionBBKeyID = m_blackboardComponent->GetKeyID("GroupTargetPosition");

    // Set this agent in the BT
    m_blackboardComponent->SetValue<UBlackboardKeyType_Object>(m_blackboardComponent->GetKeyID("SelfActor"), GetPawn());

    m_behaviorTreeComponent->StartTree(*m_aiBehaviorTree);
}

void ABT_SDTAIController::Tick(float deltaTime)
{
    AAIController::Tick(deltaTime);

    auto groupManager = GroupManager::GetInstance();
    auto controllers = groupManager->GetRegisteredControllers();
    if (controllers.Contains(this))
    {
        DrawDebugSphere(GetWorld(), GetPawn()->GetActorLocation() + FVector(0.f, 0.f, 100.f), 30.0f, 32, FColor::Orange);
    }
}
