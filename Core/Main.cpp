#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "PpmRender.h"
#include "MathUtil.h"

void drawLine(vec2<int> p0, vec2<int> p1, const PpmRender::Color& color, PpmRender& render)
{
	bool steep = false;
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}
	if (p0.x > p1.x) {
		std::swap(p0.x, p1.x);
		std::swap(p0.y, p1.y);
	}
	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;
	int derror = std::abs(dy) * 2;
	int error = 0;
	int y = p0.y;
	for (int x = p0.x; x < p1.x; ++x) {
		if (steep) {
			render.putPixel(y, x, color);
		}
		else {
			render.putPixel(x, y, color);
		}
		error += derror;
		if (error > dx) {
			y += (p1.y > p0.y ? 1 : -1);
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

 void triangle(vec2<int> t0, vec2<int> t1, vec2<int> t2, const PpmRender::Color& color, PpmRender& render)
 {
	 drawLine(t0, t1, color, render);
	 drawLine(t1, t2, color, render);
	 drawLine(t2, t0, color, render);
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
		int x0 = (model.vertices[face.v[0]].x + 1) * width / 2.0f;
		int y0 = (model.vertices[face.v[0]].y + 1) * height / 2.0f;
		const vec2<int> p0{x0, y0};
		int x1 = (model.vertices[face.v[1]].x + 1) * width / 2.0f;
		int y1 = (model.vertices[face.v[1]].y + 1) * height / 2.0f;
		const vec2<int> p1{ x1, y1 };
		int x2 = (model.vertices[face.v[2]].x + 1) * width / 2.0f;
		int y2 = (model.vertices[face.v[2]].y + 1) * height / 2.0f;
		const vec2<int> p2{ x2, y2 };

		triangle(p0, p1, p2, white, render);
	}
 
	PpmRender::Color bgn{ 46, 82, 99 };
	render.writeFile("out.ppm", bgn);



	return 0;
}