#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_ChooseFleeLocation.generated.h"

UCLASS()
class SOFTDESIGNTRAINING_API UServiceChooseFleeLocation : public UBTService
{
	GENERATED_BODY()

public:
	UServiceChooseFleeLocation();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
