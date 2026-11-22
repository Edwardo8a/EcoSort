// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortInteractionComponent.h"
#include "EcoSortResiduo.h"
#include "EcoSortBote.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

UEcoSortInteractionComponent::UEcoSortInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	RadioInteraccion = 220.0f;   // Radio cómodo de recogida
	RadioDeposito = 260.0f;      // Radio de entrega a botes
	DistanciaApuntado = 450.0f;  // Distancia del rayo hacia la mira central
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

AEcoSortResiduo* UEcoSortInteractionComponent::BuscarResiduoObjetivo() const
{
	AActor* Propietario = GetOwner();
	UWorld* World = GetWorld();
	if (!Propietario || !World)
	{
		return nullptr;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	FVector CamLoc = Propietario->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	FRotator CamRot = Propietario->GetActorRotation();

	if (PC && PC->PlayerCameraManager)
	{
		CamLoc = PC->PlayerCameraManager->GetCameraLocation();
		CamRot = PC->PlayerCameraManager->GetCameraRotation();
	}

	const FVector CamDir = CamRot.Vector();

	// 1. Rayo asistido (SphereTrace) hacia el centro de la mira
	FHitResult Hit;
	const FVector TraceEnd = CamLoc + (CamDir * DistanciaApuntado);
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(EcoSortAimTrace), false, Propietario);

	if (World->SweepSingleByChannel(Hit, CamLoc, TraceEnd, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(40.0f), TraceParams))
	{
		if (AEcoSortResiduo* ResiduoHit = Cast<AEcoSortResiduo>(Hit.GetActor()))
		{
			if (!ResiduoHit->EstaCargado())
			{
				return ResiduoHit;
			}
		}
	}

	// 2. Si el rayo central no impactó directo, buscar por proximidad y ángulo de visión
	TArray<AActor*> ResiduosEnMundo;
	UGameplayStatics::GetAllActorsOfClass(World, AEcoSortResiduo::StaticClass(), ResiduosEnMundo);

	const FVector PosJugador = Propietario->GetActorLocation();
	const float RadioAlCuadrado = FMath::Square(RadioInteraccion);

	AEcoSortResiduo* MejorResiduo = nullptr;
	float MejorScore = -1.0f;

	for (AActor* Actor : ResiduosEnMundo)
	{
		AEcoSortResiduo* Residuo = Cast<AEcoSortResiduo>(Actor);
		if (!Residuo || Residuo->EstaCargado())
		{
			continue;
		}

		FVector HaciaResiduo = Residuo->GetActorLocation() - PosJugador;
		HaciaResiduo.Z *= 0.3f; // Menos peso a la altura para recoger fácil del piso
		const float DistanciaSq = HaciaResiduo.SizeSquared();

		if (DistanciaSq <= RadioAlCuadrado)
		{
			HaciaResiduo.Normalize();
			const float Alineacion = FMath::Max(0.05f, FVector::DotProduct(CamDir, HaciaResiduo));
			const float Score = Alineacion / (DistanciaSq + 1.0f);

			if (Score > MejorScore)
			{
				MejorScore = Score;
				MejorResiduo = Residuo;
			}
		}
	}

	return MejorResiduo;
}

AEcoSortBote* UEcoSortInteractionComponent::BuscarBoteObjetivo() const
{
	AActor* Propietario = GetOwner();
	UWorld* World = GetWorld();
	if (!Propietario || !World)
	{
		return nullptr;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	FVector CamLoc = Propietario->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	FRotator CamRot = Propietario->GetActorRotation();

	if (PC && PC->PlayerCameraManager)
	{
		CamLoc = PC->PlayerCameraManager->GetCameraLocation();
		CamRot = PC->PlayerCameraManager->GetCameraRotation();
	}

	const FVector CamDir = CamRot.Vector();

	// 1. Rayo hacia el contenedor que enfoca la mira
	FHitResult Hit;
	const FVector TraceEnd = CamLoc + (CamDir * DistanciaApuntado);
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(EcoSortBoteTrace), false, Propietario);

	if (World->SweepSingleByChannel(Hit, CamLoc, TraceEnd, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(60.0f), TraceParams))
	{
		if (AEcoSortBote* BoteHit = Cast<AEcoSortBote>(Hit.GetActor()))
		{
			return BoteHit;
		}
	}

	// 2. Por proximidad al contenedor más cercano
	TArray<AActor*> BotesEnMundo;
	UGameplayStatics::GetAllActorsOfClass(World, AEcoSortBote::StaticClass(), BotesEnMundo);

	const FVector PosJugador = Propietario->GetActorLocation();
	const float RadioAlCuadrado = FMath::Square(RadioDeposito);

	AEcoSortBote* MejorBote = nullptr;
	float MenorDistanciaSq = RadioAlCuadrado;

	for (AActor* Actor : BotesEnMundo)
	{
		AEcoSortBote* Bote = Cast<AEcoSortBote>(Actor);
		if (!Bote)
		{
			continue;
		}

		const float DistanciaSq = FVector::DistSquared(PosJugador, Bote->GetActorLocation());
		if (DistanciaSq <= MenorDistanciaSq)
		{
			MenorDistanciaSq = DistanciaSq;
			MejorBote = Bote;
		}
	}

	return MejorBote;
}

bool UEcoSortInteractionComponent::TryInteract()
{
	// -------------------------------------------------------------
	// CASO 1: Ya llevamos un residuo en la mano
	// -------------------------------------------------------------
	if (ResiduoCargado)
	{
		// 1.1 Si apuntamos a un contenedor o estamos cerca de uno, depositar
		AEcoSortBote* BoteObjetivo = BuscarBoteObjetivo();
		if (BoteObjetivo)
		{
			BoteObjetivo->Depositar(ResiduoCargado);
			ResiduoCargado = nullptr;
			return true;
		}

		// 1.2 Si no hay contenedor enfocado ni cercano, soltar al suelo
		ResiduoCargado->Soltar();
		ResiduoCargado = nullptr;
		return true;
	}

	// -------------------------------------------------------------
	// CASO 2: Manos libres -> Buscar el residuo enfocado por la mira
	// -------------------------------------------------------------
	AEcoSortResiduo* ResiduoObjetivo = BuscarResiduoObjetivo();
	if (!ResiduoObjetivo)
	{
		return false;
	}

	USceneComponent* Mano = ObtenerComponenteMano();
	if (!Mano)
	{
		return false;
	}

	ResiduoObjetivo->Recoger(Mano);
	ResiduoCargado = ResiduoObjetivo;
	return true;
}
