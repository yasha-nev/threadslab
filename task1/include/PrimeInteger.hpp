#ifndef PrimeInteger_hpp
#define PrimeInteger_hpp

#include <fstream>
#include <map>


/*!
    \brief Большое число

    Расскладывает число типа int на его произведение простых чисел
*/
class PrimeInteger{
    
public:
    /*!
         \brief Базовый Конструктор
    */
    PrimeInteger(){};
    
    /*!
         \brief Конструктор с параметром
         \param [in] n - Натуральное число
    */
    PrimeInteger(int n);
    
    /*!
         \brief Конструктор с параметром
         \param [in] num - уже разложенное натуральное число
    */
    explicit PrimeInteger(std::map<int, int> num) : number(num) {};
    
    /*!
         \brief Диструктор
    */
    ~PrimeInteger() noexcept {};
    
    /*!
         \brief Конструктор копирования
    */
    PrimeInteger(PrimeInteger const& other) : number(other.number) {};
    
    /*!
         \brief Перегрузка оператора присваивания
    */
    PrimeInteger& operator=(PrimeInteger const& other);
    
    /*!
         \brief Конструктор перемещения
    */
    PrimeInteger(PrimeInteger&& other) noexcept : number(other.number) {};
    
    /*!
         \brief Оператор перемещения
    */
    PrimeInteger& operator=(PrimeInteger&& other) noexcept;
    
    /*!
         \brief Перегрузка потока вывода
    */
    friend std::ostream& operator<< (std::ostream &out, const PrimeInteger &prameI);
    
    /*!
         \brief Перегрузка оператора произведения
    */
    friend PrimeInteger operator*(const PrimeInteger &left, const PrimeInteger &rigth);

private:
    std::map<int, int> number;  /*!< Степени простых чисел*/
};
 

#endif /* PrimeInteger_hpp */
