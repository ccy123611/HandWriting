#include "kmainwindow.h"
#include "HandwritingArea.h"
#include "HandwritingRecognizer.h"
#include <QPixmap>
#include <QTimer>
#include <QDir>



KMainWindow::KMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    // 设置窗口为工具窗口、保持在最前且不抢夺焦点
    setWindowFlags(windowFlags()
        | Qt::WindowStaysOnTopHint);  // 保持窗口置顶

    // 使用 Windows API 设置扩展样式
    HWND hwnd = (HWND)this->winId();  // 获取窗口句柄
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE); // 获取当前扩展样式
    if (exStyle != 0) {
        exStyle |= 0x8000000; // 添加 WS_EX_NOACTIVATE 样式
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle); // 更新扩展样式
    }
    QString currentPath = QDir::currentPath();
    qDebug() << "Current working directory: " << currentPath;

    predictionEngine = new TextPredictionEngine("C:\\Users\\86133\\prediction.db", "commonwords.txt");

    ui.keyEntryButtonWidget->setVisible(false);
    ui.stackedWidget->setCurrentWidget(ui.singleCharPage);
    single_mode = true;
    mult_mode = false;
    ui.handwritingOperation->raise();
    settingsDialog = new SettingsDialog(this);

    storeInitialButtonTexts();
    storeEnglishModeTexts();
    storeNumberModeTexts();
    storePunctuationModeTexts();

    setupConnections();

    //确保窗口能够接受焦点
    //setFocusPolicy(Qt::StrongFocus);

    //强制获取焦点
    this->setFocus();

}

//void KMainWindow::focusInEvent(QFocusEvent* event) {
//    QMainWindow::focusInEvent(event);  // 确保基类事件处理
//
//    // 确保窗口已经完全初始化并获取焦点
//    this->setFocus();
//
//    HWND hwnd = (HWND)this->winId();
//    if (hwnd == nullptr) {
//        qDebug() << "Invalid HWND!";
//        return;
//    }
//    qDebug() << "Window handle (HWND):" << hwnd;
//
//    // 检查当前焦点是否为目标窗口
//    HWND hwndCurrentFocus = GetForegroundWindow();
//    if (hwndCurrentFocus != hwnd) {
//        qDebug() << "Focus is not on the target window!";
//        return;
//    }
//
//    // 调用初始化TSF
//    bool result = inputMethodEngine->initializeTSF(hwnd);
//    if (result) {
//        qDebug() << "TSF initialized successfully with QTextEdit!";
//    }
//    else {
//        qDebug() << "Failed to initialize TSF with QTextEdit.";
//    }
//}

KMainWindow::~KMainWindow()
{
    delete settingsDialog;
    delete predictionEngine;
}

