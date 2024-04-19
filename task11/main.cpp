#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <mpi.h>
#include <chrono>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

#define FRAMES 622

void detectAndDisplay( Mat &frame );

void saveVideo(std::vector<Mat> &frames, int fourcc, double fps, Size capsize);

CascadeClassifier face_cascade;

CascadeClassifier eyes_cascade;

CascadeClassifier smile_cascade;


int main( int argc, char* argv[] ){
    
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if( !face_cascade.load("data/haarcascades/haarcascade_frontalcatface.xml"  ) ){
        cout << "--(!)Error loading face cascade\n";
        MPI_Finalize();
        return -1;
    };

    if( !eyes_cascade.load("data/haarcascades/haarcascade_eye.xml") ){
        cout << "--(!)Error loading eyes cascade\n";
        MPI_Finalize();
        return -1;
    };
    
    if (!smile_cascade.load(samples::findFile("data/haarcascades/haarcascade_smile.xml"))){
        cout << "--(!)Error loading smile cascade\n";
        MPI_Finalize();
        return -1;
    }
    
    VideoCapture capture("ZUA.mp4");
    
    if ( !capture.isOpened() ){
        cout << "--(!)Error opening video capture\n";
        MPI_Finalize();
        return -1;
    }
    
    
    int tag1 = 10;
    int tag2 = 20;
    int tag3 = 30;
    int tag4 = 40;
    int tag5 = 50;
    
    auto begin = std::chrono::steady_clock::now();
    if (rank == 0){
        Mat frame;
        int frame_count = 0;
        
        while (true){
            capture.read(frame);
            
            
            if( frame.empty() ) {
                cout << "--(!) No captured frame -- Break!\n";
                break;;
            }
            
            if (frame_count == 0){
                int width = frame.cols;
                int height = frame.rows;
                int count = capture.get(CAP_PROP_FRAME_COUNT);
                
                
                for (int i = 1; i < size; i++){
                    MPI_Send(&width, 1, MPI_INT, i, tag1 + i, MPI_COMM_WORLD);
                    MPI_Send(&height, 1, MPI_INT, i, tag2 + i, MPI_COMM_WORLD);
                    MPI_Send(&count, 1, MPI_INT, i, tag3 + i, MPI_COMM_WORLD);
                }
                
            }
            
            int rank_id = frame_count % (size - 2) + 1;
            
            MPI_Send(frame.data, frame.rows * frame.cols * 3, MPI_UNSIGNED_CHAR, rank_id, tag4 + rank_id, MPI_COMM_WORLD);
            
            frame_count++;
        }
    }
    else if(rank > 0 && rank < size - 1){
        MPI_Status status;
        int width = 0;
        int height = 0;
        int count = 0;
        
        MPI_Recv(&width, 1, MPI_INT, 0, tag1 + rank, MPI_COMM_WORLD, &status);
        MPI_Recv(&height, 1, MPI_INT, 0, tag2 + rank, MPI_COMM_WORLD, &status);
        MPI_Recv(&count, 1, MPI_INT, 0, tag3 + rank, MPI_COMM_WORLD, &status);
        
        Mat frame = Mat::zeros(Size(width, height), CV_8UC3);
        
        for (int i = 0; i < count / (size - 2); i++){
            MPI_Recv(frame.data, frame.rows * frame.cols * 3, MPI_UNSIGNED_CHAR, 0, tag4 + rank, MPI_COMM_WORLD, &status);
            
            detectAndDisplay(frame);
            
            MPI_Send(frame.data, frame.rows * frame.cols * 3, MPI_UNSIGNED_CHAR, 3, tag5 + rank, MPI_COMM_WORLD);
        }
    }
    else if(rank == size - 1){
        int width = 0;
        int height = 0;
        int count = 0;
        MPI_Status status;
        MPI_Recv(&width, 1, MPI_INT, 0, tag1 + rank, MPI_COMM_WORLD, &status);
        MPI_Recv(&height, 1, MPI_INT, 0, tag2 + rank, MPI_COMM_WORLD, &status);
        MPI_Recv(&count, 1, MPI_INT, 0, tag3 + rank, MPI_COMM_WORLD, &status);
        
        Mat frame = Mat::zeros(Size(width, height), CV_8UC3);
        std::vector <Mat> frames;
        for (int i = 0; i < capture.get(CAP_PROP_FRAME_COUNT); i++){
            int id = i % 2 + 1;
            
            MPI_Recv(frame.data, frame.rows * frame.cols * 3, MPI_UNSIGNED_CHAR, id, tag5 + id, MPI_COMM_WORLD, &status);
            frames.push_back(frame.clone());
            cout << i << endl;
            
        }
        saveVideo(frames, capture.get(CAP_PROP_FOURCC), capture.get(CAP_PROP_FPS), Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT)));
    }

    auto end = std::chrono::steady_clock::now();
    
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "The time: " << elapsed_ms.count() << " ms\n";
    
    capture.release();
        
    MPI_Finalize();
    return 0;
}

void detectAndDisplay( Mat &frame ){
    Mat frame_gray, blur;
    GaussianBlur(frame, blur, Size(9, 9), 3);
    cvtColor( blur, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    std::vector<Rect> faces;
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 1);
    for ( size_t i = 0; i < faces.size(); i++ ){
        Rect face = Rect(faces[i].x - 50, faces[i].y - 50, faces[i].width + 100, faces[i].height + 100);
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        Mat faceROI = frame_gray(face);

        std::vector<Rect> eyes;
        std::vector<Rect> smiles;
        
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 25);
        smile_cascade.detectMultiScale( faceROI, smiles, 1.1, 30, 0, Size(30, 30), Size(120, 120));
        
        if (eyes.size() == 2){
            for ( size_t j = 0; j < eyes.size(); j++ ){
                Point eye_center( face.x + eyes[j].x + eyes[j].width/2, face.y + eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4 );
            }
        
            for ( size_t j = 0; j < smiles.size(); j++ ){
                Point smile_center( face.x + smiles[j].x + smiles[j].width/2, face.y + smiles[j].y + smiles[j].height/2 );
                int radius = cvRound( (smiles[j].width + smiles[j].height)*0.25 );
                circle( frame, smile_center, radius, Scalar( 0, 255, 0 ), 4 );
            }
        
            ellipse( frame, center, Size( face.width/2, face.height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4 );
        }
    }
}

void saveVideo(std::vector<Mat> &frames, int fourcc, double fps, Size capsize){
    VideoWriter out("output.mp4", fourcc, fps, capsize);
    if (!out.isOpened()){
        cout << "--(!)Error open video\n";
        return;
    };
    
    std::cout << frames.size() << std::endl;
    
    for (const auto &frame : frames){
        if (frame.empty()){
            break;
        }
        
        cv::imshow("frame", frame);
        
        if (waitKey(10) == 27){
            break;
        }
    }
    
    out.release();
}
