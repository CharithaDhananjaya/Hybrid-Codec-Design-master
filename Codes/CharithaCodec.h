#pragma once
#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include "MalindaHuffman.h"
using namespace std;
using namespace cv;

/* Code Control Class - Inherit with Huffman Encoder Class*/
class CharithaCodeControl:public coolCodec {

public:
	Mat3b creatCanvas(Mat3b ipImage, int blockSize);
	Mat doCompress(Mat imgPlane, int nRows, int nCols, int blockSize);
	Mat doQuant(Mat dctPlane, int nRows, int nCols, int qFactor);
	void zigzagEncode(Mat quantPlane, int nRows, int nCols,vector<int16_t> * arr);
	Mat zigzagDecode(vector<int16_t> zigzagVector, int nRows, int nCols);
	Mat deQuant(Mat quantPlane, int nRows, int nCols, int qFactor);
	Mat deCompress(Mat dctPlane, int nRows, int nCols, int blockSize);

	void doObjectiveEvaluation(Mat originalImage, Mat deodedImage);

private:
	
};

/* Image Codec */
class CharithaImageCodec:public CharithaCodeControl{

public:

	void encode(Mat3b originalImage,int qualityPrm, int blockSize, string outputFile);
	void decode(string oplename);

private:

};

/* Video Codec */
class CharithaVideoCodec:public CharithaCodeControl {

public:

	void encode(string originalVideo);
	void decode(string encodeVideo);

private:

};
