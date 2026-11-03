#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include "Vector3.hpp"
#include "Triangle.hpp"
#include "file_util.h"

using namespace std;

// int main()
// 

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

// Left Wall
Triangle triangle01 = Triangle{Vector3{-1, 1, -2}, Vector3{-1, 1, -1}, Vector3{-1, -1, -2}};
Triangle triangle02 = Triangle{Vector3{-1, 1, -1}, Vector3{-1, -1, -1}, Vector3{-1, -1, -2}};

// Right Wall
Triangle triangle03 = Triangle{Vector3{1, 1, -1}, Vector3{1, 1, -2}, Vector3{1, -1, -2}};
Triangle triangle04 = Triangle{Vector3{1, -1, -1}, Vector3{1, 1, -1}, Vector3{1, -1, -2}};

// Top Wall
Triangle triangle05 = Triangle{Vector3{-1, 1, -1}, Vector3{-1, 1, -2}, Vector3{1, 1, -2}};
Triangle triangle06 = Triangle{Vector3{1, 1, -1}, Vector3{-1, 1, -1}, Vector3{1, 1, -2}};

// Bottom Wall
Triangle triangle07 = Triangle{Vector3{-1, -1, -2}, Vector3{-1, -1, -1}, Vector3{1, -1, -2}};
Triangle triangle08 = Triangle{Vector3{-1, -1, -1}, Vector3{1, -1, -1}, Vector3{1, -1, -2}};

// Back Wall
Triangle triangle09 = Triangle{Vector3{-1, 1, -2}, Vector3{-1, -1, -2}, Vector3{1, -1, -2}};
Triangle triangle10 = Triangle{Vector3{1, 1, -2}, Vector3{-1, 1, -2}, Vector3{1, -1, -2}};

vector<Triangle> triangles = {
    triangle01, triangle02,
    triangle03, triangle04,
    triangle05, triangle06,
    triangle07, triangle08,
    triangle09, triangle10};


//keeping track of both our triangles, but only one is rendering
// vector<Triangle> triangles = {triangle, triangle2};

Vector3 background_color = Vector3{225, 0, 255};

float trace_ray_distance(Vector3 origin, Vector3 direction)
{
    float closest_t = INFINITY;
    
    for (const Triangle &triangle : triangles)
    {
        //Finding where we hit the plane, meaning where the light is hitting the edges of the triangle
        Vector3 edge_one = triangle.one - triangle.two;
        Vector3 edge_two = triangle.three - triangle.two;
        Vector3 p = edge_two.cross(edge_one);
        Vector3 abc = p.normalize();
        float d = -abc.dot(triangle.one);

        float t = -(abc.dot(origin) + d) / abc.dot(direction);

        if (t > closest_t || t <= 0)
         continue;

        Vector3 collision_point = direction * t + origin;

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

        if (diff > .00001 || isnan(sum))
        {
            //We're not in the triangle
        }
        else
        {
            //We're in the triangle
            closest_t = t;
        }
    }

        return closest_t;
    }

Vector3 trace_ray(Vector3 origin, Vector3 direction, int bounces = 5)
{
    Vector3 color = Vector3{background_color.x, background_color.y, background_color.z};
    if(bounces <= 0)
    return background_color;

    float closest_t = 1000000;

    for (const Triangle &triangle : triangles)
    {
        //Setting up another dimension where we're hitting the plane
        Vector3 edge_one = triangle.one - triangle.two;
        Vector3 edge_two = triangle.three - triangle.two;
        Vector3 p = edge_two.cross(edge_one);
        Vector3 abc = p.normalize();
        float d = -abc.dot(triangle.one);

        float t = -(abc.dot(origin) + d) / abc.dot(direction);

        if (t > closest_t || t <= 0)
         continue;

        Vector3 collision_point = direction * t + origin;

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

        if (diff > .00001 || isnan(sum))
        {
            //We're not in the triangle: Remember that!!
        }
        else
        {
            closest_t = t;

            Vector3 base_color = Vector3{255, 255, 255};
            Vector3 light_direction = Vector3{.5, 1, .5}.normalize();

            Vector3 ambient = base_color * .1;
            Vector3 normal = p.normalize();

            float strength = max(0.f, light_direction.dot(normal));

            Vector3 diffuse = base_color * strength;

            if(trace_ray_distance(collision_point + light_direction*.00001, light_direction) != INFINITY){
                diffuse = Vector3{0,0,0};
            }

            Vector3 reflective = Vector3{0, 0, 0};

            if(true){
                //bounces, this can also be improved to create light reflection from fluid
                Vector3 reflective_direction = direction - normal * 2 * normal.dot(direction);
                reflective = trace_ray(collision_point, reflective_direction, bounces - 1) * .5;
                diffuse *=20;
                ambient *=.5;
            }

            Vector3 _color = ambient + diffuse + reflective;
            color = _color;
        }


    }
    return color;
}

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
    
    Vector3 camera_origin = Vector3{0, 0, 1};
    Vector3 camera_look_at = Vector3{0, 0, 0};
    Vector3 camera_up = Vector3{0, 1, 0};

    ///Looping over the column 
    for (int y = 0; y < h; y++)
    {

        ///2d image processing time here, thinking about how to recreate the image on a 2 dimension side going right
        for (int x = 0; x < h; x++)
        {
            int index = y * w * 3 + x * 3;

            ///RGB
            unsigned char r = background_color.x;
            unsigned char g = background_color.y;
            unsigned char b = background_color.z;

            Vector3 target{
                x / (float)w * 2 - 1, -(y / (float)h * 2 - 1), 0};
            
            Vector3 temp = (target - camera_origin);
            Vector3 ray = temp.normalize();

            Vector3 color = trace_ray(camera_origin, ray);

            r = color.x;
            g = color.y;
            b = color.z;

            pixels[index] = r;
            pixels[index + 1] = g;
            pixels[index + 2] = b;
            }
        }

        stbi_write_png("Diffuse.png", w, h, 3, pixels, w * 3);
        return 0;
    }






