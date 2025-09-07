#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QString>
#include <QDialog>
#include <QColor>
#include <QCloseEvent>
#include <ui_settings.h>


class SettingsDialog :public QDialog
{
	Q_OBJECT
public:
	explicit SettingsDialog(QWidget* parent = nullptr);
	~SettingsDialog();

signals:
	void penSettingsChanged(int penWidth, QColor penColor); //用于传递笔刷设置的信号
	void buttonFontSizeChanged(int fontSize); //用于传递字号设置的信号
	
private slots:
	void onPenWidthChanged(int index); //记录笔迹宽度变化
	void onPenColorChanged();          //记录笔迹颜色变化
	void onFontSizeChanged(int index); //记录字号大小变化
	void applyChange();
	void closeEvent(QCloseEvent* event)override;
private:
	Ui::Dialog ui;
	int m_fontSize;    //当前字号大小
	int m_penWidth;    //当前笔迹宽度
	QColor m_penColor; //当前笔迹颜色
	
	bool pen_hasChanged;
	bool font_hasChanged;


};

#endif 