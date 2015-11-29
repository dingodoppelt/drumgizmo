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

namespace GUI {

void NativeWindowPugl::onDisplay(PuglView* view)
{
	NativeWindowPugl* native = (NativeWindowPugl*)puglGetHandle(view);
	Window* windowptr = native->window;

	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint image;

	glGenTextures(1, &image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowptr->wpixbuf.width,
	             windowptr->wpixbuf.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
	             windowptr->wpixbuf.buf);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex2f(0.0, 0.0);
	glTexCoord2d(0.0, 1.0); glVertex2f(0.0, windowptr->wpixbuf.height);
	glTexCoord2d(1.0, 1.0); glVertex2f(windowptr->wpixbuf.width, windowptr->wpixbuf.height);
	glTexCoord2d(1.0, 0.0); glVertex2f(windowptr->wpixbuf.width, 0.0);
	glEnd();

	glDeleteTextures(1, &image);
	glDisable(GL_TEXTURE_2D);
	glFlush();

	puglPostRedisplay(view);
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

	printf("%d\n", key);

	switch(key) {
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

	printf("\t text: %s\n", e->text.c_str());

	native->eventq.push_back(e);
}

NativeWindowPugl::NativeWindowPugl(Window *window)
	: window(window)
{
	INFO(nativewindow, "Running with PuGL native window\n");
	init();
}

NativeWindowPugl::~NativeWindowPugl()
{
	puglDestroy(view);
}

void NativeWindowPugl::init() {
	PuglView* oldView = view;
	if(view)
	{
		oldView = view;
	}

//	view = puglCreate(0, "DrumgGizmo", window->x(), window->y(), false, true);
	view = puglCreate(0, "DrumgGizmo", 370, 330, false, true);
	puglSetHandle(view, (PuglHandle)this);
	puglSetDisplayFunc(view, onDisplay);
	puglSetMouseFunc(view, onMouse);
	puglSetKeyboardFunc(view, onKeyboard);

	if(oldView)
	{
		free(oldView);
	}
}

void NativeWindowPugl::setFixedSize(int width, int height)
{
//	redraw();
}

void NativeWindowPugl::resize(int width, int height)
{
//	DEBUG(nativewindow_pugl, "Resizing to %dx%d\n", width, height);
//	init();
//	redraw();
}

void NativeWindowPugl::move(int x, int y)
{
//	redraw();
}

void NativeWindowPugl::show()
{
//	redraw();
}

void NativeWindowPugl::hide()
{
//	redraw();
}

void NativeWindowPugl::handleBuffer()
{
	onDisplay(view);
}

void NativeWindowPugl::redraw()
{
//	handleBuffer();
}

void NativeWindowPugl::setCaption(const std::string &caption)
{
//	redraw();
}

void NativeWindowPugl::grabMouse(bool grab)
{
//	redraw();
}

bool NativeWindowPugl::hasEvent()
{
	// dirty hack - assume that this function is called enough times to get fluent gui
	// ^^ Bad assumption
	puglProcessEvents(view);
	return !eventq.empty();
}

Event *NativeWindowPugl::getNextEvent()
{
	Event *event = nullptr;

	if(!eventq.empty()) {
		event = eventq.front();
		eventq.pop_front();
	}
	return event;
}

Event *NativeWindowPugl::peekNextEvent()
{
	Event *event = nullptr;

	if(!eventq.empty()) {
		event = eventq.front();
	}
	return event;
}

} // GUI::
