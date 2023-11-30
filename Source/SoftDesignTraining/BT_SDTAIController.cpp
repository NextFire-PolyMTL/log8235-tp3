// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_SDTAIController.h"
#include "SoftDesignTrainingCharacter.h"
#include "SDTAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


ABT_SDTAIController::ABT_SDTAIController()
    : m_isPlayerSeenBBKeyID(false),
      m_isCollectibleSeenBBKeyID(false),
      m_isPlayerPowerUpBBKeyID(false)
{
	m_behaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	m_blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

ABT_SDTAIController::~ABT_SDTAIController()
{

}

void ABT_SDTAIController::BeginPlay()
{
    Super::BeginPlay();
    StartBehaviorTree(GetPawn());
}

void ABT_SDTAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    StopBehaviorTree(GetPawn());
}

void ABT_SDTAIController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    auto world = GetWorld();
    auto loc = GetCharacter()->GetActorLocation();
    DrawDebugString(world, loc - FVector::UpVector * 100, FString::Printf(TEXT("Player: %.3fms"), m_blackboardComponent->GetValue<UBlackboardKeyType_Float>(TimeSpentFindPlayer)), nullptr, FColor::Purple, 0.0f, true);
    DrawDebugString(world, loc - FVector::UpVector * 200, FString::Printf(TEXT("Flee: %.3fms"), m_blackboardComponent->GetValue<UBlackboardKeyType_Float>(TimeSpentFindFleePoint)), nullptr, FColor::Purple, 0.0f, true);
    DrawDebugString(world, loc - FVector::UpVector * 300, FString::Printf(TEXT("Collectible: %.3fms"), m_blackboardComponent->GetValue<UBlackboardKeyType_Float>(TimeSpentFindCollectible)), nullptr, FColor::Purple, 0.0f, true);
}

void ABT_SDTAIController::StartBehaviorTree(APawn* pawn)
{
    if (ASoftDesignTrainingCharacter* aiBaseCharacter = Cast<ASoftDesignTrainingCharacter>(pawn))
    {
        if (aiBaseCharacter->GetBehaviorTree())
        {
            m_behaviorTreeComponent->StartTree(*aiBaseCharacter->GetBehaviorTree());
        }
    }
}

void ABT_SDTAIController::StopBehaviorTree(APawn* pawn)
{
    if (ASoftDesignTrainingCharacter* aiBaseCharacter = Cast<ASoftDesignTrainingCharacter>(pawn))
    {
        if (aiBaseCharacter->GetBehaviorTree())
        {
            m_behaviorTreeComponent->StopTree();
        }
    }
}

void ABT_SDTAIController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    if (ASoftDesignTrainingCharacter* aiBaseCharacter = Cast<ASoftDesignTrainingCharacter>(pawn))
    {
        if (aiBaseCharacter->GetBehaviorTree())
        {
            m_blackboardComponent->InitializeBlackboard(*aiBaseCharacter->GetBehaviorTree()->BlackboardAsset);

            m_playerPosBBKeyID = m_blackboardComponent->GetKeyID("PlayerPos");
            m_isPlayerSeenBBKeyID = m_blackboardComponent->GetKeyID("PlayerIsSeen");
            m_collectiblePosBBKeyID = m_blackboardComponent->GetKeyID("CollectiblePos");
            m_isCollectibleSeenBBKeyID = m_blackboardComponent->GetKeyID("CollectibleIsSeen");
            m_FleePosBBKeyID = m_blackboardComponent->GetKeyID("FleePos");
            m_isPlayerPowerUpBBKeyID = m_blackboardComponent->GetKeyID("PlayerIsPowerUp");

            // Set this agent in the BT
            m_blackboardComponent->SetValue<UBlackboardKeyType_Object>(m_blackboardComponent->GetKeyID("SelfActor"), pawn);

            TimeSpentFindPlayer = m_blackboardComponent->GetKeyID("TimeSpentFindPlayer");
            TimeSpentFindCollectible = m_blackboardComponent->GetKeyID("TimeSpentFindCollectible");
            TimeSpentFindFleePoint = m_blackboardComponent->GetKeyID("TimeSpentFindFleePoint");
        }
    }
}

void ABT_SDTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    m_blackboardComponent->SetValueAsBool("FollowLKP", false);
}
