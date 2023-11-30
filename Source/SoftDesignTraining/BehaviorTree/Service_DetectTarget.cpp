// Fill out your copyright notice in the Description page of Project Settings.


#include "Service_DetectTarget.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../SDTAIController.h"
#include "../SDTCollectible.h"
#include "../SDTUtils.h"



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

    ASDTAIController* aiController = Cast<ASDTAIController>(ownerComp.GetAIOwner());
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

    double beginSearchPlayer = FPlatformTime::Seconds();
    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * aiController->m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * aiController->m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_COLLECTIBLE));
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

    // Why this sweep goes through walls? Fix it in GetHightestPriorityDetectionHit for now.
    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(aiController->m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);
    double endSearchPlayer = FPlatformTime::Seconds();
    myBlackboard->SetValueAsFloat("TimeSpentFindPlayer", (endSearchPlayer - beginSearchPlayer) * 1000.0);

    // Retrieve the actual values of the blackboard to decide if we should activate the LKP or not.
    AActor* targetActor = Cast<AActor>(myBlackboard->GetValueAsObject(ChassingTargetKey.SelectedKeyName));
    auto followLKP = myBlackboard->GetValueAsBool(FollowLKPKey.SelectedKeyName);
    auto newHit = detectionHit.GetActor();
    auto newHitIsPlayer = Cast<ACharacter>(newHit) != nullptr;
    auto newHitIsCollectible= Cast<ASDTCollectible>(newHit)!=nullptr;

    // If we had a player in LOS, but it is not the case anymore, or if we don't see the player and we are already following the LKP, set the boolean to TRUE. Otherwise set it to false.
    myBlackboard->SetValueAsBool(FollowLKPKey.SelectedKeyName, !newHitIsPlayer && (targetActor != nullptr || followLKP));
    if (newHit != nullptr)
    {

        if (newHitIsPlayer)
        {
            
            if (SDTUtils::IsPlayerPoweredUp(GetWorld())) {
                myBlackboard->SetValueAsBool(FollowLKPKey.SelectedKeyName, false);
                //myBlackboard->SetValueAsObject(ChassingTargetKey.SelectedKeyName, nullptr);
                //myBlackboard->SetValueAsObject(ChassingCollectibleKey.SelectedKeyName, nullptr);
            }
            
            //else {
            
                myBlackboard->SetValueAsVector(LKPKey.SelectedKeyName, newHit->GetActorLocation());
                myBlackboard->SetValueAsObject(ChassingTargetKey.SelectedKeyName, newHit);
                myBlackboard->SetValueAsObject(ChassingCollectibleKey.SelectedKeyName, nullptr);
            //}
        }
        else if (newHitIsCollectible)
        {
            auto collectible = Cast<ASDTCollectible>(newHit);
            if (collectible->IsOnCooldown()) {
                myBlackboard->SetValueAsObject(ChassingTargetKey.SelectedKeyName, nullptr);
                myBlackboard->SetValueAsObject(ChassingCollectibleKey.SelectedKeyName, nullptr);
            }
            else {

                DrawDebugPoint(GetWorld(), newHit->GetActorLocation(), 40.0f, FColor::Red);
                myBlackboard->SetValueAsObject(ChassingTargetKey.SelectedKeyName, nullptr);
                myBlackboard->SetValueAsObject(ChassingCollectibleKey.SelectedKeyName, newHit);
            }

        }
        else
        {
            //If we don't see a collectible, there is no need to keep the previous targeted one.
            myBlackboard->SetValueAsObject(ChassingCollectibleKey.SelectedKeyName, nullptr);
        }
    }
    else
    {
        //If we don't see a collectible, there is no need to keep the previous targeted one.
        myBlackboard->SetValueAsObject(ChassingCollectibleKey.SelectedKeyName, nullptr);
    }
}
