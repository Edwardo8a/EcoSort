// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EcoSortInteractionComponent.generated.h"

class AEcoSortResiduo;
class AEcoSortBote;
class USceneComponent;

/**
 * Componente de interacción para el personaje jugador (Itzel).
 * Posee detección asistida por apuntado (cámara/retículo) y proximidad para
 * recoger residuos y depositarlos en contenedores de reciclaje con precisión.
 */
UCLASS(ClassGroup = (EcoSort), meta = (BlueprintSpawnableComponent))
class ECOSORT_API UEcoSortInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEcoSortInteractionComponent();

protected:
	virtual void BeginPlay() override;

	/** Radio máximo en centímetros para detectar y recoger un residuo del suelo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Interaccion")
	float RadioInteraccion;

	/** Radio máximo en centímetros para detectar un contenedor de reciclaje y depositar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Interaccion")
	float RadioDeposito;

	/** Distancia máxima del rayo de apuntado desde la cámara hacia el centro de la mira */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Interaccion")
	float DistanciaApuntado;

	/** Nombre del componente de escena en el personaje donde se sostiene el residuo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Interaccion")
	FName NombreComponenteMano;

	/** Referencia al residuo que Itzel está cargando actualmente */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Interaccion")
	TObjectPtr<AEcoSortResiduo> ResiduoCargado;

	/** Busca el componente de escena "Mano" en el actor propietario */
	USceneComponent* ObtenerComponenteMano() const;

	/** Encuentra el residuo al que está apuntando la mira o el más cercano al frente */
	AEcoSortResiduo* BuscarResiduoObjetivo() const;

	/** Encuentra el contenedor al que está apuntando la mira o el más cercano */
	AEcoSortBote* BuscarBoteObjetivo() const;

public:
	/**
	 * Acción principal de interacción invocada al presionar la tecla E:
	 * 1. Si lleva un residuo y apunta/está cerca de un bote: lo deposita.
	 * 2. Si lleva un residuo pero no hay botes: lo suelta al suelo.
	 * 3. Si tiene manos libres: busca y recoge el residuo enfocado por la mira.
	 * @return true si se realizó una acción con éxito.
	 */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Interaccion")
	bool TryInteract();

	/** Indica si Itzel tiene actualmente un residuo en sus manos */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Interaccion")
	bool TieneResiduo() const { return ResiduoCargado != nullptr; }

	/** Devuelve el residuo cargado */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Interaccion")
	AEcoSortResiduo* GetResiduoCargado() const { return ResiduoCargado; }
};
