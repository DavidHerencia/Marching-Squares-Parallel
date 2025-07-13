#pragma once

#include <deque>
#include <string>
#include <iostream>
#include "marching_squares.hpp"
using namespace std;

// Draw the line segment in the image buffer (simple Bresenham's line algorithm)
void draw_line(unsigned char *img_buffer, int img_w, int img_h, int x1, int y1, int x2, int y2);
void render_to_image(const deque<LineSegment> &lines, const string &filename, double min_v, double max_v, int img_w, int img_h);