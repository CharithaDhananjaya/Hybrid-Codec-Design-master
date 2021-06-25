#include "CharithaCodec.h"
#include "MalindaHuffman.h"

//Code Control Functions
Mat3b CharithaCodeControl::creatCanvas(Mat3b ipImage, int blockSize)
{
	int imgHeight = ipImage.rows;
	int imgWidth = ipImage.cols;
	if ((imgHeight % blockSize) != 0 || (imgHeight % blockSize) != 0) {

		int canvasHeight, canvasWidth;
		canvasHeight = imgHeight + blockSize - (imgHeight % blockSize);
		canvasWidth = imgWidth + blockSize - (imgWidth % blockSize);

		Mat3b imgCanvasodd(canvasHeight, canvasWidth, Vec3b(255, 255, 255));
		ipImage.copyTo(imgCanvasodd(Rect(0, 0, ipImage.cols, ipImage.rows)));

		return imgCanvasodd;
	}
	else {
		Mat3b imgCanvasevn = ipImage;
		return imgCanvasevn;
	}
}

Mat CharithaCodeControl::doCompress(Mat imgPlane, int nRows, int nCols, int blockSize)
{
	Mat newImage(Size(size(imgPlane)), CV_32FC1);
	//newImage = 0;

	for (int rows = 0; rows < nRows - (blockSize-1); rows += blockSize) {
		for (int cols = 0; cols < nCols - (blockSize-1); cols += blockSize) {
			Mat dctBlock = imgPlane(Rect(cols, rows, blockSize, blockSize));
			Mat outBlock;
			dctBlock.convertTo(dctBlock, CV_32FC1, 1, 0);
			dct(dctBlock, outBlock);
			outBlock.copyTo(newImage(Rect(cols, rows, blockSize, blockSize)));
		}
	}
	return newImage;
}

Mat CharithaCodeControl::doQuant(Mat dctPlane, int nRows, int nCols, int qFactor)
{
	dctPlane.convertTo(dctPlane, CV_16SC1);
	int qParameter = pow(2,qFactor);
	Mat quantPlane = dctPlane / qParameter;

	return quantPlane;
}

void CharithaCodeControl::zigzagEncode(Mat quantPlane, int nRows, int nCols,vector<int16_t> * arr)
{
	for (int row = 0; row < nRows; row++) {
		for (int col = 0; col < nCols; col++) {
			arr->push_back(quantPlane.at<int8_t>(row, col));
		}
	}
}

Mat CharithaCodeControl::zigzagDecode(vector<int16_t> zigzagVector, int nRows, int nCols)
{
	Mat quantPlane(Size(nRows,nCols),CV_16SC1);
	int i = 0;

	for (int row = 0; row < nRows; row++) {
		for (int col = 0; col < nCols; col++) {
			quantPlane.at<int16_t>(row, col) = zigzagVector[i + col];
		}
		i += 8;
	}
	return quantPlane;
}

Mat CharithaCodeControl::deQuant(Mat quantPlane, int nRows, int nCols, int qFactor)
{
	int qParameter = pow(2,qFactor);
	Mat dequantPlane = quantPlane * qParameter;
	dequantPlane.convertTo(dequantPlane, CV_32FC1, 1, 0);

	return dequantPlane;
}

Mat CharithaCodeControl::deCompress(Mat dctPlane, int nRows, int nCols, int blockSize)
{
	Mat newImage(Size(size(dctPlane)), CV_8UC1);

	for (int rows = 0; rows < nRows - (blockSize - 1); rows += blockSize) {
		for (int cols = 0; cols < nCols - (blockSize - 1); cols += blockSize) {
			Mat idctBlock = dctPlane(Rect(cols, rows, blockSize, blockSize));
			Mat outBlock;
			idct(idctBlock, outBlock);
			outBlock.convertTo(outBlock, CV_8UC1);
			outBlock.copyTo(newImage(Rect(cols, rows, blockSize, blockSize)));
		}
	}
	return newImage;
}

