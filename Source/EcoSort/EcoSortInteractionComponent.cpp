// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortInteractionComponent.h"
#include "EcoSortResiduo.h"
#include "EcoSortBote.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UEcoSortInteractionComponent::UEcoSortInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	RadioInteraccion = 150.0f;
	RadioDeposito = 200.0f;
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

	TArray<USceneComponent*> ComponentesEscena;
	Propietario->GetComponents<USceneComponent>(ComponentesEscena);

	for (USceneComponent* Comp : ComponentesEscena)
	{
		if (Comp && (Comp->GetFName() == NombreComponenteMano || Comp->ComponentHasTag(NombreComponenteMano)))
		{
			return Comp;
		}
	}

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

AEcoSortBote* UEcoSortInteractionComponent::BuscarBoteCercano() const
{
	AActor* Propietario = GetOwner();
	UWorld* World = GetWorld();
	if (!Propietario || !World)
	{
		return nullptr;
	}

	const FVector PosicionJugador = Propietario->GetActorLocation();
	const float RadioAlCuadrado = FMath::Square(RadioDeposito);

	TArray<AActor*> BotesEnMundo;
	UGameplayStatics::GetAllActorsOfClass(World, AEcoSortBote::StaticClass(), BotesEnMundo);

	AEcoSortBote* BoteMasCercano = nullptr;
	float MenorDistanciaAlCuadrado = RadioAlCuadrado;

	for (AActor* Actor : BotesEnMundo)
	{
		AEcoSortBote* Bote = Cast<AEcoSortBote>(Actor);
		if (!Bote)
		{
			continue;
		}

		const float DistanciaSq = FVector::DistSquared(PosicionJugador, Bote->GetActorLocation());
		if (DistanciaSq <= MenorDistanciaAlCuadrado)
		{
			MenorDistanciaAlCuadrado = DistanciaSq;
			BoteMasCercano = Bote;
		}
	}

	return BoteMasCercano;
}

bool UEcoSortInteractionComponent::TryInteract()
{
	// -------------------------------------------------------------
	// CASO 1: Ya llevamos un residuo en la mano
	// -------------------------------------------------------------
	if (ResiduoCargado)
	{
		// 1.1 Si estamos cerca de un contenedor de reciclaje (<= 200 cm), depositarlo
		AEcoSortBote* BoteCercano = BuscarBoteCercano();
		if (BoteCercano)
		{
			BoteCercano->Depositar(ResiduoCargado);
			ResiduoCargado = nullptr;
			return true;
		}

		// 1.2 Si NO hay un contenedor cerca, soltar el residuo al suelo
		ResiduoCargado->Soltar();
		ResiduoCargado = nullptr;
		return true;
	}

	// -------------------------------------------------------------
	// CASO 2: Manos libres -> Buscar y recoger el residuo más cercano (<= 150 cm)
	// -------------------------------------------------------------
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
