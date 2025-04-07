// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChar.h"

// Sets default values
APlayerChar::APlayerChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create FPS Camera
	PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));

	// Attach To Head
	PlayerCamComp->SetupAttachment(GetMesh(), "head");

	PlayerCamComp->bUsePawnControlRotation = true;

	// Set Resources Stuff
	ResourcesArray.SetNum(3);

	ResourcesNameArray.Add(TEXT("Wood"));
	ResourcesNameArray.Add(TEXT("Stone"));
	ResourcesNameArray.Add(TEXT("Berry"));

	// Set Building Stuff
	BuildingArray.SetNum(3);



}

// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	Super::BeginPlay();
	// Start Timer
	FTimerHandle StatsTimerHandle;
	// Call Decrease Stats Function every 2 seconds
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerChar::DecreaseStats, 2.0f, true);
	
	// Objective Widget Stuff

	if (objectiveWidget)
	{
		objectiveWidget->UpdateBuildingObjectives(0.0f);
		objectiveWidget->UpdateMaterialObjectives(0.0f);
	}
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	playerUI->UpdateBars(Health, Hunger, Stamina);

	// Building Routine
	if (isBuilding) 
	{
		if (spawnedPart) {
			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			spawnedPart->SetActorLocation(EndLocation);

		}
	}

}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up Axis Inputs and bind them to appropriate functions
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerChar::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerChar::AddControllerYawInput);

	// Set up Action Inputs and bind them to appropriate functions
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerChar::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerChar::StopJump);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::FindObject);

	PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &APlayerChar::RotateBuilding);

	PlayerInputComponent->BindAction("LowerPart", IE_Pressed, this, &APlayerChar::LowerBuilding);

	PlayerInputComponent->BindAction("RaisePart", IE_Pressed, this, &APlayerChar::RaiseBuilding);

	PlayerInputComponent->BindAction("EatBerry", IE_Pressed, this, &APlayerChar::EatABerry);

}
// 
// Move Player Forwards or Back depending on input
//
void APlayerChar::MoveForward(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, axisValue);
}
//
// Move Player Left or Right depending on input
//
void APlayerChar::MoveRight(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, axisValue);
}
//
//  Start Jump
//
void APlayerChar::StartJump()
{
	bPressedJump = true;
}
//
// Stop Jump
//
void APlayerChar::StopJump()
{
	bPressedJump = false;
}

void APlayerChar::FindObject()
{
	FHitResult HitResult;
	FVector StartLocation = PlayerCamComp->GetComponentLocation();
	FVector Direction = PlayerCamComp->GetForwardVector() * 800;
	FVector EndLocation = StartLocation + Direction;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Blue, TEXT("Finding Object!!!"));


	// Do actual line trace 

	if (!isBuilding) {
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
		{
			AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());

			// Check if player has enough stamina to gather resource
			if (Stamina >= -5.0f)
			{
				// Check if it hits a resource
				if (HitResource)
				{
					FString hitName = HitResource->ResourceName;
					int resourceAmount = HitResource->resourceAmount;

					HitResource->totalResources = HitResource->totalResources - resourceAmount;

					if (HitResource->totalResources >= resourceAmount)
					{
						GiveResource(resourceAmount, hitName);

						matsCollected = matsCollected + resourceAmount;

						objectiveWidget->UpdateMaterialObjectives(matsCollected);

						check(GEngine != nullptr);
						GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("Resource Collected!!!"));

						// Spawn Decal on Resource
						UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);

						// Decrease Stamina since gathering is hard work
						SetStamina(-5.0f);
					}
					else
					{
						HitResource->Destroy();
						check(GEngine != nullptr);
						GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("Resource Exhausted!!!"));

					}

				}

			}
		}
	}
	else
	{
		// This is called when placing building element
		isBuilding = false;

		// Keeps track of how many objects built
		objectsBuilt = objectsBuilt + 1.0f;

		// Updates objective widget
		objectiveWidget->UpdateBuildingObjectives(objectsBuilt);


	}

	

}

