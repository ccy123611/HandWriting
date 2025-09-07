#include "SettingsDialog.h"
#include<QColor>

SettingsDialog::SettingsDialog(QWidget* parent)
	:QDialog(parent), m_fontSize(10), m_penWidth(5), m_penColor(Qt::black), pen_hasChanged(false), font_hasChanged(false)
{
	//通过UI文件设置界面
	ui.setupUi(this);

	ui.comboBoxPenWidth->addItem(QString::fromLocal8Bit("粗"), 7);//显示“粗”，用户数据为7
	ui.comboBoxPenWidth->addItem(QString::fromLocal8Bit("中"), 5);
	ui.comboBoxPenWidth->addItem(QString::fromLocal8Bit("细"), 3);

	ui.comboBoxFontSize->addItem(QString::fromLocal8Bit("大"), 16);//显示“大”，用户数据为16
	ui.comboBoxFontSize->addItem(QString::fromLocal8Bit("中"), 12);
	ui.comboBoxFontSize->addItem(QString::fromLocal8Bit("小"), 10);

	//设置默认选项
	ui.comboBoxPenWidth->setCurrentIndex(1); //默认“中”
	ui.comboBoxFontSize->setCurrentIndex(2); //默认“小”

	//信号与槽连接
	connect(ui.comboBoxPenWidth, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenWidthChanged(int)));
	connect(ui.comboBoxFontSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontSizeChanged(int)));
	connect(ui.btnApplyNewSettings, &QPushButton::clicked, this, &SettingsDialog::accept);
	connect(ui.btnSetSettingsDefault, &QPushButton::clicked, this, [this]() {
		m_penWidth = 5;     //默认宽度
		m_fontSize = 10;    //默认字号
		m_penColor = Qt::black;//默认颜色

		ui.comboBoxFontSize->setCurrentIndex(2);//恢复“小”
		ui.comboBoxPenWidth->setCurrentIndex(1);//恢复“中”
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

//析构函数
SettingsDialog::~SettingsDialog() {}

//槽函数：处理笔迹宽度变化
void SettingsDialog::onPenWidthChanged(int index)
{
	//获取选中笔迹宽度
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

//槽函数，处理字号大小变化
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
	//自定义关闭行为
	//不触发任何信号，直接接受关闭事件
	pen_hasChanged = false;
	font_hasChanged = false;
	event->accept();
}