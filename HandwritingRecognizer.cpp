#include "HandwritingRecognizer.h"
#include <QDebug>

HandwritingRecognizer::HandwritingRecognizer()
	:g_pIInkRecoContext(nullptr), g_pIInkCollector(nullptr), g_pIInkDisp(nullptr)
{
}
HandwritingRecognizer::~HandwritingRecognizer()
{
	if (g_pIInkRecoContext)
	{
		g_pIInkRecoContext->Release();
	}
	if (g_pIInkCollector)
	{
		g_pIInkCollector->Release();
	}
	if (g_pIInkDisp)
	{
		g_pIInkDisp->Release();
	}
	CoUninitialize();
}

void HandwritingRecognizer::clearInk()
{
	if (g_pIInkDisp)
	{
		g_pIInkDisp->DeleteStrokes();
	}
}

void HandwritingRecognizer::removeLastStroke()
{
	if (!g_pIInkDisp)return;
	IInkStrokes* pStrokes = nullptr;
	HRESULT hr = g_pIInkDisp->get_Strokes(&pStrokes);
	if (SUCCEEDED(hr) && pStrokes)
	{
		long count = 0;
		pStrokes->get_Count(&count);//获取笔迹数量
		if (count > 0)
		{
			IInkStrokeDisp* lastStroke = nullptr;
			hr = pStrokes->Item(count - 1, &lastStroke);//获取最后一条笔迹
			if (SUCCEEDED(hr) && lastStroke)
			{
				//创建一个IInkStrokes对象，用于存放将被删除的笔迹
				IInkStrokes* pRemoveStrokes = nullptr;
				VARIANT varEmpty;
				VariantInit(&varEmpty);//初始化空的VARIANT，表示不指定StrokeIds
				
				hr = g_pIInkDisp->CreateStrokes(varEmpty, &pRemoveStrokes);//调用CreateStrokes创建空的IInkStrokes
				if (SUCCEEDED(hr) && pRemoveStrokes)
				{
					//将最后一条笔迹添加到临时IInkStrokes对象中
					pRemoveStrokes->Add(lastStroke);
					//删除该笔迹
					hr = g_pIInkDisp->DeleteStrokes(pRemoveStrokes);
					if (FAILED(hr))
					{
						qDebug()<<"Failed to delete the last stroke.";
					}
					pRemoveStrokes->Release();//释放临时的IInkStrokes对象
				}
				lastStroke->Release();//释放最后一条笔迹对象
			}
		}
		pStrokes->Release();//释放笔迹集合

	}
}

void HandwritingRecognizer::inputInit(HWND hwnd)
{
    if (g_pIInkCollector) {
        // 如果已经初始化过了，则不需要重新初始化
        return;
    }

    CoInitialize(NULL);
    HRESULT hr;

    // 创建识别器上下文
    hr = CoCreateInstance(CLSID_InkRecognizerContext, NULL, CLSCTX_INPROC_SERVER, IID_IInkRecognizerContext, (void**)&g_pIInkRecoContext);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkRecognizerContext";
        return;
    }

    // 创建墨迹收集器
    hr = CoCreateInstance(CLSID_InkCollector, NULL, CLSCTX_INPROC_SERVER, IID_IInkCollector, (void**)&g_pIInkCollector);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkCollector";
        return;
    }

    // 获取墨迹对象
    hr = g_pIInkCollector->get_Ink(&g_pIInkDisp);
    if (FAILED(hr)) {
        qDebug() << "Failed to get Ink";
        return;
    }

    // 绑定窗口句柄
    hr = g_pIInkCollector->put_hWnd((long)hwnd);
    if (FAILED(hr)) {
        qDebug() << "Failed to bind HWND";
        return;
    }

    // 开启墨迹收集
    hr = g_pIInkCollector->put_Enabled(VARIANT_TRUE);
    if (FAILED(hr)) {
        qDebug() << "Failed to enable InkCollector";
    }

    // 获取当前窗口已经存在的笔迹
    IInkStrokes* pIInkStrokes = nullptr;
    hr = g_pIInkDisp->get_Strokes(&pIInkStrokes);
    if (SUCCEEDED(hr) && pIInkStrokes) {
        long count = 0;
        pIInkStrokes->get_Count(&count);  // 获取现有笔迹的数量

        if (count > 0) {
            qDebug() << "Existing strokes found: " << count;

            // 你可以在此处对每个笔迹进行处理
            for (long i = 0; i < count; ++i) {
                IInkStrokeDisp* pStroke = nullptr;
                hr = pIInkStrokes->Item(i, &pStroke);
                if (SUCCEEDED(hr) && pStroke) {
                    // 处理每个笔迹
                    // 如获取笔迹的特性、进行识别等
                    pStroke->Release();  // 释放笔迹对象
                }
            }
        }

        pIInkStrokes->Release();  // 释放笔迹集合
    }
    else {
        qDebug() << "Failed to get existing strokes!";
    }
}

