#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <locale>
#include <unordered_map>

int main () {
    std::ifstream file;

    file.open("text.txt");
    std::string delChar = ".,!:*«()' ";

    std::vector<std::string> strs;
    std::unordered_map<std::string, int> map;

    #pragma omp parallel
    {
        if ( file.is_open() ) {
            while(file.peek() != EOF){
                std::string str;

                #pragma omp critical
                {
                    file >> str;
                }

                #pragma omp for
                for (auto ch : delChar){
                    auto res = str.find(ch);

                    #pragma omp critical
                    {
                        if (res != std::string::npos){
                            str.erase(res);
                        }
                    }
                }

                #pragma omp critical
                {
                    std::transform(str.begin(), str.end(), str.begin(), tolower);
                    strs.push_back(str);
                }
            }
        }
    }


    #pragma omp for
    for (auto &str : strs){
        if (map[str]){
            map[str]++;
        }
        else{
            map[str] = 1;
        }
    }

    for (const std::pair<const std::string, int>& n : map){
        std::cout << n.first << ": " <<  n.second << "\n";
    }

    auto res = std::max_element(map.begin(), map.end(), [](auto &r, auto &l){
        return r.second < l.second;
    });

    std::cout << (*res).first << ":" << (*res).second << std::endl;
}
