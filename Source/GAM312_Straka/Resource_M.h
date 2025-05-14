#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Resource_M.generated.h"

UCLASS()
class GAM312_STRAKA_API AResource_M : public AActor
{
	GENERATED_BODY()

public:
	AResource_M();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	FString resourceName = "Wood";  

	UPROPERTY(EditAnywhere)
	int resourceAmount = 5;  

	UPROPERTY(EditAnywhere)
	int totalResource = 100; 

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* ResourceNameTxt;  

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;  

	UPROPERTY(EditAnywhere)
	UStaticMesh* resourceMesh;  
};