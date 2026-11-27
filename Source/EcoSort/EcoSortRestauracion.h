// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EcoSortRestauracion.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/**
 * Controlador de restauración ambiental para el río y el parque.
 * Reacciona a los cambios en el porcentaje de hábitat del GameState:
 * - Transforma el color y claridad del agua del río en tiempo real.
 * - Hace aparecer vegetación y fauna en los hitos del 30%, 60% y 100%.
 */
UCLASS()
class ECOSORT_API AEcoSortRestauracion : public AActor
{
	GENERATED_BODY()

public:
	AEcoSortRestauracion();

protected:
	virtual void BeginPlay() override;

	/** Actor que representa el plano de agua del río en el nivel */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Agua")
	TObjectPtr<AActor> ActorAguaRio;

	/** Nombre del parámetro de color en el material de agua (ej. "BaseColor", "ColorAgua") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Agua")
	FName NombreParametroColor;

	/** Nombre del parámetro escalar de claridad/turbidez (ej. "Claridad", "Opacidad") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Agua")
	FName NombreParametroClaridad;

	/** Color del agua sucia y contaminada al inicio (café/lodoso) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Agua")
	FLinearColor ColorAguaContaminada;

	/** Color del agua limpia y restaurada al 100% (azul/turquesa cristalino) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Agua")
	FLinearColor ColorAguaLimpia;

	/** Instancia de material dinámico creada en tiempo de ejecución para el agua */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> MaterialDinamicoAgua;

	// -------------------------------------------------------------
	// Hitos de Restauración (Fauna y Vegetación)
	// -------------------------------------------------------------

	/** Actores que aparecen al alcanzar el 30% de hábitat (ej. lirios, peces pequeños) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Hitos")
	TArray<TObjectPtr<AActor>> ActoresHito30;

	/** Actores que aparecen al alcanzar el 60% de hábitat (ej. patos, juncos verdes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Hitos")
	TArray<TObjectPtr<AActor>> ActoresHito60;

	/** Actores que aparecen al alcanzar el 100% de hábitat (ej. garzas, mariposas, flores) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Restauracion|Hitos")
	TArray<TObjectPtr<AActor>> ActoresHito100;

	/** Banderas para registrar hitos ya activados */
	bool bHito30Alcanzado;
	bool bHito60Alcanzado;
	bool bHito100Alcanzado;

	/** Inicializa el material dinámico del agua */
	void InicializarMaterialAgua();

	/** Oculta o muestra un grupo de actores */
	void ConfigurarVisibilidadGrupo(const TArray<TObjectPtr<AActor>>& Grupo, bool bVisible);

	/** Handler para cuando cambia el porcentaje de hábitat en el GameState */
	UFUNCTION()
	void HandleHabitatCambiado(float NuevoPorcentaje);

public:
	/** Aplica manualmente un porcentaje de restauración (0.0 a 100.0) */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Restauracion")
	void AplicarRestauracion(float Porcentaje);

	/** Evento disparado en Blueprint cuando se supera un hito ecológico (30, 60 o 100) */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|Restauracion")
	void OnHitoAlcanzado(int32 Hito, float PorcentajeActual);
};
