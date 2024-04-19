#include <iostream>
#include <string>
#include <complex>
#include <mpi.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define MAS 0.9

double xMin = -2 * MAS;
double xMax = 1 * MAS;
double yMin = -1 * MAS;
double yMax = 1 * MAS;


int GetColor(std::complex<double> &zn, std::complex<double> &z){
    z = zn;
    int Color = 255;

    while(std::norm(z) <= 4 && Color){
        z = std::pow(z, 2) + zn;
        Color--;
    }

    return Color;
}

void fractal(size_t y0, size_t y1, cv::Mat &image){
    double stepx, stepy;
    std::complex<double> zn;

    zn.imag(yMin);

    stepx = (xMax - xMin) / image.rows;
    stepy = (yMax - yMin) / image.cols;
    
    zn.imag(zn.imag() + stepy * y0);

    for(size_t y = y0; y < y1; y++){
        zn.imag(zn.imag() + stepy);
        zn.real(xMin);

        for(size_t x = 0; x < image.rows; x++){
            zn.real( zn.real() + stepx);
            std::complex<double> z;
            
            int color = GetColor(zn, z);

            image.at<cv::Vec3b>(y, x)[0] = color;
            image.at<cv::Vec3b>(y, x)[1] = color;
            image.at<cv::Vec3b>(y, x)[2] = color;
        }
    }
}

int main( int argc, char *argv[] ){
    int rank, size;
    cv::Mat image = cv::Mat::zeros(500, 500, CV_8UC3);
    cv::Mat res = cv::Mat::zeros(500, 500, CV_8UC3);
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    size_t step = 500 / size;
    size_t ost = 500 % size;
    
    size_t srow = rank * step;
    size_t nrow = step * (rank + 1);
    
    for (size_t i = srow; i < nrow; i += step){
        
        if (i < step - 1){
            fractal(i, i + step, image);
        }
        else{
            fractal(i, i + step + ost, image);
        }
    }
    
    void * copy_buf = image.data + srow * image.cols * 3;
    void * res_buf = res.data + srow * image.cols * 3;
    int len = static_cast<int> ((nrow - srow) * image.cols * 3);
    
    std::cout << rank << std::endl;
    
    MPI_Gather(copy_buf, len, MPI_UNSIGNED_CHAR,
             res_buf, len, MPI_UNSIGNED_CHAR, 0,
             MPI_COMM_WORLD);
    
    if (rank == 0){
        cv::imshow("title", res);
        cv::moveWindow("title", 0, 200 );
        cv::waitKey( 0 );
    }
    
    MPI_Finalize();
    return(0);
}
