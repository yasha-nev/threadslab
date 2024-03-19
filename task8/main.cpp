#include "terminalcanvas.h"
#include <random>
#include <time.h>
#include <omp.h>

#define height 59

#define width 105
//#define width 210

using namespace std::literals::chrono_literals;

void mapInit(std::vector<std::vector<int>> &map);

void mapZeros(std::vector<std::vector<int>> &map);

void checkAlives(std::vector<std::vector<int>> &map, std::vector<std::vector<int>> &nmap);

int main(){
    std::vector<std::vector<int>> map(height, std::vector<int>(width, 1));
    std::vector<std::vector<int>> newMap(height, std::vector<int>(width, 1));

    mapInit(map);

    //TerminalCanvas canvas;
    //canvas.plot(map);
    //canvas.startDrawing();
    for (;;){
        //mapZeros(newMap);
        checkAlives(map, newMap);
        std::swap(map, newMap);
        //canvas.plot(map);
        std::this_thread::sleep_for(200ms);
    }

    return 0;
}

void mapInit(std::vector<std::vector<int>> &map){
    srand(static_cast<unsigned int>(time(nullptr)));

    for (size_t i = 0; i < height; i++){
        for (size_t j = 0; j < width; j++){

            int p = rand() % 10;

            if (p % 2){
                map[i][j] = COLORS::black;
            }
            else{
                map[i][j] = COLORS::red;
            }
        }
    }
}

void mapZeros(std::vector<std::vector<int>> &map){
    for (size_t i = 0; i < height; i++){
        for (size_t j = 0; j < width; j++){
            map[i][j] = 0;
        }
    }
}

void checkAlives(std::vector<std::vector<int>> &map, std::vector<std::vector<int>> &nmap){
    #pragma omp parallel
    {
        for (size_t i = 0; i < height; i++){
            for (size_t j = 0; j < width; j++){
                int neiborhoods = 0;
                
                size_t l = i - 1;
                if (l < 0){
                    l = 0;
                }
                
                size_t k = i + 2;
                if (k > height){
                    k = height;
                }
                
                for (l; l < k; l++){
                    size_t p = j - 1;
                    if (p < 0){
                        p = 0;
                    }
                    
                    size_t n = j + 2;
                    if (n > width){
                        n = width;
                    }
                    
                    for (p; p < n; p++){
                        if (l == i && p == j){
                            continue;
                        }
                        
                        if (map[l][p] == COLORS::red){
                            neiborhoods++;
                        }
                    }
                }
                #pragma omp critical
                {
                    if ((neiborhoods == 2 || neiborhoods == 3) && map[i][j] == COLORS::red){
                        nmap[i][j] = COLORS::red;
                    }
                    else if (map[i][j] != COLORS::red && neiborhoods == 3){
                        nmap[i][j] = COLORS::red;
                    }
                    else{
                        nmap[i][j] = COLORS::black;
                    }
                }
            }
        }
    }
}