void KMainWindow::setupConnections()
{
    connect(ui.btnPredictCandidate1, &QPushButton::clicked, this, &KMainWindow::updatePriority);
    connect(ui.btnPredictCandidate2, &QPushButton::clicked, this, &KMainWindow::updatePriority);
    connect(ui.btnPredictCandidate3, &QPushButton::clicked, this, &KMainWindow::updatePriority);
    connect(ui.btnPredictCandidate4, &QPushButton::clicked, this, &KMainWindow::updatePriority);
    //使用信号与槽连接多个按钮
    QList<QPushButton*>candidatesButtons = ui.frameCandidates->findChildren<QPushButton*>();
    for (QPushButton* button : candidatesButtons)
    {
        connect(button, &QPushButton::clicked, this, &KMainWindow::clickCandidateButtonsInput);
        connect(button, &QPushButton::clicked, ui.handwritingArea,&HandwritingArea::rewrite);
    }

    QList<QPushButton*>longCandidatesButtons = ui.longCandidatesButtonsWidget->findChildren<QPushButton*>();
    for (QPushButton* button : longCandidatesButtons)
    {
        connect(button, &QPushButton::clicked, this, &KMainWindow::clickCandidateButtonsInput);
        connect(button, &QPushButton::clicked, ui.handwritingAreaForMult, &HandwritingArea::rewrite);
    }

    QList<QPushButton*>keyEntryButtons = ui.keyEntryButtonWidget->findChildren<QPushButton*>();
    for (QPushButton* button : keyEntryButtons)
    {
        connect(button, &QPushButton::clicked, this, &KMainWindow::clickCandidateButtonsInput);
    }


    connect(ui.btnSingleCharMode, &QPushButton::clicked, this, &KMainWindow::switchToSingleMode);
    connect(ui.btnMultCharMode, &QPushButton::clicked, this, &KMainWindow::switchToMultMode);

    connect(ui.btnSwitchModeChinese, &QPushButton::clicked, this, &KMainWindow::switchToChineseMode);
    connect(ui.btnSwitchModeEnglish, &QPushButton::clicked, this, &KMainWindow::switchToEnglishMode);
    connect(ui.btnSwitchModeNumber, &QPushButton::clicked, this, &KMainWindow::switchToNumberMode);
    connect(ui.btnSwitchModePunctuation, &QPushButton::clicked, this, &KMainWindow::switchToPunctuationMode);

    connect(ui.btnUndo, &QPushButton::clicked, ui.handwritingArea, &HandwritingArea::undo);
    connect(ui.btnRewrite, &QPushButton::clicked, ui.handwritingArea, &HandwritingArea::rewrite);
    connect(ui.btnUndo, &QPushButton::clicked, ui.handwritingAreaForMult, &HandwritingArea::undo);
    connect(ui.btnRewrite, &QPushButton::clicked, ui.handwritingAreaForMult, &HandwritingArea::rewrite);

    connect(ui.handwritingArea, &HandwritingArea::needToReco, this, &KMainWindow::getRecognitonResult);
    connect(ui.handwritingAreaForMult, &HandwritingArea::needToReco, this, &KMainWindow::getRecognitonResult);

    connect(this, &KMainWindow::needToUpdateCandidateButtons, this, &KMainWindow::updateShortCandidateButtons);
    connect(ui.handwritingArea, &HandwritingArea::inkClear, this, &KMainWindow::resetButtonTextsToDefault);
    connect(ui.handwritingAreaForMult, &HandwritingArea::inkClear, this, &KMainWindow::resetButtonTextsToDefault);

    //连接设置按钮
    connect(ui.SettingsButton, &QPushButton::clicked, this, [this]() {
        //设置模态对话框
        settingsDialog->setWindowFlags(settingsDialog->windowFlags() | Qt::WindowStaysOnTopHint);
        settingsDialog->setWindowModality(Qt::ApplicationModal);
        settingsDialog->exec();
        });
    
    //处理设置界面发出的信号
    connect(settingsDialog, &SettingsDialog::penSettingsChanged, ui.handwritingArea, [this](int penWidth, QColor penColor) {
        ui.handwritingArea->setPenWidth(penWidth);
        ui.handwritingArea->setPenColor(penColor);
        });
    connect(settingsDialog, &SettingsDialog::penSettingsChanged, ui.handwritingAreaForMult, [this](int penWidth, QColor penColor) {
        ui.handwritingAreaForMult->setPenWidth(penWidth);
        ui.handwritingAreaForMult->setPenColor(penColor);
        });
    connect(settingsDialog, &SettingsDialog::buttonFontSizeChanged, this, [this](int fontSize) {
        for (auto& button : buttonInitialTexts.keys()) {
            if (button == ui.btnPossibleShow)continue;
            QFont font = button->font();
            font.setPointSize(fontSize);
            button->setFont(font);
        }
        });

}

