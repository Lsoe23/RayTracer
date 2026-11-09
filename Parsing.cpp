#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include "Vector3.hpp"
#include "Triangle.hpp"
#include "file_util.h"

using namespace std;


int main()

{

    //parsing through our blender file 
    vector<string> lines = readLines("simple.obj");

    //Vertices - x, y, z
    //v -0.5 -0.5 0.0
    vector<float> x_coordinates;
    vector<float> y_coordinates;
    vector<float> z_coordinates;

    //VT - U V
    //VT 0 0
    vector<float> u_coordinates;
    vector<float> v_coordinates;


    for(const string& line : lines) {
        //cout << line << endl;
        vector<string> parts = split(line, ' ');

            if (parts.empty())
                continue;

            //If we do find a vertices identifier
            if (parts[0] == "v")
            {
                //cout << parts[0] << endl;

                //After every iteration through the loops, we're storing each vertices in here
                vector<string> coordinates;
            
            //We're removing our identifer so we'll only interact with our vertices
            for (const string& part : parts)
            {
                if (!part.empty() && part != "v")
                {
                    {
                        coordinates.push_back(part);

                        //cout << "Coordinates found: " << coordinates.size() << endl;

                        if (coordinates.size() == 3)
                        {
                            //converting this to a floating point number and pushing it back to x, y, z
                            x_coordinates.push_back(stof(coordinates[0]));
                            y_coordinates.push_back(stof(coordinates[1]));
                            z_coordinates.push_back(stof(coordinates[2]));

                        }

                    }
                }
            }
        }
        if (parts[0] == "vt")
                {

                    vector<string> texture_coordinates;

                    for (const string& part : parts)
                    {
                        if (!part.empty() && part != "vt")
                        {
                            texture_coordinates.push_back(part);
                        }
                        //vt = 0 0
                        //our U and V coordinate
                        if (texture_coordinates.size() == 2)
                        {
                            u_coordinates.push_back(stof(texture_coordinates[0]));
                            v_coordinates.push_back(stof(texture_coordinates[1]));

                        }
                    }
                }
    }
    //Printing out the x, y, z vertices
    cout << "This is the Vertex Section" << endl;
    for (int i = 0; i < x_coordinates.size(); i++)
    {
        cout << "Vertex " << i + 1 << ": "
        << x_coordinates[i] << ", "
        << y_coordinates[i] << ", "
        << z_coordinates[i] << endl;

    }

    //Printing our the u and v coodinates
    cout << " " << endl;
    // cout << u_coordinates.size() << endl;
    // cout << v_coordinates.size() << endl;
    cout << "This is the Vertex Texture" << endl;
    for (int i = 0; i < u_coordinates.size(); i++)
    {
        cout << "U & V Coordinates " << i + 1 << ": "
            << u_coordinates[i] << ", "
            << v_coordinates[i] << endl;
    }

    return 0;
}