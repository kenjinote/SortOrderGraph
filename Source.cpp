#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "gdiplus")
#include <windows.h>
#include <gdiplus.h>
#include <array>
#include <random>

#define MAX_NUM 100
#define TEST_COUNT 100

using namespace Gdiplus;

TCHAR szClassName[] = TEXT("Window");

void BubleSort(int* data, int size, int* pScanningCount, int* pExchangeCount)
{
	BOOL bFlag;
	int k = 0;
	do {
		bFlag = FALSE;
		for (int i = 0; i < size - 1 - k; ++i) {
			if (data[i] > data[i + 1]) {
				bFlag = TRUE;
				const int nTmp = data[i];
				data[i] = data[i + 1];
				data[i + 1] = nTmp;
				++*pExchangeCount;
			}
			++*pScanningCount;
		}
		++k;
	} while (bFlag);
}

void QuickSort(int start, int end, int *data, int* pScanningCount, int* pExchangeCount)
{
	int lower, upper, div, temp;
	if (start >= end) {
		return;
	}
	// 先頭の値を「適当な値」とする
	div = data[start];
	for (lower = start, upper = end; lower < upper;) {
		while (lower <= upper && data[lower] <= div) {
			++lower;
			++*pScanningCount;
		}
		while (lower <= upper&&data[upper] > div) {
			--upper;
			++*pScanningCount;
		}
		if (lower < upper) {
			temp = data[lower];
			data[lower] = data[upper];
			data[upper] = temp;
			++*pExchangeCount;
		}
	}
	//最初に選択した値を中央に移動する
	temp = data[start];
	data[start] = data[upper];
	data[upper] = temp;
	QuickSort(start, upper - 1, data, pScanningCount, pExchangeCount);
	QuickSort(upper + 1, end, data, pScanningCount, pExchangeCount);
}

void InitList(int* pList, int nSize)
{
	std::array<std::seed_seq::result_type, std::mt19937::state_size> seed_data;
	std::random_device rd;
	std::generate_n(seed_data.data(), seed_data.size(), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 engine(seq);
	for (int i = 0; i < nSize; ++i) {
		pList[i] = engine();
	}
}

void Shuffle(int* pList, int nSize)
{
	std::array<std::seed_seq::result_type, std::mt19937::state_size> seed_data;
	std::random_device rd;
	std::generate_n(seed_data.data(), seed_data.size(), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 engine(seq);
	for (int i = 0; i < nSize; ++i) {
		const int j = engine() % nSize;
		if (i != j) {
			int temp = pList[i];
			pList[i] = pList[j];
			pList[j] = temp;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static double averageBubleSort1[MAX_NUM];
	static double averageBubleSort2[MAX_NUM];
	static double averageQuickSort1[MAX_NUM];
	static double averageQuickSort2[MAX_NUM];
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("変換"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				Graphics g(hdc);
				g.SetSmoothingMode(SmoothingModeAntiAlias);
				g.Clear(Color(0, 0, 0));//g.Clear(0);でもよい
				double dmax = 0.0;
				for (int i = 0; i < MAX_NUM; ++i) {
					dmax = max(averageBubleSort1[i], dmax);
				}
				for (int i = 0; i < MAX_NUM; ++i) {
					const int x = i * rect.right / MAX_NUM;
					g.DrawLine(&Pen(Color(64 - 4, 64 - 4, 64 - 4), 20.0f), Point(x, rect.bottom), Point(x, rect.bottom - averageBubleSort1[i] * rect.bottom / dmax));
					g.DrawLine(&Pen(Color(64 + 4, 64 + 4, 64 + 4), 20.0f), Point(x, rect.bottom), Point(x, rect.bottom - averageBubleSort2[i] * rect.bottom / dmax));
					g.DrawLine(&Pen(Color(128 - 4, 128 - 4, 128 - 4), 20.0f), Point(x, rect.bottom), Point(x, rect.bottom - averageQuickSort1[i] * rect.bottom / dmax));
					g.DrawLine(&Pen(Color(128 + 4, 128 + 4, 128 + 4), 20.0f), Point(x, rect.bottom), Point(x, rect.bottom - averageQuickSort2[i] * rect.bottom / dmax));
				}
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			for (int i = 0; i < MAX_NUM; ++i) {
				int nSumBubleSort1 = 0;
				int nSumBubleSort2 = 0;
				int nSumQuickSort1 = 0;
				int nSumQuickSort2 = 0;
				for (int j = 0; j < TEST_COUNT; ++j) {
					int* pList1 = new int[i];
					if (!pList1) { MessageBox(hWnd, 0, 0, 0); return 0; }
					InitList(pList1, i);
					Shuffle(pList1, i);
					int* pList2 = new int[i];
					if (!pList2) { MessageBox(hWnd, 0, 0, 0); return 0; }
					CopyMemory(pList2, pList1, sizeof(int)*i);
					int nCountBubleSort1 = 0;
					int nCountBubleSort2 = 0;
					int nCountQuickSort1 = 0;
					int nCountQuickSort2 = 0;
					BubleSort(pList1, i, &nCountBubleSort1, &nCountBubleSort2);
					QuickSort(0, i - 1, pList2, &nCountQuickSort1, &nCountQuickSort2);
					nSumBubleSort1 += nCountBubleSort1;
					nSumBubleSort2 += nCountBubleSort2;
					nSumQuickSort1 += nCountQuickSort1;
					nSumQuickSort2 += nCountQuickSort2;
					delete[]pList1;
					delete[]pList2;
				}
				averageBubleSort1[i] = (double)nSumBubleSort1 / (double)TEST_COUNT;
				averageBubleSort2[i] = (double)nSumBubleSort2 / (double)TEST_COUNT;
				averageQuickSort1[i] = (double)nSumQuickSort1 / (double)TEST_COUNT;
				averageQuickSort2[i] = (double)nSumQuickSort2 / (double)TEST_COUNT;
			}
			InvalidateRect(hWnd, 0, 1);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	ULONG_PTR gdiToken;
	GdiplusStartupInput gdiSI;
	GdiplusStartup(&gdiToken, &gdiSI, NULL);
	MSG msg;
	const WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	const HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GdiplusShutdown(gdiToken);
	return (int)msg.wParam;
}
