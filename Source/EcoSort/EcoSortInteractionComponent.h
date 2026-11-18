// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EcoSortInteractionComponent.generated.h"

class AEcoSortResiduo;
class USceneComponent;

/**
 * Componente de interacción para el personaje jugador (Itzel).
 * Busca residuos cercanos y gestiona la recolección hacia el punto de anclaje "Mano".
 */
UCLASS(ClassGroup = (EcoSort), meta = (BlueprintSpawnableComponent))
class ECOSORT_API UEcoSortInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEcoSortInteractionComponent();

protected:
	virtual void BeginPlay() override;

	/** Radio máximo en centímetros para detectar y recoger un residuo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Interaccion")
	float RadioInteraccion;

	/** Nombre del componente de escena o socket en el personaje donde se sostendrá el residuo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Interaccion")
	FName NombreComponenteMano;

	/** Referencia al residuo que Itzel está cargando actualmente (nullptr si tiene las manos libres) */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Interaccion")
	TObjectPtr<AEcoSortResiduo> ResiduoCargado;

	/** Busca el componente de escena "Mano" en el actor propietario */
	USceneComponent* ObtenerComponenteMano() const;

	/** Encuentra el residuo más cercano dentro del radio configurado */
	AEcoSortResiduo* BuscarResiduoCercano() const;

public:
	/**
	 * Acción principal de interacción invocada al presionar la tecla de interacción (ej. E).
	 * Si tiene un residuo en mano, lo suelta. Si tiene las manos vacías, busca y recoge el más cercano.
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
