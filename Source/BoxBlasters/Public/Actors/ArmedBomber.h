#pragma once

#include "CoreMinimal.h"
#include "Actors/Bomber.h"
#include "Actors/AirStrike.h"
#include "Actors/Bomb.h"
#include "ArmedBomber.generated.h"

UENUM(BlueprintType)
enum class ESpecialType : uint8
{
	None UMETA(DisplayName = "None"),
	Remote UMETA(DisplayName = "Remote"),
	Mine UMETA(DisplayName = "Mine"),
	Air UMETA(DisplayName = "Air Strike"),
};

UCLASS()
class BOXBLASTERS_API AArmedBomber : public ABomber
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABomb> NormalClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABomb> RemoteClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABomb> MineClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAirStrike> AirClass;
	UPROPERTY(BlueprintReadOnly)
	ESpecialType EquippedSpecial = ESpecialType::None;
	UPROPERTY(BlueprintReadOnly)
	ABomb* DeployedRemote;
	UPROPERTY(BlueprintReadOnly)
	AAirStrike* DeployedAirStrike;
	UFUNCTION(BlueprintCallable)
	bool PlaceBomb();
	UFUNCTION(BlueprintCallable)
	bool ActivateSpecial();
	UFUNCTION(BlueprintCallable)
	bool EquipSpecial(ESpecialType SpecialType);
	UFUNCTION(BlueprintCallable)
	bool MoveSpecial(FVector Movement);
};
