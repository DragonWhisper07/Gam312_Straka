#include "BuildingPart.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

// Constructor
ABuildingPart::ABuildingPart()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize and attach mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Initialize and attach pivot arrow
	PivotArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("PivotArrow"));
	PivotArrow->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABuildingPart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABuildingPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}