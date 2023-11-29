// Fill out your copyright notice in the Description page of Project Settings.


#include "Service_DetectTarget.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../BT_SDTAIController.h"
#include "../SDTCollectible.h"
#include "../SDTUtils.h"
#include "GroupManager.h"



UDetectTarget::UDetectTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    ChassingTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDetectTarget, ChassingTargetKey), AActor::StaticClass());
    LKPKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UDetectTarget, LKPKey));
    ChassingCollectibleKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDetectTarget, ChassingCollectibleKey), AActor::StaticClass());
    FollowLKPKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UDetectTarget, FollowLKPKey));
}

void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit)
{
    for (const FHitResult& hit : hits)
    {
        if (UPrimitiveComponent* component = hit.GetComponent())
        {
            if (component->GetCollisionObjectType() == COLLISION_PLAYER)
            {
                // We can't get more important than the player
                outDetectionHit = hit;
                return;
            }
            else if (component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
            {
                outDetectionHit = hit;
            }
            else if (component->GetCollisionObjectType() == ECC_WorldStatic)
            {
                // Don't look through walls.
                return;
            }
        }
    }
}

void LookForBestTarget(const APawn* selfPawn, const ASDTAIController* aiController, FHitResult& detectionHit)
{
    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * aiController->m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * aiController->m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_COLLECTIBLE));
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

    // Why this sweep goes through walls? Fix it in GetHightestPriorityDetectionHit for now.
    TArray<FHitResult> allDetectionHits;
    selfPawn->GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(aiController->m_DetectionCapsuleRadius));

    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);
}

void UDetectTarget::TickNode(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
    /*
    check(ChassingTargetKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass());
    check(LKPKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass());
    check(ChassingCollectibleKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass());
    check(FollowLKPKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass());
    */

    UBlackboardComponent* myBlackboard = ownerComp.GetBlackboardComponent();
    if (myBlackboard == nullptr)
    {
        return;
    }

    ABT_SDTAIController* aiController = Cast<ABT_SDTAIController>(ownerComp.GetAIOwner());
    if (aiController == nullptr || aiController->AtJumpSegment)
    {
        return;
    }

    const APawn* selfPawn = aiController->GetPawn();
    if (selfPawn == nullptr)
    {
        return;
    }

    const ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (playerCharacter == nullptr)
    {
        return;
    }

    FHitResult detectionHit;
    double executionTime = SDTUtils::MeasureExecutionTime(&LookForBestTarget, selfPawn, aiController, detectionHit);
    myBlackboard->SetValueAsFloat("TimeSpentFindPlayer", executionTime);

    auto newHit = detectionHit.GetActor();
    auto newHitIsPlayer = Cast<ACharacter>(newHit) != nullptr;
    auto newHitIsCollectible= Cast<ASDTCollectible>(newHit)!=nullptr;

    FVector LKPPos;
    auto followLKP = false;

    // Group logic
    auto groupManager = GroupManager::GetInstance();
    if (aiController->m_isGroupRegistered)
    {
        auto targetFoundByGroup = false;
        auto targetInfoFromGroup = groupManager->GetLKPFromGroup(targetFoundByGroup);
        if (targetFoundByGroup)
        {
            LKPPos = targetInfoFromGroup.GetLKPPos();
            followLKP = !targetInfoFromGroup.IsPoweredUp();
        }
        else
        {
            groupManager->UnregisterController(aiController);
            aiController->m_isGroupRegistered = false;
        }
    }

    float currentElapsedTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    AActor *chassingTarget = nullptr;
    AActor *chassingCollectible = nullptr;

    if (newHit != nullptr)
    {
        if (newHitIsPlayer)
        {
            LKPPos = newHit->GetActorLocation();
            followLKP = true;

            auto isPoweredUp = SDTUtils::IsPlayerPoweredUp(GetWorld());

            // Update LKP
            aiController->m_currentTargetLkpInfo.SetLKPState(TargetLKPInfo::ELKPState::LKPState_ValidByLOS);
            aiController->m_currentTargetLkpInfo.SetLKPPos(LKPPos);
            aiController->m_currentTargetLkpInfo.SetPoweredUp(isPoweredUp);
            aiController->m_currentTargetLkpInfo.SetLastUpdatedTimeStamp(currentElapsedTime);

            if (!aiController->m_isGroupRegistered)
            {
                groupManager->RegisterController(aiController);
                aiController->m_isGroupRegistered = true;
            }

            chassingTarget = isPoweredUp ? nullptr : newHit;
        }
        else if (!followLKP && newHitIsCollectible)
        {
            auto collectible = Cast<ASDTCollectible>(newHit);
            auto onCooldown = collectible->IsOnCooldown();

            chassingCollectible = onCooldown ? nullptr : newHit;
        }
    }
    else
    {
        // Lost sight of the target
        if (aiController->m_currentTargetLkpInfo.GetLKPState() == TargetLKPInfo::ELKPState::LKPState_ValidByLOS)
        {
            aiController->m_currentTargetLkpInfo.SetLKPState(TargetLKPInfo::ELKPState::LKPState_Valid);
            aiController->m_currentTargetLkpInfo.SetLastUpdatedTimeStamp(currentElapsedTime);
        }

        if ((selfPawn->GetActorLocation() - LKPPos).Size2D() < 50.f)
        {
            followLKP = false;
            aiController->m_currentTargetLkpInfo.SetLKPState(TargetLKPInfo::ELKPState::LKPState_Invalid);
            aiController->m_currentTargetLkpInfo.SetLastUpdatedTimeStamp(currentElapsedTime);
        }
    }

    myBlackboard->SetValueAsObject(ChassingTargetKey.SelectedKeyName, chassingTarget);
    myBlackboard->SetValueAsObject(ChassingCollectibleKey.SelectedKeyName, chassingCollectible);

    if (aiController->m_isGroupRegistered)
    {
        DrawDebugSphere(GetWorld(), selfPawn->GetActorLocation() + FVector(0.f, 0.f, 100.f), 30.0f, 32, FColor::Orange);
    }
    if (followLKP)
    {
        DrawDebugSphere(GetWorld(), LKPPos, 30.0f, 32, FColor::Green);
        DrawDebugLine(GetWorld(), selfPawn->GetActorLocation(), LKPPos, FColor::Green);
    }

    myBlackboard->SetValueAsVector(LKPKey.SelectedKeyName, LKPPos);
    myBlackboard->SetValueAsBool(FollowLKPKey.SelectedKeyName, followLKP);
}
