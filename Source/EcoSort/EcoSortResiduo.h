// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EcoSortTipos.h"
#include "EcoSortResiduo.generated.h"

class UStaticMeshComponent;
class USceneComponent;

/**
 * Clase base C++ para todos los residuos reciclables de EcoSort.
 * Los Blueprints hijos (BP_Residuo_PET, BP_Residuo_Lata, etc.) definen la malla visual y el tipo.
 */
UCLASS(Blueprintable)
class ECOSORT_API AEcoSortResiduo : public AActor
{
	GENERATED_BODY()

public:
	AEcoSortResiduo();

protected:
	virtual void BeginPlay() override;

	/** Componente de malla estática principal con física */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EcoSort|Componentes")
	TObjectPtr<UStaticMeshComponent> MallaResiduo;

	/** Tipo de residuo que representa este actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Residuo")
	EEcoTipoResiduo TipoResiduo;

	/** Indica si el residuo está actualmente en las manos del jugador */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Residuo")
	bool bEstaCargado;

public:
	/**
	 * Desactiva la física, apaga colisiones y acopla el residuo al componente Mano del personaje.
	 * @param ComponenteMano Componente de escena frente al jugador donde se colocará el residuo.
	 */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Residuo")
	virtual void Recoger(USceneComponent* ComponenteMano);

	/**
	 * Desacopla el residuo del jugador y reactiva la física y colisiones para que caiga al suelo.
	 */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Residuo")
	virtual void Soltar();

	/** Obtiene el tipo de residuo clasificado */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Residuo")
	EEcoTipoResiduo GetTipoResiduo() const { return TipoResiduo; }

	/** Consulta si el residuo está siendo cargado */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Residuo")
	bool EstaCargado() const { return bEstaCargado; }

	/** Acceso a la malla estática */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Residuo")
	UStaticMeshComponent* GetMallaResiduo() const { return MallaResiduo; }
};
