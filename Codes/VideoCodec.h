#pragma once
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <fstream>
#include <stdint.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <opencv2/videoio/videoio_c.h>
#include "CharithaCodec.h"
#include "MalindaHuffman.h"

using namespace std;
using namespace cv;

struct motionVectorelement {
	int x;
	int y;
};

struct motionVectorBuffer {
	vector<motionVector> vec[3];
};

class videoEncoder {
public:
	void videoEncode(string filename, int qFactor, int blockSize, int nFrames, string opFname);

private:
	motionVectorBuffer GetMV(Mat refFrm, Mat predFrm);
	uint8_t costFunction(Mat& currentBlock, Mat& orgBlock, int blockSize);
	Mat motionCompensationf(Mat orgImage, motionVectorBuffer MVH, int blockSize);
	Mat intraDecode(Mat img, int Qparam, int blockSize);
};

