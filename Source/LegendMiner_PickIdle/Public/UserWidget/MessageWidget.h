#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageConfirmed, bool, bConfirmed);

UCLASS()
class LEGENDMINER_PICKIDLE_API UMessageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MessageText;

    UPROPERTY(meta = (BindWidget))
    class UButton* ConfirmButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* CancelButton;

    // 메시지 확인(확인/취소) 결과를 전달하는 delegate
    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnMessageConfirmed OnMessageConfirmed;

    /** 메시지를 설정하고 버튼을 표시할지 결정 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetupMessage(const FText& Message, bool bShowCancelButton,
        const FText& ConfirmText = NSLOCTEXT("MessageWidget", "Confirm", "확인"),
        const FText& CancelText = NSLOCTEXT("MessageWidget", "Cancel", "취소"));

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnConfirmClicked();

    UFUNCTION()
    void OnCancelClicked();
};
