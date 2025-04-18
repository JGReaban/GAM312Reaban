// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAM312REABAN_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	//  Function to Update Player HUD bars
	//  
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBars(float Health1, float Hunger1, float Stamina1);

	
};
