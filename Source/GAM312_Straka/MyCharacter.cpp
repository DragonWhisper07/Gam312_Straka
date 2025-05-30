

#include "MyCharacter.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));
	PlayerCamComp->SetupAttachment(RootComponent);
	PlayerCamComp->bUsePawnControlRotation = true;

	BuildingArray.SetNum(3);
	ResourcesArray.SetNum(3);
	ResourcesNameArray.Add(TEXT("Wood"));
	ResourcesNameArray.Add(TEXT("Stone"));
	ResourcesNameArray.Add(TEXT("Berry"));
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle StatsTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &AMyCharacter::DecreaseStats, 2.0f, true);

	check(PlayerCamComp != nullptr);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	playerUI->UpdateBars(Health, Hunger, Stamina);



	if (isBuilding && spawnedPart)
	{
		FVector StartLocation = PlayerCamComp->GetComponentLocation();
		FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
		FVector EndLocation = StartLocation + Direction;
		spawnedPart->SetActorLocation(EndLocation);
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("JumpInput", IE_Pressed, this, &AMyCharacter::StartJump);
	PlayerInputComponent->BindAction("JumpInput", IE_Released, this, &AMyCharacter::StopJump);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::FindObject);
	PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &AMyCharacter::RotateBuilding);
}

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

void AMyCharacter::StartJump()
{
	bPressedJump = true;
}

void AMyCharacter::StopJump()
{
	bPressedJump = false;
}

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
					SetStamina(-5.0f);
				}
				else
				{
					HitResource->Destroy();
				}
			}
		}
	}
	else
	{
		isBuilding = false;
	}
}

void AMyCharacter::SetHealth(float amount)
{
	if (Health + amount < 100)
	{
		Health += amount;
	}
}

void AMyCharacter::SetHunger(float amount)
{
	if (Hunger + amount < 100)
	{
		Hunger += amount;
	}
}

void AMyCharacter::SetStamina(float amount)
{
	if (Stamina + amount < 100)
	{
		Stamina += amount;
	}
}

void AMyCharacter::DecreaseStats()
{
	if (Hunger > 0)
	{
		SetHunger(-1.0f);
	}
	SetStamina(10.0f);

	if (Hunger <= 0)
	{
		SetHealth(-3.0f);
	}
}

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

void AMyCharacter::RotateBuilding()
{
	if (isBuilding && spawnedPart)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0));
	}
}
