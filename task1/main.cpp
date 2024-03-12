#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <sstream>
#include "PrimeInteger.hpp"

void factorialPiece(int n, int m, std::promise<PrimeInteger> && primeI){
    std::map<int, int> res;
    
    int i;
    
    n <= 1 ? i = 2 : i = n;
    
    for(; i <= m; i++){
        
        int sup = i;
        
        for(int j = 2; j * j <= sup; j++){
            if ( sup % j){
                continue;
            }
            
            while(sup % j == 0){
                res[j]++;
                sup /= j;
            }
        }
        if (sup > 1){
            res[sup]++;
        }
    }
    primeI.set_value(PrimeInteger(res));
}


int main(int argc, char * argv[]){
    int n;
    PrimeInteger res;
    
    if (argc < 2){
        std::cout << "Не введено число для нахождения факториала.\n";
        std::cout << "Пример: ./main 100" << std::endl;
        return 0;
    }
    
    std::istringstream ss(argv[1]);
    
    if (!(ss >> n)) {
      std::cerr << "Неверно введено число: " << argv[1] << '\n';
    }
    
    int threadCount = std::thread::hardware_concurrency();
    
    if (n <= threadCount){
        threadCount = 1;
    }
    
    std::future<PrimeInteger> futures[threadCount];
    std::thread thrs[threadCount];
    
    int step = n / threadCount;
    
    auto begin = std::chrono::steady_clock::now();
    
    for (int i = 0; i < threadCount; i++){
        int second;
        std::promise<PrimeInteger> promise;
        futures[i] = promise.get_future();
        i == threadCount - 1 ? second = n : second = step * (i + 1);
        thrs[i] = std::thread(&factorialPiece, step * i, second, std::move(promise));
    }
    
    for (int i = 0; i < threadCount; i++){
        thrs[i].join();
        res = res * futures[i].get();
    }
    
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    
    std::cout << "Результат: " <<res << std::endl;
    std::cout << "Время расчета: " << elapsed_ms.count() << " ns\n\n";
}
