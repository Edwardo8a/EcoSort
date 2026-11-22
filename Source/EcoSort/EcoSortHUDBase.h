// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EcoSortHUDBase.generated.h"

class AEcoSortGameState;

/**
 * Clase base C++ para el HUD de EcoSort.
 * Se enlaza automáticamente a los delegados de AEcoSortGameState para que la interfaz
 * se actualice de forma reactiva y eficiente (sin usar Event Tick).
 * El diseño visual (textos, barras, imágenes, animaciones) se realiza en el Blueprint hijo (WBP_HUD).
 */
UCLASS(Abstract, Blueprintable)
class ECOSORT_API UEcoSortHUDBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Referencia en caché al GameState activo */
	UPROPERTY(BlueprintReadOnly, Category = "EcoSort|HUD")
	TObjectPtr<AEcoSortGameState> EcoGameState;

	// --- Métodos puente internos conectados a los delegados ---
	UFUNCTION()
	void HandlePuntosCambiados(int32 NuevosPuntos);

	UFUNCTION()
	void HandleHabitatCambiado(float NuevoPorcentaje);

	UFUNCTION()
	void HandleNivelRioCambiado(float NuevoNivel);

	UFUNCTION()
	void HandleResiduoClasificado(int32 Clasificados, int32 Meta);

	UFUNCTION()
	void HandleAciertoRegistrado(int32 PuntosGanados);

	UFUNCTION()
	void HandleErrorRegistrado(int32 PuntosPerdidos);

public:
	// --- Eventos implementables en Blueprint para diseñar los gráficos en WBP_HUD ---

	/** Llamado cuando el puntaje total cambia */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|HUD")
	void ActualizarPuntos(int32 NuevosPuntos);

	/** Llamado cuando el porcentaje de hábitat cambia (0.0% a 100.0%) */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|HUD")
	void ActualizarHabitat(float NuevoPorcentaje);

	/** Llamado cuando el nivel de salud del río cambia (0.0 a 100.0) */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|HUD")
	void ActualizarNivelRio(float NuevoNivel);

	/** Llamado para actualizar el contador de meta (ej. 3 / 10) */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|HUD")
	void ActualizarResiduos(int32 Clasificados, int32 Meta);

	/** Llamado al clasificar un residuo con éxito (muestra check verde y "+10") */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|HUD")
	void MostrarFeedbackAcierto(int32 PuntosGanados);

	/** Llamado al equivocarse de contenedor (muestra X roja y "-5") */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|HUD")
	void MostrarFeedbackError(int32 PuntosPerdidos);
};
