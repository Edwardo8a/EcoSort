// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortHUDBase.h"
#include "EcoSortGameState.h"

void UEcoSortHUDBase::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	EcoGameState = World->GetGameState<AEcoSortGameState>();
	if (EcoGameState)
	{
		// 1. Suscribirse a los delegados dinámicos del GameState
		EcoGameState->OnPuntosCambiados.AddDynamic(this, &UEcoSortHUDBase::HandlePuntosCambiados);
		EcoGameState->OnHabitatCambiado.AddDynamic(this, &UEcoSortHUDBase::HandleHabitatCambiado);
		EcoGameState->OnNivelRioCambiado.AddDynamic(this, &UEcoSortHUDBase::HandleNivelRioCambiado);
		EcoGameState->OnResiduoClasificado.AddDynamic(this, &UEcoSortHUDBase::HandleResiduoClasificado);
		EcoGameState->OnAciertoRegistrado.AddDynamic(this, &UEcoSortHUDBase::HandleAciertoRegistrado);
		EcoGameState->OnErrorRegistrado.AddDynamic(this, &UEcoSortHUDBase::HandleErrorRegistrado);

		// 2. Transmitir los valores iniciales inmediatamente para inicializar la UI en Frame 0
		ActualizarPuntos(EcoGameState->GetPuntos());
		ActualizarHabitat(EcoGameState->GetHabitatPorcentaje());
		ActualizarNivelRio(EcoGameState->GetNivelRio());
		ActualizarResiduos(EcoGameState->GetResiduosClasificados(), EcoGameState->GetMetaResiduos());
	}
}

void UEcoSortHUDBase::NativeDestruct()
{
	if (EcoGameState)
	{
		EcoGameState->OnPuntosCambiados.RemoveDynamic(this, &UEcoSortHUDBase::HandlePuntosCambiados);
		EcoGameState->OnHabitatCambiado.RemoveDynamic(this, &UEcoSortHUDBase::HandleHabitatCambiado);
		EcoGameState->OnNivelRioCambiado.RemoveDynamic(this, &UEcoSortHUDBase::HandleNivelRioCambiado);
		EcoGameState->OnResiduoClasificado.RemoveDynamic(this, &UEcoSortHUDBase::HandleResiduoClasificado);
		EcoGameState->OnAciertoRegistrado.RemoveDynamic(this, &UEcoSortHUDBase::HandleAciertoRegistrado);
		EcoGameState->OnErrorRegistrado.RemoveDynamic(this, &UEcoSortHUDBase::HandleErrorRegistrado);
	}

	Super::NativeDestruct();
}

void UEcoSortHUDBase::HandlePuntosCambiados(int32 NuevosPuntos)
{
	ActualizarPuntos(NuevosPuntos);
}

void UEcoSortHUDBase::HandleHabitatCambiado(float NuevoPorcentaje)
{
	ActualizarHabitat(NuevoPorcentaje);
}

void UEcoSortHUDBase::HandleNivelRioCambiado(float NuevoNivel)
{
	ActualizarNivelRio(NuevoNivel);
}

void UEcoSortHUDBase::HandleResiduoClasificado(int32 Clasificados, int32 Meta)
{
	ActualizarResiduos(Clasificados, Meta);
}

void UEcoSortHUDBase::HandleAciertoRegistrado(int32 PuntosGanados)
{
	MostrarFeedbackAcierto(PuntosGanados);
}

void UEcoSortHUDBase::HandleErrorRegistrado(int32 PuntosPerdidos)
{
	MostrarFeedbackError(PuntosPerdidos);
}
