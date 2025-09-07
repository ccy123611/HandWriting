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
	void penSettingsChanged(int penWidth, QColor penColor); //���ڴ��ݱ�ˢ���õ��ź�
	void buttonFontSizeChanged(int fontSize); //���ڴ����ֺ����õ��ź�
	
private slots:
	void onPenWidthChanged(int index); //��¼�ʼ���ȱ仯
	void onPenColorChanged();          //��¼�ʼ���ɫ�仯
	void onFontSizeChanged(int index); //��¼�ֺŴ�С�仯
	void applyChange();
	void closeEvent(QCloseEvent* event)override;
private:
	Ui::Dialog ui;
	int m_fontSize;    //��ǰ�ֺŴ�С
	int m_penWidth;    //��ǰ�ʼ����
	QColor m_penColor; //��ǰ�ʼ���ɫ
	
	bool pen_hasChanged;
	bool font_hasChanged;


};

#endif 