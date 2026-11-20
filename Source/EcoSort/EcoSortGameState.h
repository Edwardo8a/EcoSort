// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EcoSortTipos.h"
#include "EcoSortGameState.generated.h"

// Delegados dinámicos para que la interfaz (HUD) y los actores del mundo se actualicen sin Tick
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPuntosCambiados, int32, NuevosPuntos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHabitatCambiado, float, NuevoPorcentajeHabitat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNivelRioCambiado, float, NuevoNivelRio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResiduoClasificado, int32, TotalClasificados, int32, Meta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAciertoRegistrado, int32, PuntosGanados);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnErrorRegistrado, int32, PuntosPerdidos);

/**
 * GameState principal de EcoSort.
 * Administra el puntaje global, porcentaje de restauración del hábitat y salud del río.
 */
UCLASS()
class ECOSORT_API AEcoSortGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AEcoSortGameState();

protected:
	virtual void BeginPlay() override;

	/** Puntos acumulados por clasificación correcta */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Estado")
	int32 Puntos;

	/** Porcentaje de restauración ambiental (0.0% a 100.0%) */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Estado")
	float HabitatPorcentaje;

	/** Salud del río (0.0 a 100.0). En Nivel 2 baja con el tiempo */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Estado")
	float NivelRio;

	/** Cantidad de residuos depositados correctamente */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Estado")
	int32 ResiduosClasificados;

	/** Meta de residuos clasificados para completar el nivel */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Estado")
	int32 MetaResiduos;

public:
	// --- Delegados para la UI ---
	UPROPERTY(BlueprintAssignable, Category = "EcoSort|Eventos")
	FOnPuntosCambiados OnPuntosCambiados;

	UPROPERTY(BlueprintAssignable, Category = "EcoSort|Eventos")
	FOnHabitatCambiado OnHabitatCambiado;

	UPROPERTY(BlueprintAssignable, Category = "EcoSort|Eventos")
	FOnNivelRioCambiado OnNivelRioCambiado;

	UPROPERTY(BlueprintAssignable, Category = "EcoSort|Eventos")
	FOnResiduoClasificado OnResiduoClasificado;

	UPROPERTY(BlueprintAssignable, Category = "EcoSort|Eventos")
	FOnAciertoRegistrado OnAciertoRegistrado;

	UPROPERTY(BlueprintAssignable, Category = "EcoSort|Eventos")
	FOnErrorRegistrado OnErrorRegistrado;

	// --- Funciones de Lógica de Juego ---

	/**
	 * Registra un depósito correcto en un contenedor:
	 * +10 puntos, +10% de hábitat, suma a residuos clasificados y sube nivel de río.
	 */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Puntaje")
	void RegistrarAcierto();

	/**
	 * Registra un depósito erróneo:
	 * -5 puntos (sin bajar de 0) y dispara notificación de error en UI.
	 */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Puntaje")
	void RegistrarError();

	/** Reduce el nivel del río (usado en Nivel 2 por contaminación constante) */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Puntaje")
	void ReducirNivelRio(float Cantidad);

	// Getters
	UFUNCTION(BlueprintPure, Category = "EcoSort|Estado")
	int32 GetPuntos() const { return Puntos; }

	UFUNCTION(BlueprintPure, Category = "EcoSort|Estado")
	float GetHabitatPorcentaje() const { return HabitatPorcentaje; }

	UFUNCTION(BlueprintPure, Category = "EcoSort|Estado")
	float GetNivelRio() const { return NivelRio; }

	UFUNCTION(BlueprintPure, Category = "EcoSort|Estado")
	int32 GetResiduosClasificados() const { return ResiduosClasificados; }

	UFUNCTION(BlueprintPure, Category = "EcoSort|Estado")
	int32 GetMetaResiduos() const { return MetaResiduos; }
};
