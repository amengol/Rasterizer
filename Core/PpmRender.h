#pragma once

#include <map>

class PpmRender
{
public:
	struct Color
	{
		int r, g, b;
	};
	enum class Format
	{
		P1,
		P2,
		P3
	};

	PpmRender(Format format, int width, int height);
	void putPixel(int x, int y, Color color);
	void writeFile(const char* path, Color background);
private:
	Format m_format;
	int m_width;
	int m_height;
	std::map<std::pair<int, int>, Color> m_colorMap;
};

