// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
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
