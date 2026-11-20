// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EcoSortTipos.h"
#include "EcoSortBote.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AEcoSortResiduo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDepositoResultado, AEcoSortBote*, Bote, bool, bEsCorrecto);

/**
 * Contenedor de reciclaje para EcoSort (Azul = PET, Amarillo = Metal, Rojo = Papel).
 * Posee una caja de colisión para detectar depósitos y valida los tipos de residuo aceptados.
 */
UCLASS(Blueprintable)
class ECOSORT_API AEcoSortBote : public AActor
{
	GENERATED_BODY()

public:
	AEcoSortBote();

protected:
	virtual void BeginPlay() override;

	/** Malla estática visual del bote de basura */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EcoSort|Componentes")
	TObjectPtr<UStaticMeshComponent> MallaBote;

	/** Zona de interacción / detección de depósito */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EcoSort|Componentes")
	TObjectPtr<UBoxComponent> ZonaDeposito;

	/** Tipos de residuo que este contenedor acepta */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Bote")
	TArray<EEcoTipoResiduo> TiposAceptados;

	/** Nombre descriptivo del contenedor (ej. "Contenedor Azul - Plásticos") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Bote")
	FText NombreContenedor;

public:
	/** Evento despachado para reproducir efectos visuales o sonidos en Blueprints hijos */
	UPROPERTY(BlueprintAssignable, Category = "EcoSort|Eventos")
	FOnDepositoResultado OnDepositoResultado;

	/**
	 * Intenta depositar un residuo en este contenedor:
	 * - Si el tipo coincide con TiposAceptados: suma puntos en GameState y destruye el residuo.
	 * - Si el tipo NO coincide: resta puntos en GameState, suelta el residuo al suelo.
	 * @param Residuo El actor residuo que Itzel está intentando depositar.
	 * @return true si fue clasificado correctamente, false si fue un error.
	 */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Bote")
	virtual bool Depositar(AEcoSortResiduo* Residuo);

	/** Verifica si este bote acepta un tipo específico de residuo */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Bote")
	bool AceptaTipo(EEcoTipoResiduo Tipo) const;

	/** Retorna los tipos aceptados */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Bote")
	const TArray<EEcoTipoResiduo>& GetTiposAceptados() const { return TiposAceptados; }
};
