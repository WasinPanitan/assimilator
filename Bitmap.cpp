#include <fstream>
#include <iostream>

using namespace std;

bool** ReadBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");

    if(f == NULL)
        throw "Argument Exception";

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    int depth = *(int*)&info[28];

    cout << "Image: " << filename << endl;
    cout << "Width: " << width << endl;
    cout << "Height: " << height << endl;
    cout << "Depth: " << depth << endl;

    int row_padded = (width*3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];
    unsigned char tmp;

    bool** bitmap = new bool*[height];
    for(int i = 0;i < height; ++i){
        bitmap[i] = new bool[width];
    }

    for(int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for(int j = 0; j < width*3; j += 3)
        {
            // Convert (B, G, R) to (R, G, B)
            tmp = data[j];
            data[j] = data[j+2];
            data[j+2] = tmp;

            if(((int)data[j]+(int)data[j+1]+(int)data[j+2])/3 < 128){
                bitmap[i][j/3] = true;
            }
            else{
                bitmap[i][j/3] = false;
            }
            //cout << "Height: " << i << " Width: " << j/3 << " R: "<< (int)data[j] << " G: " << (int)data[j+1]<< " B: " << (int)data[j+2]<< endl;
        }
    }

    fclose(f);
    cout << "\n\n\n" << endl;
    //int a;
    //cin >> a;
    for(int i =0; i < height; i++){
        for(int j = 0;j< width;j++){
            cout << bitmap[i][j] << " ";
        }
        cout << endl;
    }
    return bitmap;
    //return data;
}

/*int main(){

    //ReadBMP("bunny.bmp");
    //return 0;
}*/