void KMainWindow::storeInitialButtonTexts()
{
    buttonInitialTexts[ui.btnShortCandidate1] = ui.btnShortCandidate1->text();
    buttonInitialTexts[ui.btnShortCandidate2] = ui.btnShortCandidate2->text();
    buttonInitialTexts[ui.btnShortCandidate3] = ui.btnShortCandidate3->text();
    buttonInitialTexts[ui.btnShortCandidate4] = ui.btnShortCandidate4->text();
    buttonInitialTexts[ui.btnShortCandidate5] = ui.btnShortCandidate5->text();
    buttonInitialTexts[ui.btnShortCandidate6] = ui.btnShortCandidate6->text();
    buttonInitialTexts[ui.btnShortCandidate7] = ui.btnShortCandidate7->text();
    buttonInitialTexts[ui.btnShortCandidate8] = ui.btnShortCandidate8->text();
    buttonInitialTexts[ui.btnShortCandidate9] = ui.btnShortCandidate9->text();
    // 其他按钮同样保存

    buttonInitialTexts[ui.btnLongCandidate1] = ui.btnLongCandidate1->text();
    buttonInitialTexts[ui.btnLongCandidate2] = ui.btnLongCandidate2->text();
    buttonInitialTexts[ui.btnLongCandidate3] = ui.btnLongCandidate3->text();
    buttonInitialTexts[ui.btnLongCandidate4] = ui.btnLongCandidate4->text();
    buttonInitialTexts[ui.btnLongCandidate5] = ui.btnLongCandidate5->text();
    buttonInitialTexts[ui.btnLongCandidate6] = ui.btnLongCandidate6->text();
    buttonInitialTexts[ui.btnLongCandidate7] = ui.btnLongCandidate7->text();
    buttonInitialTexts[ui.btnLongCandidate8] = ui.btnLongCandidate8->text();
    buttonInitialTexts[ui.btnLongCandidate9] = ui.btnLongCandidate9->text();
    buttonInitialTexts[ui.btnLongCandidate10] = ui.btnLongCandidate10->text();

    buttonInitialTexts[ui.btnPredictCandidate1] = ui.btnPredictCandidate1->text();
    buttonInitialTexts[ui.btnPredictCandidate2] = ui.btnPredictCandidate2->text();
    buttonInitialTexts[ui.btnPredictCandidate3] = ui.btnPredictCandidate3->text();
    buttonInitialTexts[ui.btnPredictCandidate4] = ui.btnPredictCandidate4->text();

    buttonInitialTexts[ui.btnPossibleShow] = ui.btnPossibleShow->text();
    
    storeEnglishModeTexts();
    storeNumberModeTexts();
    storePunctuationModeTexts();
}
void KMainWindow::storeEnglishModeTexts()
{
    englishModeTexts[ui.btnKeyEntry1_1] = "a";
    englishModeTexts[ui.btnKeyEntry1_2] = "b";
    englishModeTexts[ui.btnKeyEntry1_3] = "c";
    englishModeTexts[ui.btnKeyEntry1_4] = "d";
    englishModeTexts[ui.btnKeyEntry1_5] = "e";
    englishModeTexts[ui.btnKeyEntry1_6] = "f";
    englishModeTexts[ui.btnKeyEntry1_7] = "g";
    englishModeTexts[ui.btnKeyEntry1_8] = "h";
    englishModeTexts[ui.btnKeyEntry1_9] = "i";
    englishModeTexts[ui.btnKeyEntry2_1] = "j";
    englishModeTexts[ui.btnKeyEntry2_2] = "k";
    englishModeTexts[ui.btnKeyEntry2_3] = "l";
    englishModeTexts[ui.btnKeyEntry2_4] = "m";
    englishModeTexts[ui.btnKeyEntry2_5] = "n";
    englishModeTexts[ui.btnKeyEntry2_6] = "o";
    englishModeTexts[ui.btnKeyEntry2_7] = "p";
    englishModeTexts[ui.btnKeyEntry2_8] = "q";
    englishModeTexts[ui.btnKeyEntry2_9] = "r";
    englishModeTexts[ui.btnKeyEntry3_1] = "s";
    englishModeTexts[ui.btnKeyEntry3_2] = "t";
    englishModeTexts[ui.btnKeyEntry3_3] = "u";
    englishModeTexts[ui.btnKeyEntry3_4] = "v";
    englishModeTexts[ui.btnKeyEntry3_5] = "w";
    englishModeTexts[ui.btnKeyEntry3_6] = "x";
    englishModeTexts[ui.btnKeyEntry3_7] = "y";
    englishModeTexts[ui.btnKeyEntry3_8] = "z";
    englishModeTexts[ui.btnKeyEntry3_9] = "";
    englishModeTexts[ui.btnKeyEntry4_1] = "A";
    englishModeTexts[ui.btnKeyEntry4_2] = "B";
    englishModeTexts[ui.btnKeyEntry4_3] = "C";
    englishModeTexts[ui.btnKeyEntry4_4] = "D";
    englishModeTexts[ui.btnKeyEntry4_5] = "E";
    englishModeTexts[ui.btnKeyEntry4_6] = "F";
    englishModeTexts[ui.btnKeyEntry4_7] = "G";
    englishModeTexts[ui.btnKeyEntry4_8] = "H";
    englishModeTexts[ui.btnKeyEntry4_9] = "I";
    englishModeTexts[ui.btnKeyEntry5_1] = "J";
    englishModeTexts[ui.btnKeyEntry5_2] = "K";
    englishModeTexts[ui.btnKeyEntry5_3] = "L";
    englishModeTexts[ui.btnKeyEntry5_4] = "M";
    englishModeTexts[ui.btnKeyEntry5_5] = "N";
    englishModeTexts[ui.btnKeyEntry5_6] = "O";
    englishModeTexts[ui.btnKeyEntry5_7] = "P";
    englishModeTexts[ui.btnKeyEntry5_8] = "Q";
    englishModeTexts[ui.btnKeyEntry5_9] = "R";
    englishModeTexts[ui.btnKeyEntry6_1] = "S";
    englishModeTexts[ui.btnKeyEntry6_2] = "T";
    englishModeTexts[ui.btnKeyEntry6_3] = "U";
    englishModeTexts[ui.btnKeyEntry6_4] = "V";
    englishModeTexts[ui.btnKeyEntry6_5] = "W";
    englishModeTexts[ui.btnKeyEntry6_6] = "X";
    englishModeTexts[ui.btnKeyEntry6_7] = "Y";
    englishModeTexts[ui.btnKeyEntry6_8] = "Z";
    englishModeTexts[ui.btnKeyEntry6_9] = "";
}

