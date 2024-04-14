#pragma once

#include "CoreMinimal.h"
#include "Area.generated.h"

UCLASS()
class BOXBLASTERS_API AArea : public AActor
{
	GENERATED_BODY()
public:
	AArea(): Warning(false), Blocked(false) {};
	void SetWarning(const bool InWarning)
	{
		Warning = InWarning;
	}
	void SetBlocked(const bool InBlocked)
	{
		Blocked = InBlocked;
	}
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Warning;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Blocked;
};
