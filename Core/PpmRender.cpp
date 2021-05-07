#include "PpmRender.h"
#include <fstream>
#include <sstream>

PpmRender::PpmRender(Format format, int width, int height)
	: m_format(format), m_width(width), m_height(height)
{
}

void PpmRender::putPixel(int x, int y, Color color)
{
	std::pair<int, int> xyPair(x, y);
	m_colorMap[xyPair] = color;
}

void PpmRender::writeFile(const char* path, Color background)
{
	std::stringstream ss;

	auto findPair = [](int index, int width, int height) {
		int line = index % width;
		int column = height - (index / width);
		return std::pair<int, int>(line, column);
	};

	switch (m_format)
	{
	case PpmRender::Format::P1:
		break;
	case PpmRender::Format::P2:
		break;
	case PpmRender::Format::P3:
		ss << "P3\n" << m_width << ' ' << m_height << '\n' << "255\n";
		for (int i = 0; i < m_width * m_height; ++i) {
			auto colorIt = m_colorMap.find(std::move(findPair(i, m_width, m_height)));
			if (colorIt != m_colorMap.end()) {
				ss << colorIt->second.r << ' ' << colorIt->second.g << ' ' << colorIt->second.b << '\n';
			}
			else {
				ss << background.r << ' ' << background.g << ' ' << background.b << '\n';
			}
		}
		break;
	default:
		break;
	}

	std::ofstream file(path);
	if (ss.fail())
		return;
	file.write(ss.str().c_str(), ss.str().size());
	file.close();
}