void KMainWindow::storeNumberModeTexts()
{
    numberModeTexts[ui.btnKeyEntry1_1] = "0";
    numberModeTexts[ui.btnKeyEntry1_2] = "1";
    numberModeTexts[ui.btnKeyEntry1_3] = "2";
    numberModeTexts[ui.btnKeyEntry1_4] = "3";
    numberModeTexts[ui.btnKeyEntry1_5] = "4";
    numberModeTexts[ui.btnKeyEntry1_6] = "5";
    numberModeTexts[ui.btnKeyEntry1_7] = "6";
    numberModeTexts[ui.btnKeyEntry1_8] = "7";
    numberModeTexts[ui.btnKeyEntry1_9] = "8";
    numberModeTexts[ui.btnKeyEntry2_1] = "9";
    numberModeTexts[ui.btnKeyEntry2_2] = "";
    numberModeTexts[ui.btnKeyEntry2_3] = "";
    numberModeTexts[ui.btnKeyEntry2_4] = "";
    numberModeTexts[ui.btnKeyEntry2_5] = "";
    numberModeTexts[ui.btnKeyEntry2_6] = "";
    numberModeTexts[ui.btnKeyEntry2_7] = "";
    numberModeTexts[ui.btnKeyEntry2_8] = "";
    numberModeTexts[ui.btnKeyEntry2_9] = "";
    numberModeTexts[ui.btnKeyEntry3_1] = QString::fromLocal8Bit("一");
    numberModeTexts[ui.btnKeyEntry3_2] = QString::fromLocal8Bit("二");
    numberModeTexts[ui.btnKeyEntry3_3] = QString::fromLocal8Bit("三");
    numberModeTexts[ui.btnKeyEntry3_4] = QString::fromLocal8Bit("四");
    numberModeTexts[ui.btnKeyEntry3_5] = QString::fromLocal8Bit("五");
    numberModeTexts[ui.btnKeyEntry3_6] = QString::fromLocal8Bit("六");
    numberModeTexts[ui.btnKeyEntry3_7] = QString::fromLocal8Bit("七");
    numberModeTexts[ui.btnKeyEntry3_8] = QString::fromLocal8Bit("八");
    numberModeTexts[ui.btnKeyEntry3_9] = QString::fromLocal8Bit("九");
    numberModeTexts[ui.btnKeyEntry4_1] = QString::fromLocal8Bit("十");
    numberModeTexts[ui.btnKeyEntry4_2] = QString::fromLocal8Bit("零");
    numberModeTexts[ui.btnKeyEntry4_3] = "";
    numberModeTexts[ui.btnKeyEntry4_4] = "";
    numberModeTexts[ui.btnKeyEntry4_5] = "";
    numberModeTexts[ui.btnKeyEntry4_6] = "";
    numberModeTexts[ui.btnKeyEntry4_7] = "";
    numberModeTexts[ui.btnKeyEntry4_8] = "";
    numberModeTexts[ui.btnKeyEntry4_9] = "";
    numberModeTexts[ui.btnKeyEntry5_1] = QString::fromLocal8Bit("壹");
    numberModeTexts[ui.btnKeyEntry5_2] = QString::fromLocal8Bit("贰");
    numberModeTexts[ui.btnKeyEntry5_3] = QString::fromLocal8Bit("叁");
    numberModeTexts[ui.btnKeyEntry5_4] = QString::fromLocal8Bit("肆");
    numberModeTexts[ui.btnKeyEntry5_5] = QString::fromLocal8Bit("伍");
    numberModeTexts[ui.btnKeyEntry5_6] = QString::fromLocal8Bit("陆");
    numberModeTexts[ui.btnKeyEntry5_7] = QString::fromLocal8Bit("柒");
    numberModeTexts[ui.btnKeyEntry5_8] = QString::fromLocal8Bit("捌");
    numberModeTexts[ui.btnKeyEntry5_9] = QString::fromLocal8Bit("玖");
    numberModeTexts[ui.btnKeyEntry6_1] = QString::fromLocal8Bit("拾");
    numberModeTexts[ui.btnKeyEntry6_2] = QString::fromLocal8Bit("佰");
    numberModeTexts[ui.btnKeyEntry6_3] = QString::fromLocal8Bit("仟");
    numberModeTexts[ui.btnKeyEntry6_4] = QString::fromLocal8Bit("万");
    numberModeTexts[ui.btnKeyEntry6_5] = "";
    numberModeTexts[ui.btnKeyEntry6_6] = "";
    numberModeTexts[ui.btnKeyEntry6_7] = "";
    numberModeTexts[ui.btnKeyEntry6_8] = "";
    numberModeTexts[ui.btnKeyEntry6_9] = "";
}
void KMainWindow::storePunctuationModeTexts()
{
    punctuationModeTexts[ui.btnKeyEntry1_1] = QString::fromLocal8Bit("，");
    punctuationModeTexts[ui.btnKeyEntry1_2] = QString::fromLocal8Bit("。");
    punctuationModeTexts[ui.btnKeyEntry1_3] = QString::fromLocal8Bit("、");
    punctuationModeTexts[ui.btnKeyEntry1_4] = QString::fromLocal8Bit("；");
    punctuationModeTexts[ui.btnKeyEntry1_5] = QString::fromLocal8Bit("：");
    punctuationModeTexts[ui.btnKeyEntry1_6] = QString::fromLocal8Bit("？");
    punctuationModeTexts[ui.btnKeyEntry1_7] = QString::fromLocal8Bit("！");
    punctuationModeTexts[ui.btnKeyEntry1_8] = QString::fromLocal8Bit("·");
    punctuationModeTexts[ui.btnKeyEntry1_9] = QString::fromLocal8Bit("…");
    punctuationModeTexts[ui.btnKeyEntry2_1] = QString::fromLocal8Bit("—");
    punctuationModeTexts[ui.btnKeyEntry2_2] = QString::fromLocal8Bit("～");
    punctuationModeTexts[ui.btnKeyEntry2_3] = QString::fromLocal8Bit("‘’");
    punctuationModeTexts[ui.btnKeyEntry2_4] = QString::fromLocal8Bit("“”");
    punctuationModeTexts[ui.btnKeyEntry2_5] = "/";
    punctuationModeTexts[ui.btnKeyEntry2_6] = "\\";
    punctuationModeTexts[ui.btnKeyEntry2_7] = QString::fromLocal8Bit("ˉ");
    punctuationModeTexts[ui.btnKeyEntry2_8] = QString::fromLocal8Bit("¨");
    punctuationModeTexts[ui.btnKeyEntry2_9] = QString::fromLocal8Bit("―");
    punctuationModeTexts[ui.btnKeyEntry3_1] = QString::fromLocal8Bit("ˇ");
    punctuationModeTexts[ui.btnKeyEntry3_2] = QString::fromLocal8Bit("∶");
    punctuationModeTexts[ui.btnKeyEntry3_3] = QString::fromLocal8Bit("﹕");
    punctuationModeTexts[ui.btnKeyEntry3_4] = QString::fromLocal8Bit("﹗");
    punctuationModeTexts[ui.btnKeyEntry3_5] = QString::fromLocal8Bit("｀");
    punctuationModeTexts[ui.btnKeyEntry3_6] = QString::fromLocal8Bit("＇");
    punctuationModeTexts[ui.btnKeyEntry3_7] = QString::fromLocal8Bit("〃");
    punctuationModeTexts[ui.btnKeyEntry3_8] = QString::fromLocal8Bit("＂");
    punctuationModeTexts[ui.btnKeyEntry3_9] = QString::fromLocal8Bit("々");
    punctuationModeTexts[ui.btnKeyEntry4_1] = QString::fromLocal8Bit("｜");
    punctuationModeTexts[ui.btnKeyEntry4_2] = QString::fromLocal8Bit("‖");
    punctuationModeTexts[ui.btnKeyEntry4_3] = "<>";
    punctuationModeTexts[ui.btnKeyEntry4_4] = QString::fromLocal8Bit("＜＞");
    punctuationModeTexts[ui.btnKeyEntry4_5] = QString::fromLocal8Bit("〈〉");
    punctuationModeTexts[ui.btnKeyEntry4_6] = QString::fromLocal8Bit("《》");
    punctuationModeTexts[ui.btnKeyEntry4_7] = QString::fromLocal8Bit("≮≯");
    punctuationModeTexts[ui.btnKeyEntry4_8] = "()";
    punctuationModeTexts[ui.btnKeyEntry4_9] = QString::fromLocal8Bit("（）");
    punctuationModeTexts[ui.btnKeyEntry5_1] = QString::fromLocal8Bit("〔〕");
    punctuationModeTexts[ui.btnKeyEntry5_2] = QString::fromLocal8Bit("［］");
    punctuationModeTexts[ui.btnKeyEntry5_3] = QString::fromLocal8Bit("〖〗");
    punctuationModeTexts[ui.btnKeyEntry5_4] = QString::fromLocal8Bit("【】");
    punctuationModeTexts[ui.btnKeyEntry5_5] = QString::fromLocal8Bit("「」");
    punctuationModeTexts[ui.btnKeyEntry5_6] = QString::fromLocal8Bit("『』");
    punctuationModeTexts[ui.btnKeyEntry5_7] = QString::fromLocal8Bit("﹝﹞");
    punctuationModeTexts[ui.btnKeyEntry5_8] = "{}";
    punctuationModeTexts[ui.btnKeyEntry5_9] = QString::fromLocal8Bit("｛｝");
    punctuationModeTexts[ui.btnKeyEntry6_1] = QString::fromLocal8Bit("︿﹀");
    punctuationModeTexts[ui.btnKeyEntry6_2] = QString::fromLocal8Bit("︽︾");
    punctuationModeTexts[ui.btnKeyEntry6_3] = QString::fromLocal8Bit("︹︺");
    punctuationModeTexts[ui.btnKeyEntry6_4] = QString::fromLocal8Bit("︵︶");
    punctuationModeTexts[ui.btnKeyEntry6_5] = QString::fromLocal8Bit("︻︼");
    punctuationModeTexts[ui.btnKeyEntry6_6] = QString::fromLocal8Bit("﹁﹂");
    punctuationModeTexts[ui.btnKeyEntry6_7] = QString::fromLocal8Bit("﹃﹄");
    punctuationModeTexts[ui.btnKeyEntry6_8] = QString::fromLocal8Bit("︷︸");
    punctuationModeTexts[ui.btnKeyEntry6_9] = "";
}

