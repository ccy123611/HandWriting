#ifndef HANDWRITINGAREA_H
#define HANDWRITINGAREA_H

#include"HandwritingRecognizer.h"
#include<msinkaut.h>
#include<QWidget>
#include<Qframe>
#include<QPainter>
#include<QPainterPath>
#include<QDebug>
#include<QMouseEvent>


class HandwritingArea :public QFrame
{
	Q_OBJECT;

public:
	explicit HandwritingArea(QWidget* parent = nullptr);
	~HandwritingArea();

	void rewrite();    //�����д����
	void undo();       //�����ʻ�

	void setPenWidth(int width); //���ñʼ����
	void setPenColor(QColor color); //���ñʼ���ɫ

	virtual bool eventFilter(QObject* watcher, QEvent* event)override;

	HandwritingRecognizer* recognizer;
signals:
	void needToReco(IInkRecognitionAlternates* recoginitionResult);
	void inkClear();

protected:
	void paintEvent(QPaintEvent* event)override;
	void mousePressEvent(QMouseEvent* event)override;
	void mouseMoveEvent(QMouseEvent* event)override;
	void mouseReleaseEvent(QMouseEvent* event)override;

private:
	bool isDrawing = false;
	QPoint lastPoint;  //��¼�ϴ�����λ��
	QPainterPath currentPath;  //��ǰ���Ƶ�·��
	QList<QPainterPath>strokes;//�洢���еıʼ�

	int m_penWidth;
	QColor m_penColor;


};


#endif
