#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace cv;
using namespace std;

int main() {
  VideoCapture cap(0);

  if (!cap.isOpened()) {
    cerr << "Не удалось открыть камеру!" << endl;
    return -1;
  }

  auto transform_image = [](const Mat& frame) {
    Mat rotatedFrame;
    rotate(frame, rotatedFrame, ROTATE_90_CLOCKWISE);
    return rotatedFrame;
  };

  auto start_total = chrono::high_resolution_clock::now();
  int frame_count = 0;
  long long total_input_time = 0;
  long long total_transform_time = 0;
  long long total_output_time = 0;

  while (true) {
    auto start_input = chrono::high_resolution_clock::now();
    Mat frame;
    bool ret = cap.read(frame);
    auto end_input = chrono::high_resolution_clock::now();
    total_input_time += chrono::duration_cast<chrono::microseconds>(end_input - start_input).count();

    if (!ret) {
      break;
    }

    auto start_transform = chrono::high_resolution_clock::now();
    Mat transformed_frame = transform_image(frame);
    auto end_transform = chrono::high_resolution_clock::now();
    total_transform_time += chrono::duration_cast<chrono::microseconds>(end_transform - start_transform).count();

    auto start_output = chrono::high_resolution_clock::now();
    imshow("Video", transformed_frame);
    auto end_output = chrono::high_resolution_clock::now();
    total_output_time += chrono::duration_cast<chrono::microseconds>(end_output - start_output).count();


    frame_count++;
    auto end_time = chrono::high_resolution_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_total);
    if (elapsed_time.count() >= 1000) {
      double fps = (double)frame_count / (elapsed_time.count() / 1000.0);
      cout << "FPS: " << fps << endl;

      long long total_time = total_input_time + total_transform_time + total_output_time;
      cout << "Input time: " << (double)total_input_time / 1000.0 << "ms (" << (double)total_input_time * 100.0 / total_time << "%)" << endl;
      cout << "Transform time: " << (double)total_transform_time / 1000.0 << "ms (" << (double)total_transform_time * 100.0 / total_time << "%)" << endl;
      cout << "Output time: " << (double)total_output_time / 1000.0 << "ms (" << (double)total_output_time * 100.0 / total_time << "%)" << endl;
      cout << "Total time: " << (double)total_time / 1000.0 << "ms" << endl;


      start_total = chrono::high_resolution_clock::now();
      frame_count = 0;
      total_input_time = 0;
      total_transform_time = 0;
      total_output_time = 0;
    }

    if (waitKey(1) == 'q') {
      break;
    }
  }

  cap.release();
  destroyAllWindows();
  return 0;
}

