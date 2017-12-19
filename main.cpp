#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

typedef struct voxel{
    char *data;
    uin32_t width,height,depth;
    char get(uint32_t x,uint32_t y,uint32_t z) {
        return data[y * width * depth + x * depth + z];
    }
    char set(uint32_t x,uint32_t y,uint32_t z,char &v){
        data[y * width * depth + x * depth + z] = v;
    }
} voxel;

typdef struct field{
    char *data;
    uint32_t width,height;
}

char *bitmap;
char *voxel;
vector<voxel> voxels;
vector<char*> output;


char* genvoxel(uint32_t width,uint32_t height,uint32_t depth) {
    voxel b;
    b.data = (char*)malloc(width * height * depth * sizeof(char));
    voxels.push_back(voxel);
    return b;
}


int main() {
    voxel vx;
    bitmap = (char*)malloc(bitmapSize * sizeof(char));
    vx.data = (char*)malloc(vxSize * sizeof(char));
    free(bitmap);
    free(vx.data);
    for(auto &i:voxels) {
        free(i.data);
    }
    return 0;
}