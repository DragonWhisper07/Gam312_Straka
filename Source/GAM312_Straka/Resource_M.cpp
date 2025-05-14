#include "Resource_M.h"
#include "Engine/Engine.h"

// Sets default values
AResource_M::AResource_M()
{
	PrimaryActorTick.bCanEverTick = true;
	ResourceNameTxt = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Mesh;

	ResourceNameTxt->SetupAttachment(Mesh);

	resourceMesh = nullptr;
}

void AResource_M::BeginPlay()
{
	Super::BeginPlay();

	if (resourceMesh)
	{
		Mesh->SetStaticMesh(resourceMesh);
	}

	ResourceNameTxt->SetText(FText::FromString(resourceName));
}

// Called every frame
void AResource_M::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}