void APlayerChar::SetHunger(float amount)
{
	if (Hunger + amount < 100)
	{
		Hunger = Hunger + amount;
	}
	if (Hunger + amount >= 100)
	{
		Hunger = 100;
	}
}

void APlayerChar::SetHealth(float amount)
{
	if (Health + amount < 100)
	{
		Health = Health + amount;
	}
	if (Health + amount >= 100)
	{
		Health = 100;
	}
}

void APlayerChar::SetStamina(float amount)
{
	if (Stamina + amount < 100)
	{
		Stamina = Stamina + amount;
	}
	if (Stamina + amount >= 100)
	{
		Stamina = 100;
	}
}

void APlayerChar::DecreaseStats()
{
	if (Hunger > 0)			// If not starving
	{
		SetHunger(-1.0f);	// Player gets hungry
		SetStamina(10.0f);	// Regenerate Stamina
	}
	if (Hunger <= 0)	// If starving reduce health
	{
		SetHealth(-3.0f);
	}
}

void APlayerChar::GiveResource(float amount, FString resourceType)
{
	if (resourceType == "Wood")
	{
		ResourcesArray[0] = ResourcesArray[0] + amount;
	}
	if (resourceType == "Stone")
	{
		ResourcesArray[1] = ResourcesArray[1] + amount;
	}
	if (resourceType == "Berry")
	{
		ResourcesArray[2] = ResourcesArray[2] + amount;
	}


}

void APlayerChar::UpdateResources(float woodAmount, float stoneAmount, FString buildingObject)
{
	if (woodAmount <= ResourcesArray[0])
	{
		if (stoneAmount <= ResourcesArray[1])
		{
			ResourcesArray[0] = ResourcesArray[0] - woodAmount;
			ResourcesArray[1] = ResourcesArray[1] - stoneAmount;

			if (buildingObject == "Wall")
			{
				BuildingArray[0] = BuildingArray[0] + 1;
			}
			if (buildingObject == "Floor")
			{
				BuildingArray[1] = BuildingArray[1] + 1;
			}
			if (buildingObject == "Ceiling")
			{
				BuildingArray[2] = BuildingArray[2] + 1;
			}
		}
	}
}

void APlayerChar::SpawnBuilding(int buildingID, bool& isSuccess)
{
	if (!isBuilding)
	{
		if (BuildingArray[buildingID] >= 1)
		{
			isBuilding = true;
			FActorSpawnParameters SpawnParams;
			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			FRotator myRot(0, 0, 0);

			BuildingArray[buildingID] = BuildingArray[buildingID] - 1;

			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);

			isSuccess = true;


		}
		else {
			isSuccess = false;
		}
	}
}

void APlayerChar::RotateBuilding()
{
	if (isBuilding)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0));
	}
}

void APlayerChar::LowerBuilding()
{
	if (isBuilding)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 0, -5));
	}
}

void APlayerChar::RaiseBuilding()
{
	if (isBuilding)
	{
		//spawnedPart->AddActorWorldRotation(FRotator(0, 0, 5));
		FVector StartLocation = spawnedPart->GetActorLocation();
		FVector Direction = FVector(0.0f, 0.0f, 100.2f);
		FVector EndLocation = Direction + StartLocation;
		FRotator myRot(0, 0, 0);
		FVector myScale = FVector(1.0f, 1.0f, 1.0f);
		FTransform myTransform = UKismetMathLibrary::MakeTransform(EndLocation, myRot, myScale);
		//spawnedPart->SetActorRelativeLocation(Direction);
		//spawnedPart->AddActorWorldTransform(myTransform);
		//spawnedPart->
		spawnedPart->SetActorLocation(Direction);
	}
}

void APlayerChar::EatABerry()
{
	// This eats a berry without having to use the menu
	
	// Check if the player has berries
	// Berries are stored in ResourcesArray[2]

	if (ResourcesArray[2] >= 1) 
	{
		SetStamina(50.0f);
		SetHunger(5.0f);
		SetHealth(5.0f);
		ResourcesArray[2] = ResourcesArray[2] - 1;
	}
}