//     for (int y = 0; y < h; y++)
//     {
//         for (int x = 0; x < w; x++)
//         {
//             int index = y * w * 3 + x * 3;

//             unsigned char r = background_color.x;

//             unsigned char g = background_color.y;

//             unsigned char b = background_color.z;

           
//             //Where our camera is currently positioned
//             Vector3 camera_origin = Vector3{1, 1, 1};

//             //Our target focus
//             Vector3 camera_look_at = Vector3{0, 0, 0};

//             //Camera_up? Is that the position on the z-axis or moving where the camera is looking at
//             //Moving the camera around the 3d space
//             Vector3 camera_up = Vector3{0, 1, 0};

//             Vector3 target{
//                 x / (float)w * 2 - 1, -(y / (float)h * 2 - 1), 0};
                
//             Vector3 temp = (target - camera_origin);

//             Vector3 ray = temp.normalize();


//             //Distance of how far the light is hitting
//             float closest_t = 1000000;

//             for (const Triangle &triangle : triangles)
//             {
//                 //Calculating the edges of the vector
//                 Vector3 edge_one = triangle.one - triangle.two;
//                 Vector3 edge_two = triangle.three - triangle.two;
//                 Vector3 p = edge_two.cross(edge_one);
//                 Vector3 abc = p.normalize();

//                 float d = -abc.dot(triangle.one);

//                 float t = -(abc.dot(camera_origin) + d) / abc.dot(ray);
                
//                 //If i hit anything in the negative, ignore it
//                 if(t > closest_t || t <= 0)
//                  continue;

//                 Vector3 collision_point = ray * t + camera_origin;

//                 //Calculating all three triangle within that vector
//                 Vector3 minus_one = collision_point - triangle.one;
//                 Vector3 minus_two = collision_point - triangle.two;
//                 Vector3 minus_three = collision_point - triangle.three;

//                 float triangle_area = p.length() / 2;

//                 float area_one = minus_two.cross(minus_three).length() / 2;
//                 float area_two = minus_three.cross(minus_one).length() / 2;
//                 float area_three = minus_one.cross(minus_two).length() / 2;


//                 float u = area_one / triangle_area;
//                 float v = area_two / triangle_area;
//                 float a = area_three / triangle_area;

//                 float sum = u + v + a;

//                 float diff = abs(1 - sum);

//                 if (diff > .00001 || isnan(sum))
//                 {
//                     //It's not in the TRIANGLE!!
//                     //This is our 3D space
//                     // closest_t = t;
//                     // r = 255 * u + 255 * v + 255 * a;
//                     // g = 0 * u + 0 * v + 0 * a;
//                     // b = 0 * u + 0 * v + 0 * a;
//                 }
//                 else
//                 {
//                     //Inside the TRIANGLE!!
//                     closest_t = t;

//                     Vector3 base_color = Vector3{255, 255, 255};

//                     Vector3 light_direction = Vector3{.5, 1, .1}.normalize();

//                     Vector3 ambient = base_color * .1;
//                     Vector3 normal = p.normalize();

//                     float strength = max(0.f, light_direction.dot(normal));


//                     // float strength = abs(light_direction.dot(normal));
//                     //Not doing double sided triangle



                    
//                     Vector3 diffuse = {0,0,0};
//                     Vector3 reflective = {0,0,0};

//                     Vector3 color = ambient + diffuse + reflective;

//                     r = color.x;
//                     g = color.y;
//                     b = color.z;

//                     // r = 0 * u + 0 * v + 0 * a;
//                     // g = 255 * u + 255 * v + 255 * a;
//                     // b = 255 * u + 255 * v + 255 * a;
//                 }
//             }

//             pixels[index] = r;
//             pixels[index + 1] = g;
//             pixels[index + 2] = b;
//         }
//     }

//     //Outputing our image file
//     stbi_write_png("image10.png", w, h, 3, pixels, w * 3);
//     return 0;
// }