HRESULT HandwritingRecognizer::SetRecognizerLanguage(LANGID langId)
{
    CComPtr<IInkRecognizers> spIInkRecognizers;
    HRESULT hr = spIInkRecognizers.CoCreateInstance(CLSID_InkRecognizers);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkRecognizers instance.";
        return hr;
    }

    // 获取识别器数目
    LONG count = 0;
    hr = spIInkRecognizers->get_Count(&count);
    if (FAILED(hr)) {
        qDebug() << "Failed to get InkRecognizers count.";
        return hr;
    }

    if (count > 0) {
        CComPtr<IInkRecognizer> spIInkRecognizer;
        for (LONG i = 0; i < count; i++) {
            hr = spIInkRecognizers->Item(i, &spIInkRecognizer);
            if (FAILED(hr)) {
                continue;
            }

            CComVariant vLanguages;
            hr = spIInkRecognizer->get_Languages(&vLanguages);
            if (SUCCEEDED(hr) && VT_ARRAY == (VT_ARRAY & vLanguages.vt) && vLanguages.parray && vLanguages.parray->rgsabound[0].cElements > 0) {
                CComBSTR bstrName;
                hr = spIInkRecognizer->get_Name(&bstrName);
                if (SUCCEEDED(hr) && wcscmp(bstrName, L"Microsoft 中文(简体)手写识别器") == 0) {
                    g_pIInkRecoContext->Release();
                    hr = spIInkRecognizer->CreateRecognizerContext(&g_pIInkRecoContext);
                    if (FAILED(hr)) {
                        qDebug() << "Failed to create recognizer context for the chosen recognizer.";
                        return hr;
                    }
                    break;
                }
            }

            spIInkRecognizer.Release();
        }
    }
    return hr;
}

void HandwritingRecognizer::setWidthAndHeight(int width, int height)
{
    this->width = width;
    this->height = height;
}

void HandwritingRecognizer::setSingleCharacterRecognitionMode()
{
    HRESULT hr;

    // 创建 IInkRecognizerGuide 实例
    CComPtr<IInkRecognizerGuide> pRecognizerGuide;
    hr = CoCreateInstance(CLSID_InkRecognizerGuide, NULL, CLSCTX_INPROC_SERVER, IID_IInkRecognizerGuide, (void**)&pRecognizerGuide);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkRecognizerGuide";
        return;
    }

    InkRecoGuide recoGuide;
    RECT rect;
    rect.bottom = this->height;  // Set to 2 instead of 1 for correct recognition range
    rect.left = 0;
    rect.right = this->width;
    rect.top = 0;

    recoGuide.rectWritingBox = rect;      // Define the writing area for a single character
    recoGuide.rectDrawnBox = rect;        // Define the drawn area for a single character
    recoGuide.cRows = 1;                  // Only one row for single character mode
    recoGuide.cColumns = 1;               // Only one column for single character mode
    recoGuide.midline = -1;               // No specific midline for single character mode

    hr = pRecognizerGuide->put_GuideData(recoGuide);
    if (FAILED(hr)) {
        qDebug() << "Failed to set guide data";
        return;
    }

    // Apply this guide to the recognizer context
    hr = g_pIInkRecoContext->putref_Guide(pRecognizerGuide);
    if (FAILED(hr)) {
        qDebug() << "Failed to set guide to recognition context";
    }
}

void HandwritingRecognizer::setMultCharacterRecognitionMode()
{
    HRESULT hr;

    // 创建识别器引导（guide）
    IInkRecognizerGuide* pRecognizerGuide = nullptr;
    hr = CoCreateInstance(CLSID_InkRecognizerGuide, NULL, CLSCTX_INPROC_SERVER, IID_IInkRecognizerGuide, (void**)&pRecognizerGuide);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkRecognizerGuide for multi-character mode!";
        return;
    }

    InkRecoGuide recoGuide;
    RECT rect;

    // 设置多字模式的识别区域，假设手写区域与窗口大小一致
    rect.left = 0;
    rect.top = 0;
    rect.right = this->width;  // 使用窗口的宽度
    rect.bottom = this->height;  // 使用窗口的高度

    recoGuide.rectWritingBox = rect;  // 设置写入区域
    recoGuide.rectDrawnBox = rect;    // 设置绘制区域
    recoGuide.cRows = 1;              // 一行
    recoGuide.cColumns = 0;           // 0表示没有列数限制，允许多个字符
    recoGuide.midline = -1;           // 没有中线（不进行字符方向判断）

    // 将设置的guide数据应用到识别器
    pRecognizerGuide->put_GuideData(recoGuide);

    // 设置识别上下文的guide
    g_pIInkRecoContext->putref_Guide(pRecognizerGuide);

    // 清理
    pRecognizerGuide->Release();

    qDebug() << "Switched to multi-character recognition mode!";
}

IInkRecognitionAlternates* HandwritingRecognizer::RegDataEx()
{
    IInkStrokes* pIInkStrokes = NULL;
    HRESULT hr = g_pIInkDisp->get_Strokes(&pIInkStrokes);
    if (SUCCEEDED(hr)) {
        hr = g_pIInkRecoContext->putref_Strokes(pIInkStrokes);
        if (SUCCEEDED(hr)) {
            IInkRecognitionResult* pIInkRecoResult = NULL;
            InkRecognitionStatus RecognitionStatus = IRS_NoError;
            hr = g_pIInkRecoContext->Recognize(&RecognitionStatus, &pIInkRecoResult);
            if (SUCCEEDED(hr) && pIInkRecoResult) {
                IInkRecognitionAlternates* spIInkRecoAlternates;
                hr = pIInkRecoResult->AlternatesFromSelection(0, -1, 10, &spIInkRecoAlternates);
                if (SUCCEEDED(hr)) {
                    return spIInkRecoAlternates;
                }
            }
        }
        pIInkStrokes->Release();
        return NULL;
    }
}