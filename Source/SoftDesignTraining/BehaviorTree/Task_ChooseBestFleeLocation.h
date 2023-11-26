#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Task_ChooseBestFleeLocation.generated.h"

UCLASS()
class SOFTDESIGNTRAINING_API UChooseBestFleeLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UChooseBestFleeLocation(const FObjectInitializer& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) override;
};
