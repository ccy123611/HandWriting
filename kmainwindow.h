#ifndef __KMAINWINDOW_H__
#define __KMAINWINDOW_H__

#include <QtWidgets/QMainWindow>
#include <QFocusEvent>
#include <QShowEvent>
#include <iostream>
#include "ui_kmainwindow.h"
#include "HandwritingArea.h"
#include "SettingsDialog.h"
#include "TextPredictionEngine.h"
#include <QLabel>

class KMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    KMainWindow(QWidget *parent = nullptr);
    ~KMainWindow();

    void setupConnections();
    void storeInitialButtonTexts();
    void storeEnglishModeTexts();
    void storeNumberModeTexts();
    void storePunctuationModeTexts();

    void SendKeys(HWND hwnd, const std::wstring& text);

    bool single_mode;
    bool mult_mode;
    
protected:
    //void focusInEvent(QFocusEvent* event)override;
    
signals:
    void needToUpdateCandidateButtons(const QStringList& texts);

public slots:
    void getRecognitonResult(IInkRecognitionAlternates* recoginitionResult);
    void updateShortCandidateButtons(const QStringList& texts);
    void updateButtonTexts(const QMap<QPushButton*, QString>& newTexts);
    void resetButtonTextsToDefault();
    void clickCandidateButtonsInput();
    void updatePriority();

    void switchToSingleMode();
    void switchToMultMode();

    void switchToChineseMode();
    void switchToEnglishMode();
    void switchToNumberMode();
    void switchToPunctuationMode();


private:
    QMap<QPushButton*, QString>buttonInitialTexts;
    QMap<QPushButton*, QString> englishModeTexts;
    QMap<QPushButton*, QString> numberModeTexts;
    QMap<QPushButton*, QString> punctuationModeTexts;

    Ui::KMainWindowClass ui;
    SettingsDialog* settingsDialog;
    TextPredictionEngine* predictionEngine;
};

#endif