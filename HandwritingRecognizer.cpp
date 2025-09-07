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
		pStrokes->get_Count(&count);//��ȡ�ʼ�����
		if (count > 0)
		{
			IInkStrokeDisp* lastStroke = nullptr;
			hr = pStrokes->Item(count - 1, &lastStroke);//��ȡ���һ���ʼ�
			if (SUCCEEDED(hr) && lastStroke)
			{
				//����һ��IInkStrokes�������ڴ�Ž���ɾ���ıʼ�
				IInkStrokes* pRemoveStrokes = nullptr;
				VARIANT varEmpty;
				VariantInit(&varEmpty);//��ʼ���յ�VARIANT����ʾ��ָ��StrokeIds
				
				hr = g_pIInkDisp->CreateStrokes(varEmpty, &pRemoveStrokes);//����CreateStrokes�����յ�IInkStrokes
				if (SUCCEEDED(hr) && pRemoveStrokes)
				{
					//�����һ���ʼ���ӵ���ʱIInkStrokes������
					pRemoveStrokes->Add(lastStroke);
					//ɾ���ñʼ�
					hr = g_pIInkDisp->DeleteStrokes(pRemoveStrokes);
					if (FAILED(hr))
					{
						qDebug()<<"Failed to delete the last stroke.";
					}
					pRemoveStrokes->Release();//�ͷ���ʱ��IInkStrokes����
				}
				lastStroke->Release();//�ͷ����һ���ʼ�����
			}
		}
		pStrokes->Release();//�ͷűʼ�����

	}
}

void HandwritingRecognizer::inputInit(HWND hwnd)
{
    if (g_pIInkCollector) {
        // ����Ѿ���ʼ�����ˣ�����Ҫ���³�ʼ��
        return;
    }

    CoInitialize(NULL);
    HRESULT hr;

    // ����ʶ����������
    hr = CoCreateInstance(CLSID_InkRecognizerContext, NULL, CLSCTX_INPROC_SERVER, IID_IInkRecognizerContext, (void**)&g_pIInkRecoContext);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkRecognizerContext";
        return;
    }

    // ����ī���ռ���
    hr = CoCreateInstance(CLSID_InkCollector, NULL, CLSCTX_INPROC_SERVER, IID_IInkCollector, (void**)&g_pIInkCollector);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkCollector";
        return;
    }

    // ��ȡī������
    hr = g_pIInkCollector->get_Ink(&g_pIInkDisp);
    if (FAILED(hr)) {
        qDebug() << "Failed to get Ink";
        return;
    }

    // �󶨴��ھ��
    hr = g_pIInkCollector->put_hWnd((long)hwnd);
    if (FAILED(hr)) {
        qDebug() << "Failed to bind HWND";
        return;
    }

    // ����ī���ռ�
    hr = g_pIInkCollector->put_Enabled(VARIANT_TRUE);
    if (FAILED(hr)) {
        qDebug() << "Failed to enable InkCollector";
    }

    // ��ȡ��ǰ�����Ѿ����ڵıʼ�
    IInkStrokes* pIInkStrokes = nullptr;
    hr = g_pIInkDisp->get_Strokes(&pIInkStrokes);
    if (SUCCEEDED(hr) && pIInkStrokes) {
        long count = 0;
        pIInkStrokes->get_Count(&count);  // ��ȡ���бʼ�������

        if (count > 0) {
            qDebug() << "Existing strokes found: " << count;

            // ������ڴ˴���ÿ���ʼ����д���
            for (long i = 0; i < count; ++i) {
                IInkStrokeDisp* pStroke = nullptr;
                hr = pIInkStrokes->Item(i, &pStroke);
                if (SUCCEEDED(hr) && pStroke) {
                    // ����ÿ���ʼ�
                    // ���ȡ�ʼ������ԡ�����ʶ���
                    pStroke->Release();  // �ͷűʼ�����
                }
            }
        }

        pIInkStrokes->Release();  // �ͷűʼ�����
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

    // ��ȡʶ������Ŀ
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
                if (SUCCEEDED(hr) && wcscmp(bstrName, L"Microsoft ����(����)��дʶ����") == 0) {
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

    // ���� IInkRecognizerGuide ʵ��
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

    // ����ʶ����������guide��
    IInkRecognizerGuide* pRecognizerGuide = nullptr;
    hr = CoCreateInstance(CLSID_InkRecognizerGuide, NULL, CLSCTX_INPROC_SERVER, IID_IInkRecognizerGuide, (void**)&pRecognizerGuide);
    if (FAILED(hr)) {
        qDebug() << "Failed to create InkRecognizerGuide for multi-character mode!";
        return;
    }

    InkRecoGuide recoGuide;
    RECT rect;

    // ���ö���ģʽ��ʶ�����򣬼�����д�����봰�ڴ�Сһ��
    rect.left = 0;
    rect.top = 0;
    rect.right = this->width;  // ʹ�ô��ڵĿ��
    rect.bottom = this->height;  // ʹ�ô��ڵĸ߶�

    recoGuide.rectWritingBox = rect;  // ����д������
    recoGuide.rectDrawnBox = rect;    // ���û�������
    recoGuide.cRows = 1;              // һ��
    recoGuide.cColumns = 0;           // 0��ʾû���������ƣ��������ַ�
    recoGuide.midline = -1;           // û�����ߣ��������ַ������жϣ�

    // �����õ�guide����Ӧ�õ�ʶ����
    pRecognizerGuide->put_GuideData(recoGuide);

    // ����ʶ�������ĵ�guide
    g_pIInkRecoContext->putref_Guide(pRecognizerGuide);

    // ����
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