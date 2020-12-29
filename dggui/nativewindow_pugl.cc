/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_pugl.cc
 *
 *  Fri Dec 28 18:45:57 CET 2012
 *  Copyright 2012 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "nativewindow_pugl.h"

#include <stdlib.h>
#include <list>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/gl.h>
#endif

#include "window.h"
#include "guievent.h"

#include <hugin.hpp>

namespace GUI
{

NativeWindowPugl::NativeWindowPugl(void* native_window, Window& window)
	: window(window)
{
	INFO(nativewindow, "Running with PuGL native window\n");
	view = puglInit(nullptr, nullptr);
	puglInitContextType(view, PUGL_GL);
	if(native_window)
	{
		puglInitWindowParent(view, (PuglNativeWindow)native_window);
	}
	puglInitWindowClass(view, "DrumgGizmo");
	puglInitWindowSize(view, 750, 466);
	puglInitResizable(view, true);
	puglCreateWindow(view, "DrumGizmo");

	puglSetHandle(view, (PuglHandle)this);
	puglSetEventFunc(view, onEvent);
}

NativeWindowPugl::~NativeWindowPugl()
{
	puglDestroy(view);
}

void NativeWindowPugl::setFixedSize(std::size_t width, std::size_t height)
{
//	redraw();
}

void NativeWindowPugl::resize(std::size_t width, std::size_t height)
{
//	DEBUG(nativewindow_pugl, "Resizing to %dx%d\n", width, height);
//	init();
//	redraw();
}

std::pair<std::size_t, std::size_t> NativeWindowPugl::getSize() const
{
	int width, height;
	puglGetSize(view, &width, &height);
	return {width, height};
}

void NativeWindowPugl::move(int x, int y)
{
//	redraw();
}

void NativeWindowPugl::show()
{
	puglShowWindow(view);
}

void NativeWindowPugl::hide()
{
	puglHideWindow(view);
}

bool NativeWindowPugl::visible() const
{
	return puglGetVisible(view);
}

void NativeWindowPugl::redraw(const Rect& dirty_rect)
{
	//puglPostRedisplay(view);//	handleBuffer();
	onDisplay(view);
}

void NativeWindowPugl::setCaption(const std::string &caption)
{
//	redraw();
}

void NativeWindowPugl::grabMouse(bool grab)
{
	puglGrabFocus(view);
}

EventQueue NativeWindowPugl::getEvents()
{
	puglProcessEvents(view);
	EventQueue events;
	std::swap(events, event_queue);
	return events;
}

void* NativeWindowPugl::getNativeWindowHandle() const
{
	return (void*)puglGetNativeWindow(view);
}

void NativeWindowPugl::onEvent(PuglView* view, const PuglEvent* event)
{
	NativeWindowPugl* native = (NativeWindowPugl*)puglGetHandle(view);

	switch(event->type)
	{
	case PUGL_NOTHING:
		break;
	case PUGL_CONFIGURE:
		onReshape(view, event->configure.width, event->configure.height);
		{
			auto resize_event = std::make_shared<ResizeEvent>();
			resize_event->width = event->configure.width;
			resize_event->height = event->configure.height;
			native->event_queue.push_back(resize_event);
		}
		break;
	case PUGL_EXPOSE:
		onDisplay(view);
		break;
	case PUGL_CLOSE:
		//quit = 1;
		break;
	case PUGL_KEY_PRESS:
		fprintf(stderr, "Key %u (char %u) press (%s)%s\n",
		        event->key.keycode, event->key.character, event->key.utf8,
		        event->key.filter ? " (filtered)" : "");
		if (event->key.character == 'q' ||
		    event->key.character == 'Q' ||
		    event->key.character == PUGL_CHAR_ESCAPE) {
			//quit = 1;
		}
		break;
	case PUGL_KEY_RELEASE:
		fprintf(stderr, "Key %u (char %u) release (%s)%s\n",
		        event->key.keycode, event->key.character, event->key.utf8,
		        event->key.filter ? " (filtered)" : "");
		break;
	case PUGL_MOTION_NOTIFY:
		{
			auto mouseMoveEvent = std::make_shared<MouseMoveEvent>();
			mouseMoveEvent->x = event->motion.x;
			mouseMoveEvent->y = event->motion.y;
			native->event_queue.push_back(mouseMoveEvent);
		}
		break;
	case PUGL_BUTTON_PRESS:
	case PUGL_BUTTON_RELEASE:
		{
			auto buttonEvent = std::make_shared<ButtonEvent>();
			buttonEvent->x = event->button.x;
			buttonEvent->y = event->button.y;
			switch(event->button.button) {
			case 1:
				buttonEvent->button = MouseButton::left;
				break;
			case 2:
				buttonEvent->button = MouseButton::middle;
				break;
			case 3:
				buttonEvent->button = MouseButton::right;
				break;
			default:
				WARN(X11, "Unknown button %d, setting to MouseButton::left\n",
				     event->button.button);
				buttonEvent->button = MouseButton::left;
				break;
			}

			buttonEvent->direction =
				(event->type == PUGL_BUTTON_PRESS) ?
				Direction::down : Direction::up;

			buttonEvent->doubleClick =
				(event->type == PUGL_BUTTON_PRESS) &&
				((event->button.time - native->last_click) < 200);

			if(event->type == PUGL_BUTTON_PRESS)
			{
				native->last_click = event->button.time;
			}
			native->event_queue.push_back(buttonEvent);
		}
		fprintf(stderr, "Mouse %d %s at %f,%f ",
		        event->button.button,
		        (event->type == PUGL_BUTTON_PRESS) ? "down" : "up",
		        event->button.x,
		        event->button.y);
		///printModifiers(view, event->scroll.state);
		break;
	case PUGL_SCROLL:
		{
			auto scrollEvent = std::make_shared<ScrollEvent>();
			scrollEvent->x = event->scroll.x;
			scrollEvent->y = event->scroll.y;
			scrollEvent->delta = event->scroll.dy * -1;//scroll * ((xevent.xbutton.button == 4) ? -1 : 1);
			native->event_queue.push_back(scrollEvent);
		}
		fprintf(stderr, "Scroll %f %f %f %f ",
		        event->scroll.x, event->scroll.y, event->scroll.dx, event->scroll.dy);
		//printModifiers(view, event->scroll.state);
		//dist += event->scroll.dy;
		//if (dist < 10.0f) {
		//	dist = 10.0f;
		//}
		puglPostRedisplay(view);
		break;
	case PUGL_ENTER_NOTIFY:
		fprintf(stderr, "Entered\n");
		break;
	case PUGL_LEAVE_NOTIFY:
		fprintf(stderr, "Exited\n");
		break;
	case PUGL_FOCUS_IN:
		fprintf(stderr, "Focus in\n");
		break;
	case PUGL_FOCUS_OUT:
		fprintf(stderr, "Focus out\n");
		break;
	}
}

void NativeWindowPugl::onReshape(PuglView* view, int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
}

void NativeWindowPugl::onDisplay(PuglView* view)
{
	NativeWindowPugl* native = (NativeWindowPugl*)puglGetHandle(view);
	Window& window = native->window;
	//window.redraw();

	if((window.wpixbuf.width < 16) || (window.wpixbuf.height < 16))
	{
		return;
	}

	puglEnterContext(view);

	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 0.0f);

