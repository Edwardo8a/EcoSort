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
 * Gestiona la búsqueda de residuos en el suelo, transporte y depósito en contenedores de reciclaje.
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

	/** Nombre del componente de escena en el personaje donde se sostiene el residuo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Interaccion")
	FName NombreComponenteMano;

	/** Referencia al residuo que Itzel está cargando actualmente */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Interaccion")
	TObjectPtr<AEcoSortResiduo> ResiduoCargado;

	/** Busca el componente de escena "Mano" en el actor propietario */
	USceneComponent* ObtenerComponenteMano() const;

	/** Encuentra el residuo más cercano dentro del radio configurado */
	AEcoSortResiduo* BuscarResiduoCercano() const;

	/** Encuentra el contenedor más cercano dentro del radio de depósito */
	AEcoSortBote* BuscarBoteCercano() const;

public:
	/**
	 * Acción principal de interacción invocada al presionar la tecla E:
	 * 1. Si lleva un residuo y hay un bote a menos de 200 cm: lo deposita en el bote.
	 * 2. Si lleva un residuo pero no hay botes cerca: lo suelta al suelo.
	 * 3. Si tiene las manos vacías: busca y recoge el residuo más cercano a 150 cm.
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
