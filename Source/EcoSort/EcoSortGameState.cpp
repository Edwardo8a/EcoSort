// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortGameState.h"

AEcoSortGameState::AEcoSortGameState()
{
	PrimaryActorTick.bCanEverTick = false;

	Puntos = 0;
	HabitatPorcentaje = 0.0f;
	NivelRio = 100.0f;
	ResiduosClasificados = 0;
	MetaResiduos = 10;
}

void AEcoSortGameState::BeginPlay()
{
	Super::BeginPlay();

	// Transmitir valores iniciales
	OnPuntosCambiados.Broadcast(Puntos);
	OnHabitatCambiado.Broadcast(HabitatPorcentaje);
	OnNivelRioCambiado.Broadcast(NivelRio);
	OnResiduoClasificado.Broadcast(ResiduosClasificados, MetaResiduos);
}

void AEcoSortGameState::RegistrarAcierto()
{
	const int32 PuntosGanados = 10;
	Puntos += PuntosGanados;
	ResiduosClasificados++;

	// Cada acierto aumenta el hábitat en 10% hasta un tope de 100%
	HabitatPorcentaje = FMath::Clamp(HabitatPorcentaje + 10.0f, 0.0f, 100.0f);

	// En caso de que el río haya perdido salud, un acierto recupera +4.0f
	NivelRio = FMath::Clamp(NivelRio + 4.0f, 0.0f, 100.0f);

	// Notificar a la UI y al mundo
	OnPuntosCambiados.Broadcast(Puntos);
	OnHabitatCambiado.Broadcast(HabitatPorcentaje);
	OnNivelRioCambiado.Broadcast(NivelRio);
	OnResiduoClasificado.Broadcast(ResiduosClasificados, MetaResiduos);
	OnAciertoRegistrado.Broadcast(PuntosGanados);
}

void AEcoSortGameState::RegistrarError()
{
	const int32 PuntosPerdidos = 5;
	Puntos = FMath::Max(0, Puntos - PuntosPerdidos);

	// Notificar a la UI
	OnPuntosCambiados.Broadcast(Puntos);
	OnErrorRegistrado.Broadcast(PuntosPerdidos);
}

void AEcoSortGameState::ReducirNivelRio(float Cantidad)
{
	if (Cantidad <= 0.0f)
	{
		return;
	}

	NivelRio = FMath::Clamp(NivelRio - Cantidad, 0.0f, 100.0f);
	OnNivelRioCambiado.Broadcast(NivelRio);
}
