#pragma once

#include "CoreMinimal.h"
#include "CommonUtils.h"
#include "GameFramework/Actor.h"
#include "Arena.generated.h"

UCLASS()
class BOXBLASTERS_API AArena : public AActor
{
	GENERATED_BODY()
	UPROPERTY()
	UInstancedStaticMeshComponent* BaseMeshInstance;
	
public:	
	AArena();
	UPROPERTY(EditAnywhere)
	UStaticMesh* BaseMesh;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transform) override;
	
public:
};
