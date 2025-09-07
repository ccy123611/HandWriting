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
	IInkRecognizerContext* g_pIInkRecoContext;//ʶ����
	IInkCollector* g_pIInkCollector;          //ī���ռ���
	IInkDisp* g_pIInkDisp;                    //ī������

	int width;
	int height;
};

#endif