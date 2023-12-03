// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_SDTAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/GroupManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AgentBudgetManager.h"
#include "SDTUtils.h"

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
    TimeSpentFindPlayer = m_blackboardComponent->GetKeyID("TimeSpentFindPlayer");
    TimeSpentFindCollectible = m_blackboardComponent->GetKeyID("TimeSpentFindCollectible");
    TimeSpentFindFleePoint = m_blackboardComponent->GetKeyID("TimeSpentFindFleePoint");

    m_behaviorTreeComponent->StartTree(*m_aiBehaviorTree);

    BudgetManager = Cast<AAgentBudgetManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAgentBudgetManager::StaticClass()));
    if (BudgetManager != nullptr)
    {
        BudgetManager->RegisterAgent(this);
    }
}

void ABT_SDTAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (BudgetManager != nullptr)
    {
        BudgetManager->UnregisterAgent(this);
    }
}

void ABT_SDTAIController::Tick(float deltaTime)
{
    AAIController::Tick(deltaTime);

    auto groupManager = GroupManager::GetInstance();
    auto controllers = groupManager->GetRegisteredControllers();
    auto world = GetWorld();
    auto loc = GetPawn()->GetActorLocation();
    DrawDebugString(world, loc - FVector::UpVector * 100, FString::Printf(TEXT("Player: %.3fms\nFlee: %.3fms\nCollectible: %.3fms"),
        m_blackboardComponent->GetValue<UBlackboardKeyType_Float>(TimeSpentFindPlayer),
        m_blackboardComponent->GetValue<UBlackboardKeyType_Float>(TimeSpentFindFleePoint),
        m_blackboardComponent->GetValue<UBlackboardKeyType_Float>(TimeSpentFindCollectible)), nullptr, FColor::Purple, 0.0f, true);
    if (controllers.Contains(this))
    {
        DrawDebugSphere(world, loc + FVector(0.f, 0.f, 100.f), 30.0f, 32, FColor::Orange);
    }
}

void ABT_SDTAIController::UpdateAgent()
{
    double timeElapsed = SDTUtils::MeasureExecutionTime(&ABT_SDTAIController::PerformDetectPlayer, this);
    m_blackboardComponent->SetValueAsFloat("TimeSpentFindPlayer", timeElapsed);
    if (m_blackboardComponent->GetValue<UBlackboardKeyType_Bool>(m_isPlayerDetectedBBKeyID) && m_blackboardComponent->GetValue<UBlackboardKeyType_Bool>(m_shouldInvestigateBBKeyID))
    {
        auto playerPoweredUp = SDTUtils::IsPlayerPoweredUp(GetWorld());
        m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_isPlayerPoweredUpBBKeyID, playerPoweredUp);

        if (!playerPoweredUp && !m_blackboardComponent->GetValue<UBlackboardKeyType_Bool>(m_isPlayerDetectedBBKeyID))
        {
            CalculateGroupTargetPosition();
        }
    }

}

void ABT_SDTAIController::PerformDetectPlayer()
{
    auto selfPawn = GetPawn();

    auto movcomp = GetCharacter()->GetMovementComponent();
    if (movcomp->IsFlying() || movcomp->IsFalling())
    {
        return;
    }

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(m_DetectionCapsuleRadius));

    DrawDebugCapsule(GetWorld(), detectionStartLocation + m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), m_DetectionCapsuleHalfLength, m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    auto playerDetected = detectionHit.GetComponent() && (detectionHit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER);

    if (playerDetected)
    {
        // Direct raycast to check for walls
        FHitResult hit;
        GetWorld()->LineTraceSingleByChannel(hit, detectionStartLocation, detectionHit.ImpactPoint, ECollisionChannel::ECC_WorldStatic);
        playerDetected = hit.GetComponent() && (hit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER);
    }

    if (playerDetected)
    {
        DrawDebugLine(GetWorld(), detectionStartLocation, detectionHit.ImpactPoint, FColor::Red);
        m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(GetPlayerDetectedBBKeyID(), true);
        m_blackboardComponent->SetValue<UBlackboardKeyType_Vector>(GetPlayerPositionBBKeyID(), detectionHit.GetActor()->GetActorLocation());
        m_blackboardComponent->SetValue<UBlackboardKeyType_Float>(GetLastSeenBBKeyID(), UGameplayStatics::GetRealTimeSeconds(GetWorld()));
    }
    else
    {
        m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(GetPlayerDetectedBBKeyID(), false);
    }
}

void ABT_SDTAIController::CalculateGroupTargetPosition()
{
    auto groupManager = GroupManager::GetInstance();

    m_blackboardComponent->SetValue<UBlackboardKeyType_Bool>(m_shouldInvestigateBBKeyID, groupManager->ShouldInvestigate());

    auto playerFound = false;
    auto playerLKP = groupManager->GetLKPFromGroup(playerFound);

    if (!playerFound)
    {
        return; // FIXME: hmmmm
    }

    DrawDebugSphere(GetWorld(), playerLKP, 30.0f, 32, FColor::Green);

    FVector targetPos;
    if ((playerLKP - GetPawn()->GetActorLocation()).Size2D() > m_DetectionCapsuleRadius)
    {
        // surround player
        auto controllers = groupManager->GetRegisteredControllers();
        auto numControllers = controllers.Num();
        auto currentControllerIndex = controllers.Find(this);
        auto radius = m_DetectionCapsuleRadius;
        auto angle = 2 * PI * currentControllerIndex / numControllers;
        targetPos = playerLKP + FVector(radius * cos(angle), radius * sin(angle), 0.0f);
    }
    else
    {
        // just attack
        targetPos = playerLKP;
    }

    m_blackboardComponent->SetValue<UBlackboardKeyType_Vector>(m_groupTargetPositionBBKeyID, targetPos);
    DrawDebugSphere(GetWorld(), targetPos, 30.0f, 32, FColor::Orange);
    DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), targetPos, FColor::Orange);
}
