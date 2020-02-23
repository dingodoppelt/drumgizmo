/* -*- Mode: c++ -*- */
/***************************************************************************
 *            benchmarktest.cc
 *
 *  Sun Feb 23 14:24:12 CET 2020
 *  Copyright 2020 Bent Bisballe Nyeng
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
#include <chrono>
#include <iostream>
#include <cstdint>
#include <string>

#include <image.h>
#include <painter.h>
#include <canvas.h>

class TimedCanvas
	: public GUI::Canvas
{
public:
	GUI::PixelBufferAlpha& GetPixelBuffer() override
	{
		return pixbuf;
	}

private:
	GUI::PixelBufferAlpha pixbuf{800, 600};
};

class TimedScope
{
public:
	using clock_t = std::chrono::steady_clock;

	TimedScope(const std::string& title, std::size_t iterations)
		: title(title)
		, iterations(iterations)
		, begin(clock_t::now())
	{}

	~TimedScope()
	{
		clock_t::time_point end= std::chrono::steady_clock::now();

		auto micro_seconds =
			std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		micro_seconds /= iterations;
		std::cout << title << ": " << micro_seconds << " µs " <<
			"(" << (micro_seconds / 1000) << " ms)" << std::endl;
	}

private:
	std::string title;
	std::size_t iterations;
	clock_t::time_point begin;
};

int main()
{
	TimedCanvas canvas;
	GUI::Painter painter(canvas);
	GUI::Image image(":resources/bg.png");

	{
		TimedScope timed("No scale", 10000);
		for(int i = 0; i < 10000; ++i)
		{
			painter.drawImage(0, 0, image);
		}
	}

	{
		TimedScope timed("Scaled 1:1", 2000);
		for(int i = 0; i < 2000; ++i)
		{
			painter.drawImageStretched(0, 0, image, 370, 330);
		}
	}

	{
		TimedScope timed("Scaled to window", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image, 800, 6000);
		}
	}

	return 0;
}
