#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <vector>
#include <cstddef> // size_t

class Mandelbrot
{
    public:
        Mandelbrot();
        std::pair<size_t,double*> calc(const int width,const int height,const double start_x,const double end_x,const double start_y,const double end_y,unsigned int max_iter);
        ~Mandelbrot();

    private:
        std::vector<double> points;

};

#endif // MANDELBROT_H
