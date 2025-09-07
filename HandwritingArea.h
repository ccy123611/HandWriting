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

	void rewrite();    //清除手写区域
	void undo();       //撤销笔画

	void setPenWidth(int width); //设置笔迹宽度
	void setPenColor(QColor color); //设置笔迹颜色

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
	QPoint lastPoint;  //记录上次鼠标的位置
	QPainterPath currentPath;  //当前绘制的路径
	QList<QPainterPath>strokes;//存储所有的笔记

	int m_penWidth;
	QColor m_penColor;


};


#endif
