#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "PpmRender.h"

void drawLine(int x0, int y0, int x1, int y1, const PpmRender::Color& color, PpmRender& render)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror = std::abs(dy) * 2;
	int error = 0;
	int y = y0;
	for (int x = x0; x < x1; ++x) {
		if (steep) {
			render.putPixel(y, x, color);
		}
		else {
			render.putPixel(x, y, color);
		}
		error += derror;
		if (error > dx) {
			y += (y1 > y0 ? 1 : -1);
			error -= dx * 2;
		}
	}
}

struct Model
{
	struct Vertex
	{
		float x, y, z;
	};
	std::vector<Vertex> vertices;
	
	struct Face
	{
		int v[3];
	};
	std::vector<Face> faces;
};

void readObj(const std::string& path, Model& model)
{
	model.vertices.clear();
	model.faces.clear();

	std::ifstream ss(path);
	if (ss.fail())
		return;
	std::string line;
	while (!ss.eof()) {
		std::getline(ss, line);
		std::stringstream ss(line);
		char c_discard;
		if (line.compare(0, 2, "v ") == 0) {
			ss >> c_discard;
			Model::Vertex vertice;
			ss >> vertice.x >> vertice.y >> vertice.z;
			model.vertices.emplace_back(vertice);
		} else if (line.compare(0, 2, "f ") == 0) {
			ss >> c_discard;
			int i_discard;
			Model::Face face;
			for (int& i : face.v) {
				ss >> i >> c_discard >> i_discard >> c_discard >> i_discard;
				i--; // Obj starts at 1, not 0;
			}
			model.faces.emplace_back(face);
		}
	}
}

int main()
{
	// Window sizes
	const int width = 512;
	const int height = 512;

	Model model;
	readObj("assets\\bunLowRes.obj", model);
	
	PpmRender::Color white{ 255, 255, 255 };
	PpmRender render(PpmRender::Format::P3, width, height);

	for (Model::Face& face : model.faces) {
		// We have 3 vertices for each face
		// Line from V0 to V1
		int x0 = (model.vertices[face.v[0]].x + 1) * width / 2.0f;
		int y0 = (model.vertices[face.v[0]].y + 1) * height / 2.0f;
		int x1 = (model.vertices[face.v[1]].x + 1) * width / 2.0f;
		int y1 = (model.vertices[face.v[1]].y + 1) * height / 2.0f;
		drawLine(x0, y0, x1, y1, white, render);
		// Line from V1 to V2
		int x2 = (model.vertices[face.v[2]].x + 1) * width / 2.0f;
		int y2 = (model.vertices[face.v[2]].y + 1) * height / 2.0f;
		drawLine(x1, y1,x2, y2, white, render);
		// Line from V2 to V0
		drawLine(x0, y0, x2, y2, white, render);
	}

	PpmRender::Color bgn{ 46, 82, 99 };
	render.writeFile("out.ppm", bgn);

	return 0;
}