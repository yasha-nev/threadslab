#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <omp.h>

void image2grayScale(cv::Mat &input, cv::Mat &output);

void image2Sepia(cv::Mat &input, cv::Mat &output);

void image2Negative(cv::Mat &input, cv::Mat &output);

void image2countor(cv::Mat &input, cv::Mat &output);

void applySobel(cv::Mat& input, cv::Mat& output);

int main( void ){
    std::string image_path = cv::samples::findFile("/home/lotesadmin/projects/opencvtest/task3/image.webp");
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    cv::Mat grayScale, sepia, negative, contour;

    if(image.empty()){
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    //image2grayScale(image, grayScale);
    //image2Sepia(image, sepia);
    //image2Negative(image, negative);

    applySobel(image, contour);

    //cv::imshow("original", image);
    //cv::imshow("gray", grayScale);
    //cv::imshow("sepia", sepia);
    //cv::imshow("negative", negative);
    cv::imshow("original", contour);

    cv::waitKey(0);
    return 0;
}

void image2grayScale(cv::Mat &input, cv::Mat &output){
    cv::Mat grayScale = cv::Mat::zeros(input.size(), CV_8UC1);

    #pragma omp parallel
    {
        #pragma omp parallel for
        for (size_t i = 0; i < input.rows; i++){
            for (size_t j = 0; j < input.cols; j++){
                cv::Vec3b pixel = input.at<cv::Vec3b>(i, j);
                grayScale.at<uchar>(i, j) = 0.587 * pixel[0] + 0.299 * pixel[1] + 0.114 * pixel[2];
            }
        }
    }
    grayScale.copyTo(output);
}

void image2Sepia(cv::Mat &input, cv::Mat &output){
    cv::Mat sepiaScale = cv::Mat::zeros(input.size(), CV_8UC3);

    #pragma omp parallel
    {
        #pragma omp parallel for
        for (size_t i = 0; i < input.rows; i++){
            for (size_t j = 0; j < input.cols; j++){
                cv::Vec3b pixel = input.at<cv::Vec3b>(i, j);

                float blue =  0.272 * pixel[1] +  0.534 * pixel[2] + 0.131 * pixel[0];
                float green = 0.349 * pixel[1] +  0.686 * pixel[2] + 0.168 * pixel[0];
                float red = 0.393 * pixel[1] +  0.769 * pixel[2] + 0.189 * pixel[0];;

                sepiaScale.at<cv::Vec3b>(i, j)[0]= (blue < 255) ? blue : 255;
                sepiaScale.at<cv::Vec3b>(i, j)[1]= (green < 255) ? green : 255;
                sepiaScale.at<cv::Vec3b>(i, j)[2]= (red < 255) ? red : 255 ;
            }
        }
    }
    sepiaScale.copyTo(output);
}

void image2Negative(cv::Mat &input, cv::Mat &output){
    cv::Mat sepiaScale = cv::Mat::zeros(input.size(), CV_8UC3);

    #pragma omp parallel
    {
        #pragma omp parallel for
        for (size_t i = 0; i < input.rows; i++){
            for (size_t j = 0; j < input.cols; j++){
                cv::Vec3b pixel = input.at<cv::Vec3b>(i, j);
                sepiaScale.at<cv::Vec3b>(i, j)[1]= 255 - pixel[1];
                sepiaScale.at<cv::Vec3b>(i, j)[2]= 255 - pixel[2];
                sepiaScale.at<cv::Vec3b>(i, j)[0]= 255 - pixel[0];
            }
        }
    }
    sepiaScale.copyTo(output);
}

void image2countor(cv::Mat &input, cv::Mat &output){
    cv::Mat sepiaScale = cv::Mat::zeros(input.size(), CV_8UC3);

    #pragma omp parallel
    {
        #pragma omp parallel for
        for (size_t i = 0; i < input.rows; i++){
            for (size_t j = 0; j < input.cols; j++){
                cv::Vec3b pixel = input.at<cv::Vec3b>(i, j);
                sepiaScale.at<cv::Vec3b>(i, j)[1]= 255 - pixel[1];
                sepiaScale.at<cv::Vec3b>(i, j)[2]= 255 - pixel[2];
                sepiaScale.at<cv::Vec3b>(i, j)[0]= 255 - pixel[0];
            }
        }
    }
    sepiaScale.copyTo(output);
}

void applySobel(cv::Mat& input, cv::Mat& output) {
    cv::RNG rng(12345);
    cv::Mat sobel = cv::Mat::zeros(input.size(), CV_8UC1);
    cv::Mat zero = cv::Mat::zeros(input.size(), CV_8UC1);

    cv::Mat sobelX = (cv::Mat_<int>(3, 3) << -1, 0, 1,
                                             -2, 0, 2,
                                             -1, 0, 1);

    cv::Mat sobelY = (cv::Mat_<int>(3, 3) << 1, 2, 1,
                                             0, 0, 0,
                                            -1, -2, -1);


    for (int i = 0; i < input.rows; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            //int sumX = 0;
            int sumY = 0;

            for (int k = -1; k <= 1; ++k) {
                for (int l = -1; l <= 1; ++l) {
                    int rowIndex = i + k;
                    int colIndex = j + l;

                    if (rowIndex >= 0 && rowIndex < input.rows && colIndex >= 0 && colIndex < input.cols) {

                        //sumX += input.at<uchar>(rowIndex, colIndex) * sobelX.at<int>(k + 1, l + 1);
                        sumY += input.at<uchar>(rowIndex, colIndex) * sobelY.at<int>(k + 1, l + 1);

                    }
                }
            }

            int gradient = sumY;
            sobel.at<uchar>(i, j) = cv::saturate_cast<uchar>(gradient);
        }
    }
    image2Negative(sobel, output);

    //sobel.copyTo(output);
}
