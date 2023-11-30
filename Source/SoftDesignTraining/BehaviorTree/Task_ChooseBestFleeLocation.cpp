#include "Task_ChooseBestFleeLocation.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "../SDTAIController.h"
#include "../SDTFleeLocation.h"

UChooseBestFleeLocation::UChooseBestFleeLocation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    NodeName = "Choose Flee Location";
    BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UChooseBestFleeLocation, BlackboardKey));
}

EBTNodeResult::Type UChooseBestFleeLocation::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
    UBlackboardComponent* blackboard = ownerComp.GetBlackboardComponent();
    if (blackboard == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    auto aiController = ownerComp.GetAIOwner();
    if (aiController == nullptr)
    {
        return EBTNodeResult::Failed;
    }
    auto aiLocation = aiController->GetPawn()->GetActorLocation();

    auto playerCharacter = aiController->GetFocusActor();
    if (playerCharacter == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    if (BlackboardKey.SelectedKeyType != UBlackboardKeyType_Vector::StaticClass())
    {
        return EBTNodeResult::Failed;
    }

    double beginSearchFleePoint = FPlatformTime::Seconds();
    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    for (TActorIterator<ASDTFleeLocation> actorIterator(GetWorld(), ASDTFleeLocation::StaticClass()); actorIterator; ++actorIterator)
    {
        ASDTFleeLocation* fleeLocation = Cast<ASDTFleeLocation>(*actorIterator);
        if (fleeLocation != nullptr)
        {
            float distToFleeLocation = FVector::Dist(fleeLocation->GetActorLocation(), playerCharacter->GetActorLocation());

            FVector selfToPlayer = playerCharacter->GetActorLocation() - aiLocation;
            selfToPlayer.Normalize();

            FVector selfToFleeLocation = fleeLocation->GetActorLocation() - aiLocation;
            selfToFleeLocation.Normalize();

            float fleeLocationToPlayerAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(selfToPlayer, selfToFleeLocation)));
            float locationScore = distToFleeLocation + fleeLocationToPlayerAngle * 100.f;

            if (locationScore > bestLocationScore)
            {
                bestLocationScore = locationScore;
                bestFleeLocation = fleeLocation;
            }

            DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
        }
    }
    double endSearchFleePoint = FPlatformTime::Seconds();
    blackboard->SetValueAsFloat("TimeSpentFindFleePoint", (endSearchFleePoint - beginSearchFleePoint) * 1000.0);

    if (bestFleeLocation != nullptr)
    {
        blackboard->SetValueAsVector(BlackboardKey.SelectedKeyName, bestFleeLocation->GetActorLocation());
        //aiController->MoveToLocation(bestFleeLocation->GetActorLocation(), 0.5f, false, true, false, NULL, false);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        return EBTNodeResult::Failed;
    }
}
