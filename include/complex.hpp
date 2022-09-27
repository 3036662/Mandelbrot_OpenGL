#ifndef COMPLEX_HPP_INCLUDED
#define COMPLEX_HPP_INCLUDED

class Complex{
private:
    double real;
    double imaginary;
public:
    Complex();
    Complex(double,double);
    Complex operator+(const Complex&) const;
    double get_real() const;
    double get_imaginary() const;
};

Complex square(const Complex& );
double sqr_abs(const Complex&);

#endif // COMPLEX_HPP_INCLUDED
