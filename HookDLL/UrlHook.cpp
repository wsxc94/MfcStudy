#include "pch.h"
#include "UrlHook.h"

UrlHook::UrlHook()
{
	// 웹사이트 차단 리스트
	urlBlockList.emplace_back(".naver.");
	urlBlockList.emplace_back(".bing.");
}

UrlHook::~UrlHook()
{
}

void UrlHook::WidgetLoad()
{
	FindWindowWidget();
}

void UrlHook::FindWindowWidget()
{
	CoInitialize(NULL);

	HWND hwnd = NULL;	// 웹사이트의 정보를 받아올 핸들

	while (true)
	{
		//브라우저 위젯 핸들
		hwnd = FindWindowEx(0, hwnd, L"Chrome_WidgetWin_1", NULL);

		if (!hwnd) break; //핸들이 없으면 종료
		if (!IsWindowVisible(hwnd)) continue; // 윈도우가 존재하지않으면

		// UI 오토메이션
		CComQIPtr<IUIAutomation> uia;
		if (FAILED(uia.CoCreateInstance(CLSID_CUIAutomation)) || !uia) break;

		// UI 오토메이션 루트노드
		CComPtr<IUIAutomationElement> root;
		if (FAILED(uia->ElementFromHandle(hwnd, &root)) || !root) break;

		// UI 오토메이션 Condition
		CComPtr<IUIAutomationCondition> condition;
		uia->CreatePropertyCondition(UIA_ControlTypePropertyId, CComVariant(0xC354), &condition);

		// UI 오토메이션 자식노드
		CComPtr<IUIAutomationElement> edit;

		// 자식을 포함한 하위 목록 find
		if (FAILED(root->FindFirst(TreeScope_Descendants, condition, &edit)) || !edit) continue;

		// edit의 value pattern을 받아와 현재의 url 값을 받아온다.
		CComPtr<IUIAutomationValuePattern> value;
		edit->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(&value));
		BSTR bs;
		value->get_CurrentValue(&bs);

		USES_CONVERSION;
		std::string current_site = W2A(bs);

		TRACE(current_site.c_str()); //현재 사이트 디버그용
		
		BSTR url = _T("www.google.com");

		// 웹 차단 리스트 순회
		for (std::string s : urlBlockList) {
			if (current_site.find(s) != std::string::npos) {
				value->SetValue(url); // url value 값 수정
				//SetForegroundWindow(hwnd);
				PostMessage(hwnd, WM_KEYDOWN, VK_RETURN, 0); // enter 이벤트
				AfxMessageBox(_T("웹사이트 차단"));
			}
		}
		break;
	}
	CoUninitialize();
}
