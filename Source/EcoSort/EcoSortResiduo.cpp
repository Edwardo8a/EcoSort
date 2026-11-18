// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortResiduo.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

AEcoSortResiduo::AEcoSortResiduo()
{
	PrimaryActorTick.bCanEverTick = false;

	// Creación del componente de malla raíz
	MallaResiduo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaResiduo"));
	RootComponent = MallaResiduo;

	// Configuración de física y colisiones por defecto
	MallaResiduo->SetSimulatePhysics(true);
	MallaResiduo->SetCollisionProfileName(TEXT("PhysicsActor"));
	MallaResiduo->SetGenerateOverlapEvents(true);

	TipoResiduo = EEcoTipoResiduo::PET;
	bEstaCargado = false;
}

void AEcoSortResiduo::BeginPlay()
{
	Super::BeginPlay();
}

void AEcoSortResiduo::Recoger(USceneComponent* ComponenteMano)
{
	if (!ComponenteMano || bEstaCargado)
	{
		return;
	}

	bEstaCargado = true;

	// Apagar simulación física y colisiones para evitar empujar al personaje
	MallaResiduo->SetSimulatePhysics(false);
	MallaResiduo->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Acoplar al componente Mano en posición y rotación relativas
	AttachToComponent(ComponenteMano, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AEcoSortResiduo::Soltar()
{
	if (!bEstaCargado)
	{
		return;
	}

	bEstaCargado = false;

	// Desacoplar del personaje manteniendo la posición del mundo
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Reactivar colisiones y física para que caiga naturalmente al suelo
	MallaResiduo->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MallaResiduo->SetSimulatePhysics(true);
	MallaResiduo->WakeRigidBody();
}
