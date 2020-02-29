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
	GUI::Image image_no_alpha(":benchmarktest_resources/image_no_alpha.png");
	GUI::Image image_full_alpha(":benchmarktest_resources/image_full_alpha.png");
	GUI::Image image_edge_alpha(":benchmarktest_resources/image_edge_alpha.png");
	GUI::Image image_inner_alpha(":benchmarktest_resources/image_inner_alpha.png");

	{
		TimedScope timed("No scale, no alpha", 100000);
		for(int i = 0; i < 100000; ++i)
		{
			painter.drawImage(0, 0, image_no_alpha);
		}
	}

	{
		TimedScope timed("No scale, full alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImage(0, 0, image_full_alpha);
		}
	}

	{
		TimedScope timed("No scale, edge alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImage(0, 0, image_edge_alpha);
		}
	}

	{
		TimedScope timed("No scale, inner alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImage(0, 0, image_inner_alpha);
		}
	}

	{
		GUI::PixelBuffer wpixbuf(800, 600);
		std::vector<GUI::PixelBufferAlpha*> children;
		for(int i = 0; i < 100; ++i)
		{
			auto child = new GUI::PixelBufferAlpha(300, 300);
			child->x = i * 2;
			child->y = i * 2;
			children.push_back(child);
		}

		TimedScope timed("Buffer flattening", 100);
		for(int i = 0; i < 100; ++i)
		{
			for(auto child : children)
			{
				child->dirty = true;
			}

			wpixbuf.updateBuffer(children);
		}

		for(auto child : children)
		{
			delete child;
		}
	}

	{
		TimedScope timed("Scaled 1:1 no alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_no_alpha, 370, 330);
		}
	}

	{
		TimedScope timed("Scaled 1:1 full alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_full_alpha, 370, 330);
		}
	}

	{
		TimedScope timed("Scaled 1:1 edge alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_edge_alpha, 370, 330);
		}
	}

	{
		TimedScope timed("Scaled 1:1 inner alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_inner_alpha, 370, 330);
		}
	}

	{
		TimedScope timed("Scaled to window, no alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_no_alpha, 800, 600);
		}
	}

	{
		TimedScope timed("Scaled to window, full alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_full_alpha, 800, 600);
		}
	}

	{
		TimedScope timed("Scaled to window, edge alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_edge_alpha, 800, 600);
		}
	}

	{
		TimedScope timed("Scaled to window, inner alpha", 1000);
		for(int i = 0; i < 1000; ++i)
		{
			painter.drawImageStretched(0, 0, image_inner_alpha, 800, 600);
		}
	}

	return 0;
}
