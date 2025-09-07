#include "SettingsDialog.h"
#include<QColor>

SettingsDialog::SettingsDialog(QWidget* parent)
	:QDialog(parent), m_fontSize(10), m_penWidth(5), m_penColor(Qt::black), pen_hasChanged(false), font_hasChanged(false)
{
	//ͨ��UI�ļ����ý���
	ui.setupUi(this);

	ui.comboBoxPenWidth->addItem(QString::fromLocal8Bit("��"), 7);//��ʾ���֡����û�����Ϊ7
	ui.comboBoxPenWidth->addItem(QString::fromLocal8Bit("��"), 5);
	ui.comboBoxPenWidth->addItem(QString::fromLocal8Bit("ϸ"), 3);

	ui.comboBoxFontSize->addItem(QString::fromLocal8Bit("��"), 16);//��ʾ���󡱣��û�����Ϊ16
	ui.comboBoxFontSize->addItem(QString::fromLocal8Bit("��"), 12);
	ui.comboBoxFontSize->addItem(QString::fromLocal8Bit("С"), 10);

	//����Ĭ��ѡ��
	ui.comboBoxPenWidth->setCurrentIndex(1); //Ĭ�ϡ��С�
	ui.comboBoxFontSize->setCurrentIndex(2); //Ĭ�ϡ�С��

	//�ź��������
	connect(ui.comboBoxPenWidth, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenWidthChanged(int)));
	connect(ui.comboBoxFontSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontSizeChanged(int)));
	connect(ui.btnApplyNewSettings, &QPushButton::clicked, this, &SettingsDialog::accept);
	connect(ui.btnSetSettingsDefault, &QPushButton::clicked, this, [this]() {
		m_penWidth = 5;     //Ĭ�Ͽ��
		m_fontSize = 10;    //Ĭ���ֺ�
		m_penColor = Qt::black;//Ĭ����ɫ

		ui.comboBoxFontSize->setCurrentIndex(2);//�ָ���С��
		ui.comboBoxPenWidth->setCurrentIndex(1);//�ָ����С�
		pen_hasChanged = true;
		font_hasChanged = true;
		});

	connect(ui.btnSetColor_black, &QPushButton::clicked, this, &SettingsDialog::onPenColorChanged);
	connect(ui.btnSetColor_red, &QPushButton::clicked, this, &SettingsDialog::onPenColorChanged);
	connect(ui.btnSetColor_yellow, &QPushButton::clicked, this, &SettingsDialog::onPenColorChanged);
	connect(ui.btnSetColor_green, &QPushButton::clicked, this, &SettingsDialog::onPenColorChanged);
	connect(ui.btnSetColor_blue, &QPushButton::clicked, this, &SettingsDialog::onPenColorChanged);

	connect(ui.btnApplyNewSettings, &QPushButton::clicked, this, &SettingsDialog::applyChange);
}

//��������
SettingsDialog::~SettingsDialog() {}

//�ۺ���������ʼ���ȱ仯
void SettingsDialog::onPenWidthChanged(int index)
{
	//��ȡѡ�бʼ����
	m_penWidth = ui.comboBoxPenWidth->itemData(index).toInt();
	pen_hasChanged = true;
	//emit penSettingsChanged(m_penWidth, m_penColor);

}

void SettingsDialog::onPenColorChanged()
{
	QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
	if (senderButton == ui.btnSetColor_black){
		m_penColor = Qt::black;
	}
	else if (senderButton == ui.btnSetColor_red) {
		m_penColor = Qt::red;
	}
	else if (senderButton == ui.btnSetColor_yellow) {
		m_penColor = Qt::yellow;
	}
	else if (senderButton == ui.btnSetColor_green) {
		m_penColor = Qt::green;
	}
	else if (senderButton == ui.btnSetColor_blue) {
		m_penColor = Qt::blue;
	}
	pen_hasChanged = true;
	//emit penSettingsChanged(m_penWidth, m_penColor);
}

//�ۺ����������ֺŴ�С�仯
void SettingsDialog::onFontSizeChanged(int index)
{
	m_fontSize = ui.comboBoxFontSize->itemData(index).toInt();
	font_hasChanged = true;
	//emit buttonFontSizeChanged(m_fontSize);
}

void SettingsDialog::applyChange()
{
	if (pen_hasChanged)
	{
		emit penSettingsChanged(m_penWidth, m_penColor);
	}
	if (font_hasChanged)
	{
		emit buttonFontSizeChanged(m_fontSize);
	}
}

void SettingsDialog::closeEvent(QCloseEvent* event)
{
	//�Զ���ر���Ϊ
	//�������κ��źţ�ֱ�ӽ��ܹر��¼�
	pen_hasChanged = false;
	font_hasChanged = false;
	event->accept();
}