	GLuint image;

	glGenTextures(1, &image);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D,
	             0, GL_RGBA,
	             window.wpixbuf.width,
	             window.wpixbuf.height,
	             0, GL_RGB, GL_UNSIGNED_BYTE,
	             window.wpixbuf.buf);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0f,  0.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2d(0.0f, -1.0f); glVertex2f(-1.0f,  1.0f);
	glTexCoord2d(1.0f, -1.0f); glVertex2f( 1.0f,  1.0f);
	glTexCoord2d(1.0f,  0.0f); glVertex2f( 1.0f, -1.0f);
	glEnd();

	glDeleteTextures(1, &image);
	glDisable(GL_TEXTURE_2D);
	glFlush();

	puglLeaveContext(view, true);
}

void NativeWindowPugl::onMouse(PuglView* view, int button, bool press, int x, int y)
{
	NativeWindowPugl* native = (NativeWindowPugl*)puglGetHandle(view);

	DEBUG(nativewindow_pugl, "Mouse %d %s at (%d,%d)\n", button,
	      press? "down":"up", x, y);

	ButtonEvent* e = new ButtonEvent();
	e->x = x;
	e->y = y;

	switch(button) {
	case 1:
		e->button = MouseButton::left;
		break;
	case 2:
		e->button = MouseButton::middle;
		break;
	case 3:
	default:
		e->button = MouseButton::right;
		break;
	}

	e->direction = press ? Direction::down : Direction::up;
	e->doubleClick = false;

	native->eventq.push_back(e);
}

void NativeWindowPugl::onKeyboard(PuglView* view, bool press, uint32_t key)
{
	NativeWindowPugl* native = (NativeWindowPugl*)puglGetHandle(view);

	KeyEvent* e = new KeyEvent();
	e->direction = press ? Direction::down : Direction::up;

	switch(key)
	{
	case PUGL_KEY_LEFT: e->keycode = Key::left; break;
	case PUGL_KEY_RIGHT: e->keycode = Key::right; break;
	case PUGL_KEY_UP: e->keycode = Key::up; break;
	case PUGL_KEY_DOWN: e->keycode = Key::down; break;
	case PUGL_KEY_PAGE_UP: e->keycode = Key::pageDown; break;
	case PUGL_KEY_PAGE_DOWN: e->keycode = Key::pageUp; break;
	default: e->keycode = Key::unknown; break;
	}

	// TODO: perform character type check
	if(e->keycode == Key::unknown)
	{
		e->keycode = Key::character;
		e->text.assign(1, (char)key);
	}

	native->eventq.push_back(e);
}

} // GUI::
