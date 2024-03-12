#include <iostream>
#include <sstream>
#include <omp.h>

int main(int argc, char *argv[]){
    int sum = 0;
    int thrs;
    int num;
    
    if (argc < 3){
        std::cout << "Не введены K и N\n";
        return 0;
    }
    
    std::stringstream arg1(argv[1]);
    std::stringstream arg2(argv[2]);
    
    arg1 >> thrs;
    arg2 >> num;
    
    if (!arg1 || !arg2){
        std::cout << "Ошибка\n";
        return 0;
    }
    
    omp_set_num_threads(thrs);
    
    #pragma omp parallel reduction(+:sum)
    {
        #pragma omp for
        for (int i = 0; i < num + 1; i++)
        {
            sum += i;
        }
        
        #pragma critical
        {
            std::cout << omp_get_thread_num() << " " << sum << "\n";
        }
    }
    
    std::cout << "SUM = " << sum << "\n";
    return 0;
}
