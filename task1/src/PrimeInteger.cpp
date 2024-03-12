#include "PrimeInteger.hpp"

PrimeInteger::PrimeInteger(int n){
    for(int j = 2; j * j <= n; j++){
        
        if ( n % j){
            continue;
        }
        
        while(n % j == 0){
            number[j]++;
            n /= j;
        }
    }
    if (n > 1){
        number[n]++;
    }
};

PrimeInteger& PrimeInteger::operator=(PrimeInteger const& other) {
    PrimeInteger temp(other);
    std::swap(number, temp.number);
    return *this;
}

PrimeInteger& PrimeInteger::operator=(PrimeInteger&& other) noexcept {
    PrimeInteger temp(std::move(other));
    std::swap(number, temp.number);
    return *this;
}

std::ostream& operator<< (std::ostream &out, const PrimeInteger &prameI) {
    for(auto itr : prameI.number){
        out << itr.first << "^" << itr.second << " ";
    }
    return out;
}

PrimeInteger operator*(const PrimeInteger &left, const PrimeInteger &rigth){
    PrimeInteger primeI(left);
    
    for (auto number : rigth.number){
        primeI.number[number.first] += number.second;
    }
    
    return primeI;
}
