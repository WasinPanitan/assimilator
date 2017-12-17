#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;
char *bitmap;
char *voxel;
vector<char*> output;

int bm_width,bm_height;
int vx_width,vx_height,vx_depth;

char get_bm(uint32_t x,uint32_t y){
    return bitmap[y*bm_width + x];
}
char get_vx(uint32_t x,uint32_t y,uint32_t z){
    return bitmap[y*vx_depth*vx_width + x*vx_depth + z];
}


int main() {
    cin >> bm_width >> bm_height;
    cin >> vx_width >> vx_height >> vx_depth;
    int bitmapSize = (bm_width * bm_height);
    int voxelSize = (vx_width * vx_height * vx_depth);
    bitmap = (char*)malloc(bitmapSize * sizeof(char));
    voxel = (char*)malloc(voxelSize * sizeof(char));
    // output.push(block);
    return 0;
}