#include "HandwritingArea.h"

HandwritingArea::HandwritingArea(QWidget* parent)
	:QFrame(parent),
	isDrawing(false),
	recognizer(new HandwritingRecognizer),
	m_penWidth(5),  //默认笔迹宽度
	m_penColor(Qt::black) //默认笔迹颜色
{
	this->installEventFilter(this);

	//确保控件能够接收焦点
	setFocusPolicy(Qt::StrongFocus);

	recognizer->inputInit((HWND)this->winId());//将窗口句柄传递给识别器
	recognizer->setWidthAndHeight(this->width(), this->height());
	recognizer->SetRecognizerLanguage(LANG_CHINESE_SIMPLIFIED);//设置中文简体识别器
	recognizer->setSingleCharacterRecognitionMode();

	qDebug() << "HandwritingArea initialize";
}

HandwritingArea::~HandwritingArea()
{
	delete recognizer;
}

void HandwritingArea::rewrite()
{
	qDebug() << "Clear button clicked!";
	strokes.clear();
	recognizer->clearInk();
	update();
	emit inkClear();
}

void HandwritingArea::undo()
{
	qDebug() << "Undo button clicked!";
	if (!strokes.isEmpty())
	{
		strokes.removeLast();  //移除最后一个路径
		recognizer->removeLastStroke();
		update();
		if (strokes.isEmpty())
		{
			rewrite();
		}
		else
		{
			IInkRecognitionAlternates* recognitionResult = recognizer->RegDataEx();
			emit needToReco(recognitionResult);
		}

	}
}

void HandwritingArea::setPenWidth(int width)
{
	m_penWidth = width;
	update();
}

void HandwritingArea::setPenColor(QColor color)
{
	m_penColor = color;
	update();
}

bool HandwritingArea::eventFilter(QObject* obj, QEvent* event)
{
	if (!this->isEnabled())
	{
		//禁用状态下过滤掉鼠标相关事件
		if (event->type() == QEvent::MouseButtonPress ||
			event->type() == QEvent::MouseMove ||
			event->type() == QEvent::MouseButtonRelease)
		{
			return true;
		}
	}
	return QFrame::eventFilter(obj, event);
}

void HandwritingArea::paintEvent(QPaintEvent* event)
{
	if (!this->isEnabled())return;
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);//开启抗锯齿，笔画更加平滑
	painter.setPen(QPen(m_penColor, m_penWidth));//设置笔的颜色和粗细
	//绘制所有存储的笔记
	for (const auto& path : strokes)
	{
		painter.drawPath(path);
	}

	//如果当前正在绘制，就绘制当前路径（手指或鼠标移动过程中）
	if (isDrawing)
	{
		painter.drawPath(currentPath);
	}
}

void HandwritingArea::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		isDrawing = true;
		currentPath = QPainterPath(event->pos());//创建一个新的路径，从当前位置开始
	}
}

void HandwritingArea::mouseMoveEvent(QMouseEvent* event)
{
	if (isDrawing)
	{
		currentPath.lineTo(event->pos());//向当前路径添加新的点
		update();//触发绘制更新；
	}
}

void HandwritingArea::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		isDrawing = false;
		strokes.append(currentPath);//在松开时将当前路径加入到笔迹列表
		currentPath = QPainterPath();//清空当前路径

		//将笔迹传递给识别器进行处理
		update();
		IInkRecognitionAlternates* recognitionResult = recognizer->RegDataEx();//识别器接受笔迹列表
		emit needToReco(recognitionResult);//发出绘制完成/状态变更信号
	}
}
