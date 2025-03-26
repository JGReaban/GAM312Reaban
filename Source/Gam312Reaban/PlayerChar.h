// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Resource_M.h"
#include "Kismet/GameplayStatics.h"
#include "BuildingPart.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerWidget.h"
#include "PlayerChar.generated.h"

UCLASS()
class GAM312REABAN_API APlayerChar : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	APlayerChar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Player Character Functions

	UFUNCTION()
	void MoveForward(float axisValue);

	UFUNCTION()
	void MoveRight(float axisValue);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void FindObject();

	// Player Character Camera.

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* PlayerCamComp;

	// Player Stats

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Hunger = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina = 100.0f;

	// Resource Variables 

	UPROPERTY(EditAnywhere, Category = "Resources")
	int Wood;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int Stone;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int Berry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<int> ResourcesArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<FString> ResourcesNameArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitMarker")
	UMaterialInterface* hitDecal;

	// Building related variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Supplies")
	TArray<int> BuildingArray;

	UPROPERTY()
	bool isBuilding;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABuildingPart> BuildPartClass;

	UPROPERTY()
	ABuildingPart* spawnedPart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayerWidget* playerUI;


	// Setters for Player Stats

	UFUNCTION(BlueprintCallable)
	void SetHunger(float amount);

	UFUNCTION(BlueprintCallable)
	void SetHealth(float amount);

	UFUNCTION(BlueprintCallable)
	void SetStamina(float amount);

	// Reduce Stats

	UFUNCTION()
	void DecreaseStats();

	// Give Resources
	UFUNCTION()
	void GiveResource(float amount, FString resourceType);

	// Functions for Buildings

	UFUNCTION(BlueprintCallable)
	void UpdateResources(float woodAmount, float stoneAmount, FString resourceType);

	UFUNCTION(BlueprintCallable)
	void SpawnBuilding(int buildingID, bool& isSuccess);

	UFUNCTION()
	void RotateBuilding();

	UFUNCTION()
	void LowerBuilding();

	UFUNCTION()
	void RaiseBuilding();


};
