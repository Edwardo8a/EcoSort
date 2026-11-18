// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortInteractionComponent.h"
#include "EcoSortResiduo.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UEcoSortInteractionComponent::UEcoSortInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	RadioInteraccion = 150.0f;
	NombreComponenteMano = FName(TEXT("Mano"));
	ResiduoCargado = nullptr;
}

void UEcoSortInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

USceneComponent* UEcoSortInteractionComponent::ObtenerComponenteMano() const
{
	AActor* Propietario = GetOwner();
	if (!Propietario)
	{
		return nullptr;
	}

	// 1. Buscar entre todos los componentes de escena por nombre de componente o socket
	TArray<USceneComponent*> ComponentesEscena;
	Propietario->GetComponents<USceneComponent>(ComponentesEscena);

	for (USceneComponent* Comp : ComponentesEscena)
	{
		if (Comp && (Comp->GetFName() == NombreComponenteMano || Comp->ComponentHasTag(NombreComponenteMano)))
		{
			return Comp;
		}
	}

	// 2. Si no existe un componente explícito "Mano", usar el RootComponent como respaldo
	return Propietario->GetRootComponent();
}

AEcoSortResiduo* UEcoSortInteractionComponent::BuscarResiduoCercano() const
{
	AActor* Propietario = GetOwner();
	UWorld* World = GetWorld();
	if (!Propietario || !World)
	{
		return nullptr;
	}

	const FVector PosicionJugador = Propietario->GetActorLocation();
	const float RadioAlCuadrado = FMath::Square(RadioInteraccion);

	TArray<AActor*> ResiduosEnMundo;
	UGameplayStatics::GetAllActorsOfClass(World, AEcoSortResiduo::StaticClass(), ResiduosEnMundo);

	AEcoSortResiduo* ResiduoMasCercano = nullptr;
	float MenorDistanciaAlCuadrado = RadioAlCuadrado;

	for (AActor* Actor : ResiduosEnMundo)
	{
		AEcoSortResiduo* Residuo = Cast<AEcoSortResiduo>(Actor);
		if (!Residuo || Residuo->EstaCargado())
		{
			continue;
		}

		const float DistanciaSq = FVector::DistSquared(PosicionJugador, Residuo->GetActorLocation());
		if (DistanciaSq <= MenorDistanciaAlCuadrado)
		{
			MenorDistanciaAlCuadrado = DistanciaSq;
			ResiduoMasCercano = Residuo;
		}
	}

	return ResiduoMasCercano;
}

bool UEcoSortInteractionComponent::TryInteract()
{
	// Caso A: Ya llevamos un residuo en mano -> Soltarlo
	if (ResiduoCargado)
	{
		ResiduoCargado->Soltar();
		ResiduoCargado = nullptr;
		return true;
	}

	// Caso B: Manos libres -> Buscar el residuo más cercano dentro del radio
	AEcoSortResiduo* ResiduoCercano = BuscarResiduoCercano();
	if (!ResiduoCercano)
	{
		return false;
	}

	USceneComponent* Mano = ObtenerComponenteMano();
	if (!Mano)
	{
		return false;
	}

	ResiduoCercano->Recoger(Mano);
	ResiduoCargado = ResiduoCercano;
	return true;
}
