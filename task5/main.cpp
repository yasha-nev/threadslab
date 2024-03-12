#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>

void merge_sort(std::vector<int> &arr, size_t left, size_t right){
    if (right - left > 1){
        size_t mid = (left + right) / 2;
        size_t l = left;
        size_t r = mid + 1;
        std::vector<int> tmp(right - left + 1);
        
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        
        for (size_t i = 0; i < tmp.size(); i++){
            if (arr[l] > arr[r] && l < mid + 1){
                tmp[i] = arr[l];
                l++;
            }
            else if (arr[r] > arr[l] && r < right + 1){
                tmp[i] = arr[r];
                r++;
            }
            else if (l >= left && r < right + 1){
                tmp[i] = arr[r];
                r++;
            }
            else if(r >= mid + 1 && l < mid + 1){
                tmp[i] = arr[l];
                l++;
            }
            else{
                continue;
            }
        }
        
        std::copy(tmp.begin(), tmp.end(), arr.begin() + left);
        
    }
    else {
        if (arr[left] < arr[right]){
            std::swap(arr[left], arr[right]);
        }
    }
}

void merge_sort_fast(std::vector<int> &arr, size_t left, size_t right){
    #pragma omp parallel
    {
        if (right - left > 1){
            size_t mid = (left + right) / 2;
            size_t l = left;
            size_t r = mid + 1;
            std::vector<int> tmp(right - left + 1);
            
            #pragma omp sections
            {
                #pragma omp section
                merge_sort(arr, left, mid);
                
                #pragma omp section
                merge_sort(arr, mid + 1, right);
            }
                
            for (size_t i = 0; i < tmp.size(); i++){
                if (arr[l] > arr[r] && l < mid + 1){
                    tmp[i] = arr[l];
                    l++;
                }
                else if (arr[r] > arr[l] && r < right + 1){
                    tmp[i] = arr[r];
                    r++;
                }
                else if (l >= left && r < right + 1){
                    tmp[i] = arr[r];
                    r++;
                }
                else if(r >= mid + 1 && l < mid + 1){
                    tmp[i] = arr[l];
                    l++;
                }
                else{
                    continue;
                }
            }
            
            std::copy(tmp.begin(), tmp.end(), arr.begin() + left);
            
            
        }
        else {
            if (arr[left] < arr[right]){
                std::swap(arr[left], arr[right]);
            }
        }
    }
}

int main(){
    std::vector<int> arr(1000000);
    
    for (size_t i = 0; i < arr.size(); i++){
        arr[i] = rand() % 1000;
    }
    
    std::vector<int> arr2(arr);
    
    auto begin = std::chrono::steady_clock::now();
    
    merge_sort(arr, 0, arr.size() - 1);
    
    auto end = std::chrono::steady_clock::now();
    
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    
    std::cout << "Время работы обыной сортировки: " << elapsed_ms.count() << std::endl;
    
    
    begin = std::chrono::steady_clock::now();
    
    merge_sort_fast(arr, 0, arr.size() - 1);
    
    end = std::chrono::steady_clock::now();
    
    elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    
    std::cout << "Время работы сортировки с omp: " << elapsed_ms.count() << std::endl;
}
