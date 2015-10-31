/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_win32.cc
 *
 *  Fri Dec 28 18:45:52 CET 2012
 *  Copyright 2012 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "nativewindow_win32.h"

#include "window.h"

#include <cstring>

namespace GUI {

LRESULT CALLBACK NativeWindowWin32::dialogProc(HWND hwnd, UINT msg,
                                               WPARAM wp, LPARAM lp)
{
	NativeWindowWin32 *native =
		(NativeWindowWin32 *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	// NOTE: 'native' is nullptr intil the WM_CREATE message has been handled.
	if(!native)
	{
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	Window *window = native->window;

	switch(msg) {
	case WM_SIZE:
		{
			static bool first = true;
			if(!first)
			{
				ResizeEvent *e = new ResizeEvent();
				e->width = LOWORD(lp);
				e->height = HIWORD(lp);
				native->event = e;
				first = false;
			}
		}
		break;

	case WM_MOVE:
		{
//      MoveEvent *e = new MoveEvent();
//      e->x = (int)(short) LOWORD(lp);
//      e->y = (int)(short) HIWORD(lp);
//      native->event = e;
		}
		break;

	case WM_CLOSE:
		{
			CloseEvent *e = new CloseEvent();
			native->event = e;
		}
		break;
//		HWND child, old;
//		old	= 0;

//		numDialogs--;

//		while(old != (child = GetNextDlgGroupItem(hwnd, hwnd, false))) {
//			old = child;
//			EndDialog(child, 0);
//		}

//		if(numDialogs) EndDialog(hwnd, 0);
//		else PostQuitMessage(0);
//		return 0;
	case WM_MOUSEMOVE:
		{
			MouseMoveEvent *e = new MouseMoveEvent();
			e->x = (int)(short) LOWORD(lp);
			e->y = (int)(short) HIWORD(lp);
			native->event = e;
		}
		break;

	case WM_MOUSEWHEEL:
		{
			ScrollEvent *e = new ScrollEvent();

			// NOTE: lp is coordinates in screen space, not client space.
			POINT p;
			p.x = (int)(short) LOWORD(lp);
			p.y = (int)(short) HIWORD(lp);
			ScreenToClient(hwnd, &p);

			e->x = p.x;
			e->y = p.y;
			e->delta = -1 * (short)HIWORD(wp) / 60;
			native->event = e;
		}
		break;

	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
		{
			ButtonEvent *e = new ButtonEvent();
			e->x = (int)(short) LOWORD(lp);
			e->y = (int)(short) HIWORD(lp);

			if(msg == WM_LBUTTONUP ||
			   msg == WM_LBUTTONDBLCLK ||
			   msg == WM_LBUTTONDOWN)
			{
				e->button = ButtonEvent::Left;
			}
			else if(msg == WM_RBUTTONUP ||
			        msg == WM_RBUTTONDBLCLK ||
			        msg == WM_RBUTTONDOWN)
			{
				e->button = ButtonEvent::Middle;
			}
			else if(msg == WM_MBUTTONUP ||
			        msg == WM_MBUTTONDBLCLK ||
			        msg == WM_MBUTTONDOWN)
			{
				e->button = ButtonEvent::Right;
			}
			else
			{
				delete e;
				break; // unknown button
			}

			if(msg == WM_LBUTTONUP ||
			   msg == WM_RBUTTONUP ||
			   msg == WM_MBUTTONUP)
			{
				e->direction = ButtonEvent::Up;
			}
			else if(msg == WM_LBUTTONDOWN ||
			        msg == WM_RBUTTONDOWN ||
			        msg == WM_MBUTTONDOWN)
			{
				e->direction = ButtonEvent::Down;
			}
			else
			{
	      delete e;
	      break; // unknown direction
			}

			e->doubleclick = (msg == WM_LBUTTONDBLCLK ||
			                  msg == WM_RBUTTONDBLCLK ||
			                  msg == WM_MBUTTONDBLCLK);

			native->event = e;
		}
		break;

	case WM_KEYDOWN:
		{
			KeyEvent *e = new KeyEvent();
			//printf("wp: %d\n", wp);
			switch(wp) {
			case 37: e->keycode = KeyEvent::KeyLeft; break;
			case 39: e->keycode = KeyEvent::KeyRight; break;
			case 38: e->keycode = KeyEvent::KeyUp; break;
			case 40: e->keycode = KeyEvent::KeyDown; break;
			case 8:  e->keycode = KeyEvent::KeyBackspace; break;
			case 46: e->keycode = KeyEvent::KeyDelete; break;
			case 36: e->keycode = KeyEvent::KeyHome; break;
			case 35: e->keycode = KeyEvent::KeyEnd; break;
			case 33: e->keycode = KeyEvent::KeyPageUp; break;
			case 34: e->keycode = KeyEvent::KeyPageDown; break;
			case 13: e->keycode = KeyEvent::KeyEnter; break;
			default: e->keycode = KeyEvent::KeyUnknown; break;
			}
			e->text = "";
			e->direction = KeyEvent::Up;
			native->event = e;
		}
		break;

	case WM_CHAR:
		{
			//printf("WM_CHAR %d %d\n", (int)lp, (int)wp);
			if(wp >= ' ') // Filter control chars.
			{
				KeyEvent *e = new KeyEvent();
				e->keycode = KeyEvent::KeyCharacter;
				e->text += (char)wp;
				e->direction = KeyEvent::Up;
				native->event = e;
			}
		}
		break;

	case WM_PAINT:
		{
			RepaintEvent *e = new RepaintEvent();
			e->x = 0;
			e->y = 0;
			e->width = 100;
			e->height = 100;
			native->event = e;

			// Move to window.h (in class)
			HDC pDC;
			HBITMAP old;
			HBITMAP ourbitmap;
			int * framebuf;
			PixelBuffer &px = window->wpixbuf;

			{ // Create bitmap (move to window.cc)
				HDC hDC;
				BITMAPINFO bitmapinfo;
				hDC = CreateCompatibleDC(nullptr);
				bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bitmapinfo.bmiHeader.biWidth = px.width;
				bitmapinfo.bmiHeader.biHeight = -px.height; /* top-down */
				bitmapinfo.bmiHeader.biPlanes = 1;
				bitmapinfo.bmiHeader.biBitCount = 32;
				bitmapinfo.bmiHeader.biCompression = BI_RGB;
				bitmapinfo.bmiHeader.biSizeImage = 0;
				bitmapinfo.bmiHeader.biClrUsed = 256;
				bitmapinfo.bmiHeader.biClrImportant = 256;
				ourbitmap=CreateDIBSection(hDC, &bitmapinfo,
				                           DIB_RGB_COLORS, (void**)&framebuf, 0, 0);
				pDC=CreateCompatibleDC(nullptr);
				old = (HBITMAP__*)SelectObject(pDC, ourbitmap);
				DeleteDC(hDC);
			}

			{ // Copy PixelBuffer to framebuffer (move to window.cc)
				int i,j,k;
				for (k=0,i=0;i<(int)px.height;i++)
				{
					for (j=0;j<(int)px.width;j++,k++)
					{
						*(framebuf+k)=RGB(px.buf[(j + i * px.width) * 3 + 2],
						                  px.buf[(j + i * px.width) * 3 + 1],
						                  px.buf[(j + i * px.width) * 3 + 0]);
					}
				}
			}

			PAINTSTRUCT	ps;
			HDC	hdc = BeginPaint(native->m_hwnd, &ps);
			BitBlt(hdc, 0, 0, px.width, px.height, pDC, 0, 0, SRCCOPY);
			EndPaint(native->m_hwnd, &ps);

			{ // Destroy bitmap (move to window.cc)
				SelectObject(pDC,old);
				DeleteDC(pDC);
				DeleteObject(ourbitmap);
			}
		}

		return DefWindowProc(hwnd, msg, wp, lp);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

NativeWindowWin32::NativeWindowWin32(Window *window)
	: window(window)
{
	WNDCLASSEX wcex;
	WNDID wndId;

	std::memset(&wcex, 0, sizeof(wcex));

	//Time to register a window class.
	//Generic flags and everything. cbWndExtra is the size of a pointer to an
	// object - we need this in the wndproc handler.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;//class_style;
	wcex.lpfnWndProc = (WNDPROC)dialogProc;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// Set data:
	wcex.cbWndExtra = sizeof(NativeWindowWin32*); // Size of data.
	wcex.hInstance = GetModuleHandle(nullptr);

	//	if(ex_style && WS_EX_TRANSPARENT == WS_EX_TRANSPARENT) {
	//		wcex.hbrBackground = nullptr;
	//	} else {
	wcex.hbrBackground = nullptr;//(HBRUSH) COLOR_BACKGROUND + 1;
	//	}

	wcex.lpszClassName = m_className = strdup("DrumGizmoClass");

	RegisterClassEx(&wcex);

	/*
	if(parent) {
		style = style | WS_CHILD;
		wndId = parent->getWndId();
	} else {
	*/
	//style = style | WS_OVERLAPPEDWINDOW;
	wndId = 0;
	//	}

	m_hwnd = CreateWindowEx(0/*ex_style*/, m_className,
	                        "DGBasisWidget",
	                        (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
	                        window->x(), window->y(),
	                        window->width(), window->height(),
	                        wndId, nullptr,
	                        GetModuleHandle(nullptr), nullptr);

	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
}

NativeWindowWin32::~NativeWindowWin32()
{
	UnregisterClass(m_className, GetModuleHandle(nullptr));
	free(m_className);
}

void NativeWindowWin32::setFixedSize(int width, int height)
{
	resize(width, height);
	LONG style =  GetWindowLong(m_hwnd, GWL_STYLE);
	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	SetWindowLong(m_hwnd, GWL_STYLE, style);
}

void NativeWindowWin32::resize(int width, int height)
{
	SetWindowPos(m_hwnd, nullptr, -1, -1, (int)width, (int)height, SWP_NOMOVE);
	RECT r;
	GetClientRect(m_hwnd, &r);
	int w = width - r.right;
	int h = height - r.bottom;

	SetWindowPos(m_hwnd, nullptr, -1, -1, width + w, height + h, SWP_NOMOVE);
}

void NativeWindowWin32::move(int x, int y)
{
	SetWindowPos(m_hwnd, nullptr, (int)x, (int)y, -1, -1, SWP_NOSIZE);
}

void NativeWindowWin32::show()
{
	ShowWindow(m_hwnd, SW_SHOW);
}

void NativeWindowWin32::handleBuffer()
{
}

void NativeWindowWin32::hide()
{
	ShowWindow(m_hwnd, SW_HIDE);
}

void NativeWindowWin32::redraw()
{
	RedrawWindow(m_hwnd, nullptr, nullptr, RDW_ERASE|RDW_INVALIDATE);
	UpdateWindow(m_hwnd);
}

void NativeWindowWin32::setCaption(const std::string &caption)
{
	SetWindowText(m_hwnd, caption.c_str());
}

void NativeWindowWin32::grabMouse(bool grab)
{
	if(grab)
	{
		SetCapture(m_hwnd);
	}
	else
	{
		ReleaseCapture();
	}
}

bool NativeWindowWin32::hasEvent()
{
	MSG msg;
	return PeekMessage(&msg, nullptr, 0, 0, 0) != 0;
}

Event *NativeWindowWin32::getNextEvent()
{
	Event *event = nullptr;

	MSG msg;
	if(GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	event = this->event;
	this->event = nullptr;

	return event;
}

} // GUI::
