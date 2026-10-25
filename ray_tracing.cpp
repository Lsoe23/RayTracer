#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include "Vector3.hpp"
#include "Triangle.hpp"
#include "file_util.h"

using namespace std;

int main()
{

    //parsing through our blender file 
    vector<string> lines = readLines("simple.obj");
    for(const string& line : lines) {
        cout << line << endl;
        vector<string> parts = split(line, ' ');
        cout << parts[0] << endl;
    }

    //Creating our Vector here
    Vector3 temp = Vector3{0, 0, 0};
    temp += Vector3{1, 1, 1};

    //The size of our image
    const int w = 640;
    const int h = 640;

    //Storing our pixel data 
    unsigned char pixels[w * h * 3];

    //Creating our triangles
    //x moves on the x-axis
    //y moves on the y-axis
    //z moves within that 3d spcae
    Triangle triangle = Triangle{Vector3{-1, 1, 0}, Vector3{-1, -1, 0}, Vector3{1, 1, 0}};
    Triangle triangle2 = Triangle{Vector3{1, 1, 0}, Vector3{-1, -1, -0.9}, Vector3{1, -1, 0}};


    //keeping track of both our triangles, but only one is rendering
    vector<Triangle> triangles = {triangle, triangle2};

    Vector3 background_color = Vector3{225, 0, 255};

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int index = y * w * 3 + x * 3;

            unsigned char r = background_color.x;

            unsigned char g = background_color.y;

            unsigned char b = background_color.z;

            //Where our camera is currently positioned
            Vector3 camera_origin = Vector3{1, 1, 1};

            //Our target focus
            Vector3 camera_look_at = Vector3{0, 0, 0};

            //Camera_up? Is that the position on the z-axis or moving where the camera is looking at
            Vector3 camera_up = Vector3{0, 1, 0};

            Vector3 target{
                x / (float)w * 2 - 1, -(y / (float)h * 2 - 1), 0};
                
            Vector3 temp = (target - camera_origin);

            Vector3 ray = temp.normalize();


            //Distance of how far the light is hitting
            float closest_t = 1000000;

            for (const Triangle &triangle : triangles)
            {
                //Calculating the edges of the vector
                Vector3 edge_one = triangle.one - triangle.two;
                Vector3 edge_two = triangle.three - triangle.two;
                Vector3 p = edge_two.cross(edge_one);
                Vector3 abc = p.normalize();

                float d = -abc.dot(triangle.one);

                float t = -(abc.dot(camera_origin) + d) / abc.dot(ray);

                if(t > closest_t)
                 continue;

                Vector3 collision_point = ray * t + camera_origin;

                //Calculating all three triangle within that vector
                Vector3 minus_one = collision_point - triangle.one;
                Vector3 minus_two = collision_point - triangle.two;
                Vector3 minus_three = collision_point - triangle.three;

                float triangle_area = p.length() / 2;

                float area_one = minus_two.cross(minus_three).length() / 2;
                float area_two = minus_three.cross(minus_one).length() / 2;
                float area_three = minus_one.cross(minus_two).length() / 2;


                float u = area_one / triangle_area;
                float v = area_two / triangle_area;
                float a = area_three / triangle_area;

                float sum = u + v + a;

                float diff = abs(1 - sum);

                if (diff > .00001)
                {
                    //It's not in the TRIANGLE!!
                    //This is our 3D space
                    closest_t = t;
                    r = 255 * u + 255 * v + 255 * a;
                    g = 0 * u + 0 * v + 0 * a;
                    b = 0 * u + 0 * v + 0 * a;
                }
                else
                {
                    //Inside the TRIANGLE!!
                    closest_t = t;
                    r = 0 * u + 0 * v + 0 * a;
                    g = 255 * u + 255 * v + 255 * a;
                    b = 255 * u + 255 * v + 255 * a;
                }
            }

            pixels[index] = r;
            pixels[index + 1] = g;
            pixels[index + 2] = b;
        }
    }

    //Outputing our image file
    stbi_write_png("image10.png", w, h, 3, pixels, w * 3);
    return 0;
}