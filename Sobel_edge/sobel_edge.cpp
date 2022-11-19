#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/photo.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
	VideoCapture capture(0);
	Mat frame;
	
	// Default resolutions of the frame are obtained. The default resolutions are system dependent.
	int frame_width = capture.get(CAP_PROP_FRAME_WIDTH);
	int frame_height = capture.get(CAP_PROP_FRAME_HEIGHT);
	int fps = capture.get(CAP_PROP_FPS);
	printf("W x H : %d x %d, FPS : %d\n", frame_width, frame_height, fps);

	//Define the codec and create VideoWriter object. The output is stored in 'outcpp.avi' file.
	VideoWriter video("outcpp.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frame_width, frame_height));

	int count = 0, max;

	if (argc > 1) {
		printf("argv : %s (string type)\n", argv[1]);
		max = atoi(argv[1]);
		printf("argc > 1\nargv : %d (integer type)\n", max);
	}
	else {
		max = 50;
		printf("default max : %d\n", max);
	}

	Mat imgGray;		// Sobel Edge 검출을 위해 Grayscale으로 바꾼 이미지
	Mat sobel_x;		// x방향 Sobel
	Mat sobel_y;		// y방향 Sobel
	Mat result_gray;	// 두 방향을 합친 Sobel
	Mat result;			// Grayscale 이미지를 Colorscale로 영상을 저장하기 위한 변수

	if (!capture.isOpened()) {
		printf("Couldn't open the camera...\n");
		return -1;
	}
	else {
		printf("open camera\n");
	}

	while(1) {
		capture >> frame;
		printf("%d\n", count);
		if ((count > max) || (waitKey(30) >= 0))
			break;

		cvtColor(frame, imgGray, COLOR_BGR2GRAY);
		Sobel(imgGray, sobel_x, CV_8U, 1, 0);
		Sobel(imgGray, sobel_y, CV_8U, 0, 1);
		result_gray = abs(sobel_x) + abs(sobel_y);
		cvtColor(result_gray, result, COLOR_GRAY2BGR);
		
		imshow("Original", frame);
		imshow("Video", result);
		video.write(result);
		count++;
	}

	capture.release();
	video.release();
	destroyAllWindows();
	return 0;
}
