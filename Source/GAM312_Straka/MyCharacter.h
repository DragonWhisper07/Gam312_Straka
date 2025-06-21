#pragma once

// Core includes for Unreal Engine character support
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Resource_M.h"
#include "Kismet/GameplayStatics.h"
#include "BuildingPart.h"
#include "PlayerWidget.h"
#include "ObjectiveWidget.h"
#include "MyCharacter.generated.h"

/**
 * AMyCharacter
 *
 * Player character class for a survival/building game.
 * Handles movement, camera, UI interaction, resource gathering, stat management,
 * and spawning building parts.
 */
UCLASS()
class GAM312_STRAKA_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructor
	AMyCharacter();

protected:
	// Called once when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Binds input axes and actions
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Movement Functions **/
	UFUNCTION()
	void MoveForward(float axisValue);

	UFUNCTION()
	void MoveRight(float axisValue);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	/** Interact with world or place building */
	UFUNCTION()
	void FindObject();

	/** Camera attached to the player, used for first-person view */
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* PlayerCamComp;

	/** ---------- Player Stats ---------- **/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Hunger = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina = 100.0f;

	/** ---------- Resource Tracking ---------- **/

	// Tracks current amount of each resource type (by index)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<float> ResourcesArray;

	// Tracks the name of each resource type (matches index in ResourcesArray)
	UPROPERTY()
	TArray<FString> ResourcesNameArray;

	// Optional debug or legacy values — not used in the main logic
	UPROPERTY(EditAnywhere, Category = "Wood")
	int Wood;

	UPROPERTY(EditAnywhere, Category = "Stone")
	int Stone;

	UPROPERTY(EditAnywhere, Category = "Berry")
	int Berry;

	// Decal to show hit markers when gathering resources (not yet used in code)
	UPROPERTY(EditAnywhere, Category = "HitMarker")
	UMaterialInterface* hitDecal;

	/** ---------- Building System ---------- **/

	// Number of buildable parts of each type (index-based: 0 = Wall, 1 = Floor, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Supplies")
	TArray<int> BuildingArray;

	// Whether the player is currently placing a building piece
	UPROPERTY()
	bool isBuilding;

	// Class reference to spawn building parts dynamically
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABuildingPart> BuildPartClass;

	// Pointer to the current building part being previewed/spawned
	UPROPERTY()
	ABuildingPart* spawnedPart;

	/** ---------- UI Widgets ---------- **/

	// Reference to the player's main HUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayerWidget* playerUI;

	// Reference to objective tracker widget (e.g., display collected materials or tasks)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObjectiveWidget* objWidget;

	// Number of objects built by player (used for objectives/UI)
	UPROPERTY()
	float objectsBuilt;

	// Number of materials collected by player (used for objectives/UI)
	UPROPERTY()
	float matsCollected;

	/** ---------- Stat & Resource Functions ---------- **/

	// Modify Health (capped at 100)
	UFUNCTION(BlueprintCallable)
	void SetHealth(float amount);

	// Modify Hunger (capped at 100)
	UFUNCTION(BlueprintCallable)
	void SetHunger(float amount);

	// Modify Stamina (capped at 100)
	UFUNCTION(BlueprintCallable)
	void SetStamina(float amount);

	// Called every few seconds to reduce hunger, regen stamina, or drain health
	UFUNCTION(BlueprintCallable)
	void DecreaseStats();

	// Adds resource amount to appropriate index in ResourcesArray
	UFUNCTION()
	void GiveResource(float amount, FString resourceType);

	// Removes resources and increases internal build part counts
	UFUNCTION(BlueprintCallable)
	void UpdateResources(float woodAmount, float stoneAmount, FString buildingObject);

	// Spawns a buildable object in front of the player
	UFUNCTION(BlueprintCallable)
	void SpawnBuilding(int buildingID, bool& isSuccess);

	// Rotates the current preview building object
	UFUNCTION()
	void RotateBuilding();
};
