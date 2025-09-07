#include "HandwritingArea.h"

HandwritingArea::HandwritingArea(QWidget* parent)
	:QFrame(parent),
	isDrawing(false),
	recognizer(new HandwritingRecognizer),
	m_penWidth(5),  //Ĭ�ϱʼ����
	m_penColor(Qt::black) //Ĭ�ϱʼ���ɫ
{
	this->installEventFilter(this);

	//ȷ���ؼ��ܹ����ս���
	setFocusPolicy(Qt::StrongFocus);

	recognizer->inputInit((HWND)this->winId());//�����ھ�����ݸ�ʶ����
	recognizer->setWidthAndHeight(this->width(), this->height());
	recognizer->SetRecognizerLanguage(LANG_CHINESE_SIMPLIFIED);//�������ļ���ʶ����
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
		strokes.removeLast();  //�Ƴ����һ��·��
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
		//����״̬�¹��˵��������¼�
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
	painter.setRenderHint(QPainter::Antialiasing);//��������ݣ��ʻ�����ƽ��
	painter.setPen(QPen(m_penColor, m_penWidth));//���ñʵ���ɫ�ʹ�ϸ
	//�������д洢�ıʼ�
	for (const auto& path : strokes)
	{
		painter.drawPath(path);
	}

	//�����ǰ���ڻ��ƣ��ͻ��Ƶ�ǰ·������ָ������ƶ������У�
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
		currentPath = QPainterPath(event->pos());//����һ���µ�·�����ӵ�ǰλ�ÿ�ʼ
	}
}

void HandwritingArea::mouseMoveEvent(QMouseEvent* event)
{
	if (isDrawing)
	{
		currentPath.lineTo(event->pos());//��ǰ·������µĵ�
		update();//�������Ƹ��£�
	}
}

void HandwritingArea::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		isDrawing = false;
		strokes.append(currentPath);//���ɿ�ʱ����ǰ·�����뵽�ʼ��б�
		currentPath = QPainterPath();//��յ�ǰ·��

		//���ʼ����ݸ�ʶ�������д���
		update();
		IInkRecognitionAlternates* recognitionResult = recognizer->RegDataEx();//ʶ�������ܱʼ��б�
		emit needToReco(recognitionResult);//�����������/״̬����ź�
	}
}
