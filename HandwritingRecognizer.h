#ifndef HANDWRITINGRECOGNIZER_H
#define HANDWRITINGRECOGNIZER_H

#include <msinkaut_i.c>
#include <msinkaut.h>
#include <QString>
#include <QList>
#include <QPainterPath>
#include <atlbase.h>

class HandwritingRecognizer
{
public:
	HandwritingRecognizer();
	~HandwritingRecognizer();

	void inputInit(HWND hwnd);
	HRESULT SetRecognizerLanguage(LANGID langId);
	void setSingleCharacterRecognitionMode();
	void setMultCharacterRecognitionMode();
	IInkRecognitionAlternates* RegDataEx();
	void setWidthAndHeight(int width, int height);

	void removeLastStroke();
	void clearInk();

private:
	IInkRecognizerContext* g_pIInkRecoContext;//识别器
	IInkCollector* g_pIInkCollector;          //墨迹收集器
	IInkDisp* g_pIInkDisp;                    //墨迹对象

	int width;
	int height;
};

#endif