// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_SDTAIController.h"

ABT_SDTAIController::ABT_SDTAIController()
{
    m_behaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    m_blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ABT_SDTAIController::BeginPlay()
{
    AAIController::BeginPlay();

    m_blackboardComponent->InitializeBlackboard(*(m_aiBehaviorTree->BlackboardAsset));
    m_behaviorTreeComponent->StartTree(*m_aiBehaviorTree);
}

void ABT_SDTAIController::Tick(float deltaTime)
{
    AAIController::Tick(deltaTime);
}
