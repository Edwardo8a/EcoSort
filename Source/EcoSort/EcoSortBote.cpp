// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortBote.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "EcoSortResiduo.h"
#include "EcoSortGameState.h"
#include "Kismet/GameplayStatics.h"

AEcoSortBote::AEcoSortBote()
{
	PrimaryActorTick.bCanEverTick = false;

	// Componente de malla visual principal
	MallaBote = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaBote"));
	RootComponent = MallaBote;
	MallaBote->SetCollisionProfileName(TEXT("BlockAll"));

	// Caja de interacción para el depósito (zona sensible al jugador)
	ZonaDeposito = CreateDefaultSubobject<UBoxComponent>(TEXT("ZonaDeposito"));
	ZonaDeposito->SetupAttachment(RootComponent);
	ZonaDeposito->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	ZonaDeposito->SetCollisionProfileName(TEXT("Trigger"));
	ZonaDeposito->SetGenerateOverlapEvents(true);

	NombreContenedor = FText::FromString(TEXT("Contenedor"));
}

void AEcoSortBote::BeginPlay()
{
	Super::BeginPlay();
}

bool AEcoSortBote::AceptaTipo(EEcoTipoResiduo Tipo) const
{
	return TiposAceptados.Contains(Tipo);
}

bool AEcoSortBote::Depositar(AEcoSortResiduo* Residuo)
{
	if (!Residuo)
	{
		return false;
	}

	AEcoSortGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AEcoSortGameState>() : nullptr;
	const bool bEsCorrecto = AceptaTipo(Residuo->GetTipoResiduo());

	if (bEsCorrecto)
	{
		// Acierto: registrar en GameState
		if (GameState)
		{
			GameState->RegistrarAcierto();
		}

		// Disparar evento para partículas o sonido
		OnDepositoResultado.Broadcast(this, true);

		// Destruir el residuo del mundo (ha sido reciclado con éxito)
		Residuo->Destroy();
		return true;
	}
	else
	{
		// Error: registrar penalización en GameState
		if (GameState)
		{
			GameState->RegistrarError();
		}

		// Disparar evento de error
		OnDepositoResultado.Broadcast(this, false);

		// Regresar el residuo al suelo con física reactivada
		Residuo->Soltar();
		return false;
	}
}