//Objective Evaluations
void CharithaCodeControl::doObjectiveEvaluation(Mat originalImage, Mat decodedImage)
{
	Mat splitOriginal[3], splitDecode[3];
	split(originalImage, splitOriginal);
	split(decodedImage, splitDecode);
	int rows = originalImage.rows, cols = originalImage.cols;
	Mat MSEp[3];
	double MSE = 0.00, PSNR = 0.00;
	int rSum = 0;

	MSEp[0] = splitOriginal[0] - splitDecode[0];
	MSEp[0] = MSEp[0].mul(MSEp[0]);
	MSEp[1] = splitOriginal[1] - splitDecode[1];
	MSEp[1] = MSEp[1].mul(MSEp[1]);
	MSEp[2] = splitOriginal[2] - splitDecode[2];
	MSEp[2] = MSEp[2].mul(MSEp[2]);

	
	MSE = (sum(MSEp[0])[0] + sum(MSEp[1])[0] + sum(MSEp[2])[0]) / (3*rows*cols);

	PSNR = 20*log(150)-10*log(MSE);

	cout << "\n *** MSE *** \n" << MSE << "\n *** PSNR *** \n" << PSNR;

}

//Image Encoding and Decoding Functions
void CharithaImageCodec::encode(Mat3b originalImage,int qualityPrm, int blockSize, string outputFile)
{

	//Read the Image and preImage create with a canvas 
	Mat3b ipImage = originalImage;
	Mat3b sampleImage = creatCanvas(ipImage,blockSize);
	Mat preImage = sampleImage;
	
	//Splitting the image to BGR planes and idctPlanes are also initiated 
	Mat  splitImage[3], idctPlane[3];
	split(preImage, splitImage);

	//Compressing each plane
	splitImage[0] = doCompress(splitImage[0], preImage.rows, preImage.cols, blockSize);
	splitImage[1] = doCompress(splitImage[1], preImage.rows, preImage.cols, blockSize);
	splitImage[2] = doCompress(splitImage[2], preImage.rows, preImage.cols, blockSize);

	//Quantizing the dctPlane
	splitImage[0] = doQuant(splitImage[0], preImage.rows, preImage.cols, qualityPrm);
	splitImage[1] = doQuant(splitImage[1], preImage.rows, preImage.cols, qualityPrm);
	splitImage[2] = doQuant(splitImage[2], preImage.rows, preImage.cols, qualityPrm);

	//Huffman Encode
	// 1.) Generating Huffman Table
	map<string, vector<bool>> huffmanCodeBook[3];
	huffmanCodeBook[0] = Huffman_dict(splitImage[0], outputFile);
	huffmanCodeBook[1] = Huffman_dict(splitImage[1], outputFile);
	huffmanCodeBook[2] = Huffman_dict(splitImage[2], outputFile);
	// 2.) Encoding the Image
	HuffmanEncode(huffmanCodeBook[0], splitImage[0], outputFile);
	HuffmanEncode(huffmanCodeBook[1], splitImage[1], outputFile);
	HuffmanEncode(huffmanCodeBook[2], splitImage[2], outputFile);
	//Huffman Decode
	
	//DeQuantizing the H.Decoded Planes
	splitImage[0] = deQuant(splitImage[0], preImage.rows, preImage.cols, qualityPrm);
	splitImage[1] = deQuant(splitImage[1], preImage.rows, preImage.cols, qualityPrm);
	splitImage[2] = deQuant(splitImage[2], preImage.rows, preImage.cols, qualityPrm);
																		
	//Taking the idct of the quantized planes
	idctPlane[2] = deCompress(splitImage[2], preImage.rows, preImage.cols, blockSize);
	idctPlane[0] = deCompress(splitImage[0], preImage.rows, preImage.cols, blockSize);
	idctPlane[1] = deCompress(splitImage[1], preImage.rows, preImage.cols, blockSize);

	//Merging Planes
	vector<Mat> allPlanes;
	Mat deImage(preImage.rows, preImage.cols, CV_16SC3, Scalar::all(0));
	allPlanes.push_back(idctPlane[0]);
	allPlanes.push_back(idctPlane[1]);
	allPlanes.push_back(idctPlane[2]);
	merge(allPlanes, deImage);

	deImage = deImage(Rect(0, 0, ipImage.cols, ipImage.rows));

	doObjectiveEvaluation(ipImage, deImage);
}

void CharithaImageCodec::decode(string filename)
{

}

//Video Encoding and Decoding Functions
void CharithaVideoCodec::encode(string originalVideo)
{
	VideoCapture video;
	video.open(originalVideo);
	double fps = video.get(CAP_PROP_FPS);

}

void CharithaVideoCodec::decode(string encodeVideo)
{
}
