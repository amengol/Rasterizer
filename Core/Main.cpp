#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "PpmRender.h"
#include "MathUtil.h"
#include <algorithm>
#include <vector>

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

 void drawTriangle(vec2<int> p0, vec2<int> p1, vec2<int> p2, const PpmRender::Color& color, PpmRender& render)
 {
	 drawLine(p0, p1, color, render);
	 drawLine(p1, p2, color, render);
	 drawLine(p2, p0, color, render);
 }

 int sign(vec2<int> p0, vec2<int> p1, vec2<int> p2)
 {
	 return (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
 }

 bool isPointInsideTriangle(vec2<int> point, vec2<int> p0, vec2<int> p1, vec2<int> p2)
 {
	 int d1, d2, d3;
	 bool has_neg, has_pos;

	 d1 = sign(point, p0, p1);
	 d2 = sign(point, p1, p2);
	 d3 = sign(point, p2, p0);

	 has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	 has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	 return !(has_neg && has_pos);
 }

 void fillTriangle(vec2<int> p0, vec2<int> p1, vec2<int> p2, const PpmRender::Color& color, PpmRender& render)
 {
	 drawTriangle(p0, p1, p2, color, render);

	 // Find minimum x
	 int min_x = p0.x;
	 if (min_x > p1.x)
		 min_x = p1.x;
	 if (min_x > p2.x)
		 min_x = p2.x;
	 // Find minimum y
	 int min_y = p0.y;
	 if (min_y > p1.y)
		 min_y = p1.y;
	 if (min_y > p2.y)
		 min_y = p2.y;
	 // Find maximum x
	 int max_x = p0.x;
	 if (max_x < p1.x)
		 max_x = p1.x;
	 if (max_x < p2.x)
		 max_x = p2.x;
	 // Find maximum y
	 int max_y = p0.y;
	 if (max_y < p1.y)
		 max_y = p1.y;
	 if (max_y < p2.y)
		 max_y = p2.y;

	 for (int x = min_x; x <= max_x; ++x) {
		 for (int y = min_y; y <= max_y; ++y) {
			 if (isPointInsideTriangle({ x, y }, p0, p1, p2))
				 render.putPixel(x, y, color);
		 }
	 }
 }

int main()
{
	// Window sizes
	constexpr int width = 256;
	constexpr int height = 256;

// 	Model model;
// 	readObj("assets\\bunLowRes.obj", model);
	
//	PpmRender::Color white{ 255, 255, 255 };
	PpmRender render(PpmRender::Format::P3, width, height);
 
// 	for (Model::Face& face : model.faces) {
// 		// We have 3 vertices for each face
// 		int x0 = (model.vertices[face.v[0]].x + 1) * width / 2.0f;
// 		int y0 = (model.vertices[face.v[0]].y + 1) * height / 2.0f;
// 		const vec2<int> p0{x0, y0};
// 		int x1 = (model.vertices[face.v[1]].x + 1) * width / 2.0f;
// 		int y1 = (model.vertices[face.v[1]].y + 1) * height / 2.0f;
// 		const vec2<int> p1{ x1, y1 };
// 		int x2 = (model.vertices[face.v[2]].x + 1) * width / 2.0f;
// 		int y2 = (model.vertices[face.v[2]].y + 1) * height / 2.0f;
// 		const vec2<int> p2{ x2, y2 };
// 
// 		drawTriangle(p0, p1, p2, white, render);
// 	}

	vec2<int> t0[3] = { 10, 70, 50, 160, 70, 80 };
	vec2<int> t1[3] = { 180, 50, 150, 1, 70, 180 };
	vec2<int> t2[3] = { 180, 150, 120, 160, 130, 180 };

	PpmRender::Color red{ 255, 0, 0 };
	PpmRender::Color white{ 255, 255, 255 };
	PpmRender::Color green{ 0, 255, 0 };

	fillTriangle(t0[0], t0[1], t0[2], red, render);
	fillTriangle(t1[0], t1[1], t1[2], white, render);
	fillTriangle(t2[0], t2[1], t2[2], green, render);
 
	PpmRender::Color bgn{ 46, 82, 99 };
	render.writeFile("out.ppm", bgn);



	return 0;
}