void KMainWindow::switchToSingleMode()
{
    if (single_mode)return;
    switchToChineseMode();
    ui.handwritingAreaForMult->rewrite();
    ui.stackedWidget->setCurrentWidget(ui.singleCharPage);
    ui.handwritingArea->recognizer->setSingleCharacterRecognitionMode();
    single_mode = true;
    mult_mode = false;
}

void KMainWindow::switchToMultMode()

{
    if (mult_mode)return;
    switchToChineseMode();
    ui.handwritingArea->rewrite();
    ui.stackedWidget->setCurrentWidget(ui.multCharPage);
    ui.handwritingAreaForMult->recognizer->setMultCharacterRecognitionMode();
    single_mode = false;
    mult_mode = true;
}

void KMainWindow::switchToChineseMode()
{
    ui.keyEntryButtonWidget->setVisible(false);
    if (single_mode)
    {
        ui.handwritingArea->setVisible(true);
    }
    else if (mult_mode)
    {
        ui.handwritingAreaForMult->setVisible(true);
    }
}

void KMainWindow::switchToEnglishMode()
{
    if (single_mode)
    {
        ui.handwritingArea->rewrite();
        ui.handwritingArea->setVisible(false);
    }
    else if (mult_mode)
    {
        ui.handwritingAreaForMult->rewrite();
        ui.handwritingAreaForMult->setVisible(false);
    }
    ui.keyEntryButtonWidget->setVisible(true);
    updateButtonTexts(englishModeTexts);
}

