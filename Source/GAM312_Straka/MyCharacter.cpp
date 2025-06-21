

#include "MyCharacter.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and attach a first-person camera to the character's root
	PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));
	PlayerCamComp->SetupAttachment(RootComponent);
	PlayerCamComp->bUsePawnControlRotation = true;

	// Initialize arrays for building parts and resources
	BuildingArray.SetNum(3);  // e.g., Wall, Floor, Ceiling
	ResourcesArray.SetNum(3); // e.g., Wood, Stone, Berry

	// Add names to identify resource types (used in GiveResource logic)
	ResourcesNameArray.Add(TEXT("Wood"));
	ResourcesNameArray.Add(TEXT("Stone"));
	ResourcesNameArray.Add(TEXT("Berry"));
}

// Called when the game starts or the actor is spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Start a timer that periodically decreases stats every 2 seconds
	FTimerHandle StatsTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &AMyCharacter::DecreaseStats, 2.0f, true);

	// Reset UI progress bars
	if (objWidget)
	{
		objWidget->UpdatebuildObj(0.0f);
		objWidget->UpdatematOBJ(0.0f);
	}

	// Sanity check: ensure the camera component is valid
	check(PlayerCamComp != nullptr);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update player HUD elements (e.g., health, hunger, stamina bars)
	playerUI->UpdateBars(Health, Hunger, Stamina);

	// Move the spawned buildable part in front of the camera if in build mode
	if (isBuilding && spawnedPart)
	{
		FVector StartLocation = PlayerCamComp->GetComponentLocation();
		FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
		FVector EndLocation = StartLocation + Direction;

		spawnedPart->SetActorLocation(EndLocation);
	}
}

// Bind player input controls
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement and look controls
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);

	// Jump control
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &AMyCharacter::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &AMyCharacter::StopJump);

	// Interact and build rotation
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::FindObject);
	PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &AMyCharacter::RotateBuilding);
}

// Character movement logic - forward/backward
void AMyCharacter::MoveForward(float AxisValue)
{
	if (Controller && AxisValue != 0.0f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

// Character movement logic - left/right
void AMyCharacter::MoveRight(float AxisValue)
{
	if (Controller && AxisValue != 0.0f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}

// Called when jump input is pressed
void AMyCharacter::StartJump()
{
	bPressedJump = true;
}

// Called when jump input is released
void AMyCharacter::StopJump()
{
	bPressedJump = false;
}

// Triggers either resource collection or finalizes building placement
void AMyCharacter::FindObject()
{
	FHitResult HitResult;
	FVector StartLocation = PlayerCamComp->GetComponentLocation();
	FVector Direction = PlayerCamComp->GetForwardVector() * 800.0f;
	FVector EndLocation = StartLocation + Direction;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;

	if (!isBuilding)
	{
		// Attempt to hit a resource actor
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
		{
			AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());
			if (Stamina > 5.0f && HitResource)
			{
				FString hitName = HitResource->resourceName;
				int resourceValue = HitResource->resourceAmount;

				HitResource->totalResource -= resourceValue;

				if (HitResource->totalResource >= resourceValue)
				{
					GiveResource(resourceValue, hitName);
					matsCollected += resourceValue;
					objWidget->UpdatematOBJ(matsCollected);
					SetStamina(-5.0f);
				}
				else
				{
					HitResource->Destroy(); // Resource depleted
				}
			}
		}
	}
	else
	{
		// Finalize building placement
		isBuilding = false;
		objectsBuilt += 1.0f;
		objWidget->UpdatebuildObj(objectsBuilt);
	}
}

// Add to health, clamped under 100
void AMyCharacter::SetHealth(float amount)
{
	if (Health + amount < 100)
	{
		Health += amount;
	}
}

// Add to hunger, clamped under 100
void AMyCharacter::SetHunger(float amount)
{
	if (Hunger + amount < 100)
	{
		Hunger += amount;
	}
}

// Add to stamina, clamped under 100
void AMyCharacter::SetStamina(float amount)
{
	if (Stamina + amount < 100)
	{
		Stamina += amount;
	}
}

// Decreases hunger periodically and regenerates stamina
void AMyCharacter::DecreaseStats()
{
	if (Hunger > 0)
	{
		SetHunger(-1.0f);
	}
	SetStamina(10.0f);

	// Penalize health if starving
	if (Hunger <= 0)
	{
		SetHealth(-3.0f);
	}
}

// Adds resource amount to proper index in the array
void AMyCharacter::GiveResource(float amount, FString resourceType)
{
	if (resourceType == "Wood")
	{
		ResourcesArray[0] += amount;
	}
	else if (resourceType == "Stone")
	{
		ResourcesArray[1] += amount;
	}
	else if (resourceType == "Berry")
	{
		ResourcesArray[2] += amount;
	}
}

// Deducts resources and increases building counts
void AMyCharacter::UpdateResources(float woodAmount, float stoneAmount, FString buildingObject)
{
	if (woodAmount <= ResourcesArray[0] && stoneAmount <= ResourcesArray[1])
	{
		ResourcesArray[0] -= woodAmount;
		ResourcesArray[1] -= stoneAmount;

		if (buildingObject == "Wall")
		{
			BuildingArray[0]++;
		}
		else if (buildingObject == "Floor")
		{
			BuildingArray[1]++;
		}
		else if (buildingObject == "Ceiling")
		{
			BuildingArray[2]++;
		}
	}
}

// Spawns a building part in front of the player
void AMyCharacter::SpawnBuilding(int buildingID, bool& isSuccess)
{
	if (!isBuilding)
	{
		if (BuildingArray[buildingID] >= 1)
		{
			isBuilding = true;

			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			FRotator myRot(0, 0, 0);
			FActorSpawnParameters SpawnParams;

			BuildingArray[buildingID]--;
			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);

			isSuccess = true;
			return;
		}
	}

	isSuccess = false;
}

// Rotates the building part 90 degrees in world space
void AMyCharacter::RotateBuilding()
{
	if (isBuilding && spawnedPart)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0));
	}
}
