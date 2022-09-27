
#include <cmath>
#include <complex.hpp>

//конструктор
Complex::Complex():real(0.0d),imaginary(0.0d){}    //коструктор по умолчанию
Complex::Complex(double r,double i):real(r),imaginary(i){}  // конструктор

 // оператор сложения
Complex Complex::operator+(const Complex& cn) const{
        return Complex(real+cn.real,imaginary+cn.imaginary);
}

// геттеры
double Complex::get_real() const{ return real;}
double Complex::get_imaginary() const{return imaginary;}


// возвращает квадрат комплексного числа
Complex square(const Complex& cn){
    return Complex(cn.get_real()*cn.get_real()-cn.get_imaginary()*cn.get_imaginary(),2*cn.get_real()*cn.get_imaginary());
}

// возвращает квадрат модуля комплексного числа
double sqr_abs(const Complex& cn){
    return (pow(cn.get_real(),2)+pow(cn.get_imaginary(),2));
}