void KMainWindow::switchToNumberMode()
{
    if (single_mode)
    {
        ui.handwritingArea->rewrite();
        ui.handwritingArea->setVisible(false);
    }
    else if (mult_mode)
    {
        ui.handwritingAreaForMult->rewrite();
        ui.handwritingAreaForMult->setVisible(false);
    }
    ui.keyEntryButtonWidget->setVisible(true);
    updateButtonTexts(numberModeTexts);
}

void KMainWindow::switchToPunctuationMode()
{
    if (single_mode) {
        ui.handwritingArea->rewrite();
        ui.handwritingArea->setVisible(false);
    }
    else if (mult_mode) {
        ui.handwritingAreaForMult->rewrite();
        ui.handwritingAreaForMult->setVisible(false);
    }
    ui.keyEntryButtonWidget->setVisible(true);
    updateButtonTexts(punctuationModeTexts);
}

void KMainWindow::updateButtonTexts(const QMap<QPushButton*, QString>& newTexts)
{
    for (auto it = newTexts.begin(); it != newTexts.end(); ++it)
    {
        if (it.key())
        {
            it.key()->setText(it.value());//更新按钮文本
        }
    }
}

void KMainWindow::getRecognitonResult(IInkRecognitionAlternates* recoginitionResult) {
    long lCount = 0;
    if (recoginitionResult == NULL) {
        ui.handwritingArea->rewrite();
        ui.handwritingAreaForMult->rewrite();
        return;
    }
    recoginitionResult->get_Count(&lCount);

    QStringList recognitionTexts;
    for (LONG iItem = 0; iItem < lCount; iItem++) {
        IInkRecognitionAlternate* pIInkRecoAlternate = NULL;
        if (SUCCEEDED(recoginitionResult->Item(iItem, &pIInkRecoAlternate))) {
            BSTR bstr = NULL;
            if (SUCCEEDED(pIInkRecoAlternate->get_String(&bstr))) {
                recognitionTexts.append(QString::fromWCharArray(bstr));
                SysFreeString(bstr);
            }
            pIInkRecoAlternate->Release();
        }
    }
    qDebug() << "Recognition texts: " << recognitionTexts;
    qDebug() << "Recognition texts count: " << recognitionTexts.size();
    emit needToUpdateCandidateButtons(recognitionTexts);
}

