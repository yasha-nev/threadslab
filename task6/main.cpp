#include <iostream>
#include <vector>
#include <random>
#include <utility>

bool checkCircle(std::pair<float, float> &point){
    if ((point.first - 0.5) * (point.first - 0.5) + (point.second - 0.5) * (point.second - 0.5) < 0.5 * 0.5){
        return 1;
    }
    return 0;
}

int main(){
    float pointInCircle = 0;
    int numPoint = 1000000;
    std::vector<std::pair<float, float>> points(numPoint);
    
    #pragma omp parallel for
    for (auto &point : points){
        point.first = static_cast<float>(rand() % 1000) / 1000;
        point.second = static_cast<float>(rand() % 1000) / 1000;
    }
    
    #pragma omp parallel reduction(+:pointInCircle)
    {
        #pragma omp for
        for (auto &point : points){
            if (checkCircle(point)){
                pointInCircle += 1;
            }
        }
    }
    
    std::cout << "Число Пи: " << 4 * pointInCircle / numPoint << std::endl;
    
}
