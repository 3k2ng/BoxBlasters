#pragma once

#include "CoreMinimal.h"
#include "Actors/Area.h"
#include "Libraries/CommonUtils.h"
#include "GameFramework/Actor.h"
#include "Arena.generated.h"

UENUM()
enum class EStationaryBombType : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Remote UMETA(DisplayName = "Remote"),
	Mine UMETA(DisplayName = "Mine"),
};

UCLASS()
class BOXBLASTERS_API AArena : public AActor
{
	GENERATED_BODY()
	UPROPERTY()
	UInstancedStaticMeshComponent* BaseComponent;
	
public:	
	AArena();
	UPROPERTY(EditAnywhere)
	UStaticMesh* BaseMesh;
	UPROPERTY()
	TArray<AArea*> AreaMap;
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transform) override;
	
public:
	
};
