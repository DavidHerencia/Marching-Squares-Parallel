#include <deque>
#include <string>
#include <iostream>
#include <algorithm>
#include "renderer.hpp"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "stb_image_write.hpp" // Include a library for image writing, e.g., stb_image_write.h
using namespace std;

void draw_line_helper(unsigned char *img_buffer, int img_w, int img_h, int x1, int y1, int x2, int y2)
{
    // Bresenham's line algorithm implementation
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        if (x1 >= 0 && x1 < img_w && y1 >= 0 && y1 < img_h)
        {
            // Set pixel color to black
            img_buffer[(y1 * img_w + x1) * 3] = 0;     // R
            img_buffer[(y1 * img_w + x1) * 3 + 1] = 0; // G
            img_buffer[(y1 * img_w + x1) * 3 + 2] = 0; // B
        }

        if (x1 == x2 && y1 == y2)
            break;

        int err2 = err * 2;
        if (err2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (err2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_line(unsigned char *img_buffer, int img_w, int img_h, int x1, int y1, int x2, int y2, int line_width)
{
    line_width = max(1, line_width);
    int half_width = line_width / 2;

    // For thick lines, we need to draw multiple parallel lines
    // We'll draw a square brush around each point of the line
    for (int offset_x = -half_width; offset_x <= half_width; ++offset_x)
    {
        for (int offset_y = -half_width; offset_y <= half_width; ++offset_y)
        {
            draw_line_helper(img_buffer, img_w, img_h, 
                           x1 + offset_x, y1 + offset_y, 
                           x2 + offset_x, y2 + offset_y);
        }
    }
}

void render_to_image(const deque<LineSegment> &lines, const string &filename, double min_v, double max_v, int img_w, int img_h)
{
    // Create an image buffer USING a library like stb_image or similar
    unsigned char *img_buffer = new unsigned char[img_w * img_h * 3]; // RGB image

    // Initialize the image buffer to white
    for (int i = 0; i < img_w * img_h * 3; i++)
    {
        img_buffer[i] = 255;
    }

    // Draw the line segments onto the image buffer
    for (auto line : lines)
    {
        // if both endpoints are within the image bounds are outside the image bounds, skip this line
        if ((line.x1 < min_v || line.x1 > max_v || line.y1 < min_v || line.y1 > max_v) &&
            (line.x2 < min_v || line.x2 > max_v || line.y2 < min_v || line.y2 > max_v))
        {
            continue; // Skip this line segment
        }

        if (line.x1 < min_v || line.x1 > max_v || line.y1 < min_v || line.y1 > max_v)
        {
            if (line.x1 < min_v)
                line.x1 = min_v;
            if (line.x1 > max_v)
                line.x1 = max_v;
            if (line.y1 < min_v)
                line.y1 = min_v;
            if (line.y1 > max_v)
                line.y1 = max_v;
        }
        if (line.x2 < min_v || line.x2 > max_v || line.y2 < min_v || line.y2 > max_v)
        {
            if (line.x2 < min_v)
                line.x2 = min_v;
            if (line.x2 > max_v)
                line.x2 = max_v;
            if (line.y2 < min_v)
                line.y2 = min_v;
            if (line.y2 > max_v)
                line.y2 = max_v;
        }

        // Convert the coordinates to pixel indices
        int x1 = static_cast<int>((line.x1 - min_v) / (max_v - min_v) * img_w);
        int y1 = static_cast<int>((line.y1 - min_v) / (max_v - min_v) * img_h);
        int x2 = static_cast<int>((line.x2 - min_v) / (max_v - min_v) * img_w);
        int y2 = static_cast<int>((line.y2 - min_v) / (max_v - min_v) * img_h);

        // Clamp the pixel indices to the image bounds
        x1 = max(0, min(img_w - 1, x1));
        y1 = max(0, min(img_h - 1, y1));
        x2 = max(0, min(img_w - 1, x2));
        y2 = max(0, min(img_h - 1, y2));

        draw_line(img_buffer, img_w, img_h, x1, y1, x2, y2, 2);
    }

    // Save image
    stbi_write_png(filename.c_str(), img_w, img_h, 3, img_buffer, img_w * 3);

    // Clean up memory
    delete[] img_buffer;
}
