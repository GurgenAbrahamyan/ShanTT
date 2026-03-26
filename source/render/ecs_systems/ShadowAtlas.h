#pragma once
#pragma once
#include <vector>
#include <array>
#include "../../math_custom/Mat4.h"
#include "../../math_custom/Vector2.h"

struct ShadowTile {
    Vector2 uvMin;
    Vector2 uvMax;
    Mat4 lightMatrix;
    bool occupied = false;
    int size = 0;      // pixel size of this tile
    int x = 0, y = 0; // pixel coords in atlas
};

class ShadowAtlas {
public:
    static constexpr int ATLAS_SIZE = 8192;


    struct Band {
        int tileSize;
        int yStart;
        int cols;
        int rows;
    };

    static constexpr std::array<Band, 4> BANDS = {{
        { 1024, 0,    8,  4 },
        { 512,  4096, 16, 4 },
        { 256,  6144, 32, 4 },
        { 128,  7168, 64, 8 },
    }};

    ShadowAtlas() { buildTiles(); }

    int allocate(int tileSize) {
        for (int i = 0; i < (int)tiles.size(); i++) {
            if (!tiles[i].occupied && tiles[i].size == tileSize) {
                tiles[i].occupied = true;
                return i;
            }
        }
        return -1; // full
    }

   
    int allocateConsecutive(int tileSize, int count) {
        int run = 0, start = -1;
        for (int i = 0; i < (int)tiles.size(); i++) {
            if (!tiles[i].occupied && tiles[i].size == tileSize) {
                if (run == 0) start = i;
                run++;
                if (run == count) {
                    for (int j = start; j < start + count; j++)
                        tiles[j].occupied = true;
                    return start;
                }
            } else {
                run = 0;
                start = -1;
            }
        }
        return -1;
    }

    void free(int index) {
        if (index >= 0 && index < (int)tiles.size())
            tiles[index].occupied = false;
    }

    void freeConsecutive(int startIndex, int count) {
        for (int i = startIndex; i < startIndex + count; i++)
            free(i);
    }

    void freeAll() {
        for (auto& t : tiles) t.occupied = false;
    }

    ShadowTile& getTile(int index) { return tiles[index]; }
    const ShadowTile& getTile(int index) const { return tiles[index]; }
    int tileCount() const { return (int)tiles.size(); }



    int allocateBestFitConsecutive(int count) {
        static const int sizes[] = { 1024, 512, 256, 128 };

        for (int s : sizes) {
            int idx = allocateConsecutive(s, count);
            if (idx != -1)
                return idx;
        }
        return -1;
    }

    int allocateBestFit() {
        static const int sizes[] = { 1024, 512, 256, 128 };

        for (int s : sizes) {
            int idx = allocate(s);
            if (idx != -1)
                return idx;
        }
        return -1;
    }


private:
    std::vector<ShadowTile> tiles;

    void buildTiles() {
        for (auto& band : BANDS) {
            for (int row = 0; row < band.rows; row++) {
                for (int col = 0; col < band.cols; col++) {
                    ShadowTile t;
                    t.size = band.tileSize;
                    t.x    = col * band.tileSize;
                    t.y    = band.yStart + row * band.tileSize;

                    t.uvMin = {
                        (float)t.x / ATLAS_SIZE,
                        (float)t.y / ATLAS_SIZE
                    };
                    t.uvMax = {
                        (float)(t.x + t.size) / ATLAS_SIZE,
                        (float)(t.y + t.size) / ATLAS_SIZE
                    };

                    t.occupied = false;
                    tiles.push_back(t);
                }
            }
        }
    }

    
};