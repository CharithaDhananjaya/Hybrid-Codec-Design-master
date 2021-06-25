#include "VideoCodec.h"
#include <opencv2/videoio/videoio_c.h>
#include "CharithaCodec.h"

/*This part of the code is developed for the video encoding part.
  openCV and C++ libraries are used.
  Refered to the papers and Matlab Codes in the References folder in the repository.
  Debuging and implimentation are supported by Malinda.*/

void videoEncoder::videoEncode(string filename, int qFactor, int blockSize, int nFrames, string opFname)
{
	VideoCapture cap;

	cap.open(filename);
	double fps = cap.get(CAP_PROP_FPS);

	Mat currentFrm, previousFrm;
	int tot_frame = cap.get(CAP_PROP_FRAME_COUNT);
	int Iframe_interval = 9;
	int frmCount = 10;
	int i = 0;
	Mat motionCompensation, output, residual;
	int Width = 0, Height = 0;
	// CV_FOURCC will not work with every version of openCV
	VideoWriter video(opFname, CV_FOURCC('M', 'J', 'P', 'G'), fps, Size(1280, 720));

	while (i < nFrames) {
		if (i % 9 == 0) {
			cap >> currentFrm;
			previousFrm = currentFrm.clone();
		}
		else
		{	
			cap >> currentFrm;
		}

		motionVectorBuffer MotionVec;
		MotionVec = GetMV(currentFrm, previousFrm);

		Width = previousFrm.cols;
		Height = previousFrm.rows;

		motionCompensation = motionCompensationf(previousFrm, MotionVec, 16);

		residual = currentFrm - motionCompensation;

		residual = intraDecode(residual, qFactor, blockSize);

		output = residual + motionCompensation;
		video.write(output);
		i++;
	}
	video.release();

}

Mat videoEncoder::intraDecode(Mat img, int Qparam, int blockSize)
{
	CharithaImageCodec encodeResidual;
		encodeResidual.encode(img, Qparam, blockSize, "new");

}


uint8_t videoEncoder::costFunction(Mat& currentBlock, Mat& orgBlock, int blockSize)
{
	uint8_t error = 0;
	for (int i = 0; i < blockSize; i++) {
		for (int j = 0; j < blockSize; j++) {

			error += abs(currentBlock.at<uint8_t>(i, j) - orgBlock.at<uint8_t>(i, j));

		}

	}

	return (error / (blockSize*blockSize));
}

motionVectorBuffer videoEncoder::GetMV(Mat IFrame, Mat PFrame)
{
	int runingSum = 0;
	int marcoBlockSize = 16; 
	int p = 2;
	int dx, dy;
	Mat Isamp[3];
	Mat Psamp[3];
	split(IFrame, Isamp);
	split(PFrame, Psamp);

	Mat costs = Mat(2 * p + 1, 2 * p + 1, CV_16UC1, Scalar::all(255));
	vector<motionVectorelement> Motnvec[3];


	for (int plane = 0; plane < 3; plane++) {
		Motnvec[plane].reserve(IFrame.rows* IFrame.cols / (marcoBlockSize ^ 2));


		for (int i = 0; i <= (Isamp[plane].rows - marcoBlockSize); i += marcoBlockSize) {
			for (int j = 0; j <= (Isamp[plane].cols - marcoBlockSize); j += marcoBlockSize) {

				Mat block = IFrame(Rect(j, i, marcoBlockSize, marcoBlockSize));
				for (int m = -p; m <= p; m++) {
					for (int n = -p; n <= p; n++) {
						int refV = j + n;
						int refH = i + m;
						if (refV < 0 || (refV + marcoBlockSize) > Isamp[plane].cols ||
							refH < 0 || (refH + marcoBlockSize) > Isamp[plane].rows) {
							continue;
						}

						costs.at<uint16_t>(m + p, n + p) = cost(Isamp[plane](Rect(refV, refH, marcoBlockSize, marcoBlockSize)),
							Psamp[plane](Rect(j, i, marcoBlockSize,
								marcoBlockSize)), marcoBlockSize);
					}
				}
				int min = 65536;
				dx = 0; dy = 0;
				for (int a = 0; a < costs.rows; a++) {
					for (int b = 0; b < costs.cols; b++) {
						if (costs.at<uint16_t>(a, b) <= min) {

							min = costs.at<uint16_t>(a, b);
							dx = b; dy = a;

						}

					}
				}

				motionVectorelement tempMv;
				tempMv.x = dx - p - 1;
				tempMv.y = dy - p - 1;

				Motnvec[plane].push_back(tempMv);
				costs = Mat(2 * p + 1, 2 * p + 1, CV_16UC1, Scalar::all(255));
			}
		}

	}

	motionVectorBuffer output;
	output.vec[0] = Motnvec[0];
	output.vec[1] = Motnvec[1];
	output.vec[2] = Motnvec[2];

	return output;
}


Mat videoEncoder::motionCompensationf(Mat orgImage, motionVectorBuffer MVH, int blockSize)
{

	int pxlcnt = 0;
	Mat img[3];
	vector<Mat> array_to_merge;
	split(orgImage, img);

	for (int plane = 0; plane < 3; plane++) {
		pxlcnt = 0;
		Mat output = Mat(orgImage.rows, orgImage.cols, CV_8UC1, Scalar::all(255));
		for (int i = 0; i <= orgImage.rows - blockSize; i += blockSize) {
			for (int j = 0; j <= orgImage.cols - blockSize; j += blockSize) {

				int dx = MVH.vec[plane][pxlcnt].x;
				int dy = MVH.vec[plane][pxlcnt].y;
				pxlcnt++;

				int refV = j + dx;
				int refH = i + dy;

				img[plane](Rect(refV, refH, blockSize, blockSize)).copyTo(output(Rect(j, i, blockSize, blockSize)));
			}

		}

		array_to_merge.push_back(output);
	}

	Mat color(orgImage.rows, orgImage.cols, CV_8SC3, Scalar::all(0));
	merge(array_to_merge, color);
	return color;
}
