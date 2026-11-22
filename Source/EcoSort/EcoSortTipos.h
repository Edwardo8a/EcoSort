// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EcoSortTipos.generated.h"

/**
 * Enumerador de tipos de residuos reciclables y no reciclables para EcoSort.
 * BlueprintType permite seleccionarlo en variables y grafos de Blueprints.
 */
UENUM(BlueprintType)
enum class EEcoTipoResiduo : uint8
{
	PET			UMETA(DisplayName = "PET / Plastico"),
	Lata		UMETA(DisplayName = "Lata / Metal"),
	Papel		UMETA(DisplayName = "Papel / Carton"),
	Neumatico	UMETA(DisplayName = "Neumatico"),
	Vidrio		UMETA(DisplayName = "Vidrio")
};

/**
 * Estructura de configuración por nivel para EcoSort (compatible con DataTables).
 */
USTRUCT(BlueprintType)
struct FEcoNivelConfig : public FTableRowBase
{
	GENERATED_BODY()

	/** Nombre descriptivo del nivel (ej. "Nivel 1 - Orilla Gris") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Nivel")
	FText NombreNivel;

	/** Cantidad de residuos que se deben clasificar con éxito para ganar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Nivel")
	int32 MetaResiduos = 10;

	/** Salud inicial del río (0 a 100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Nivel")
	float NivelRioInicial = 100.0f;

	/** Velocidad a la que baja la salud del río por segundo (0.0 en Nivel 1, 1.2 en Nivel 2) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Nivel")
	float DrenajeRioPorSegundo = 0.0f;

	/** Cada cuántos segundos el spawner arroja un nuevo residuo al escenario */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Nivel")
	float IntervaloSpawn = 4.0f;

	/** Límite de basura acumulada en el suelo al mismo tiempo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Nivel")
	int32 MaxResiduosEnEscena = 6;

	/** Nombre del mapa del siguiente nivel a cargar al ganar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Nivel")
	FName SiguienteNivel;
};
