#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingMessageWidget.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API UFloatingMessageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetMessageText(const FText& NewText);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MessageText;
};