void KMainWindow::updateShortCandidateButtons(const QStringList& texts) {
    QList<QPushButton*> shortCandidateButtons = {
        ui.btnShortCandidate1,
        ui.btnShortCandidate2,
        ui.btnShortCandidate3,
        ui.btnShortCandidate4,
        ui.btnShortCandidate5,
        ui.btnShortCandidate6,
        ui.btnShortCandidate7,
        ui.btnShortCandidate8,
        ui.btnShortCandidate9
    };
    QList<QPushButton*> longCandidateButtons = {
        ui.btnLongCandidate1,
        ui.btnLongCandidate2,
        ui.btnLongCandidate3,
        ui.btnLongCandidate4,
        ui.btnLongCandidate5,
        ui.btnLongCandidate6,
        ui.btnLongCandidate7,
        ui.btnLongCandidate8,
        ui.btnLongCandidate9,
        ui.btnLongCandidate10
    };
    QList<QPushButton*> predictCandidateButtons = {
        ui.btnPredictCandidate1,
        ui.btnPredictCandidate2,
        ui.btnPredictCandidate3,
        ui.btnPredictCandidate4
    };

    for (int i = 0; i < shortCandidateButtons.size(); ++i) {
        if (shortCandidateButtons[i]) {
            if (i < texts.size()) {
                if (i == 0) {
                    ui.btnPossibleShow->setText(texts[i]);
                    QVector<QString> predictions = predictionEngine->predict(texts[i]);
                    for (int j = 0; j<min(predictions.size(), predictCandidateButtons.size()); j++) {
                        predictCandidateButtons[j]->setText(predictions[j]);
                    }
                }
                shortCandidateButtons[i]->setText(texts[i]);
            }
            else {
                shortCandidateButtons[i]->setText("");
            }
        }
    }

    for (int i = 0; i < longCandidateButtons.size(); ++i) {
        if (longCandidateButtons[i]) {
            if (i < texts.size()) {
                longCandidateButtons[i]->setText(texts[i]);
            }
            else {
                longCandidateButtons[i]->setText("");
            }
        }
    }
}

void KMainWindow::resetButtonTextsToDefault()
{
    for (auto it = buttonInitialTexts.begin(); it != buttonInitialTexts.end(); ++it)
    {
        it.key()->setText(it.value());
    }
}

void KMainWindow::updatePriority()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button)
    {
        //获取按钮的文字，作为输入内容
        QString input = button->text();
        if (input == "")return;
        predictionEngine->updatePriority(input);
    }
}

void KMainWindow::clickCandidateButtonsInput() {
    // 获取发出信号的按钮
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        // 获取按钮的文字，作为输入内容
        QString input = button->text();
        if (input == "")return;
        HWND hwndFocused = GetForegroundWindow();
        std::wstring wstr = input.toStdWString();
        //SendMessageW(hwndFocused, WM_CHAR, 0, (LPARAM)wstr.c_str());
        SendKeys(hwndFocused, wstr);

    }
}

void KMainWindow::SendKeys(HWND hwnd, const std::wstring& text) {
    for (wchar_t ch : text) {
        SendMessageW(hwnd, WM_CHAR, ch, 0);
    }
}