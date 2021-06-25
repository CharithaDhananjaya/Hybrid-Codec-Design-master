#include "CharithaCodec.h"
#include "VideoCodec.h"

/*Please note that this main function is developed just for the demonstraion.
  The cCode Controller parameters are changed as shown in the code but saccess to the some
  parameters msy not direct in this version. Further developments may conclude with those.*/

int main(int argc, char** argv) {

	cout << " EE 596 Image and Video Coding \n Mini Project - Hybrid Codec \n Peiris R.C.P.D [E/13/251]\n" << endl;
	string inputFile, outputFile;
	int qualityPrm, blockSize, selectMode;
	cout << "Select Mode: [ 1-Image Mode, 2-Video Mode] : ";
	cin >> selectMode;
	
	if (selectMode == 1) {
		cout << "\n Image Mode Selected... \n";
		cout << "\n Enter Input Image File Name : ";
		cin >> inputFile;
		cout << " \n Enter Quality [0 to 10 Scale , 0 - Low Quality , 10 - High Quality] : ";
		cin >> qualityPrm;
		cout << "\n Enter the Compression Level [ 1-Low, 2-Good, 3-High] : ";
		cin >> blockSize;
		cout << "\n Enter Output File Name : ";
		cin >> outputFile;

		qualityPrm = 10 - qualityPrm;
		blockSize = 4 - blockSize;
		blockSize = 8 + (blockSize - 1) * 4;

		Mat3b originalImage = imread(inputFile, CV_LOAD_IMAGE_COLOR);
	
		CharithaImageCodec encodeImage;
		encodeImage.encode(originalImage, qualityPrm, blockSize, outputFile);

		waitKey();
		cin.get();

	}
	else if (selectMode == 2) {
		
		string filename, opFname;
		int nFrames;
		cout << "\n Video Mode Selected... \n";
		cout << "\n Enter Video Input File Name : ";
		cin >> filename;
		cout << "\n Enter Quality [0 to 10 Scale , 0 - Low Quality , 10 - High Quality] : ";
		cin >> qualityPrm;
		cout << "\n Enter the Compression Level [ 1-Low, 2-Good, 3-High] : ";
		cin >> blockSize;
		cout << "\n Enter the Number of Frames Required : ";
		cin >> nFrames;
		cout << "\n Enter the Output File Name(include name.avi) : ";
		cin >> opFname;

		qualityPrm = 10 - qualityPrm;
		qualityPrm = pow(2, qualityPrm);
		blockSize = 4 - blockSize;
		blockSize = 8 + (blockSize - 1) * 4;

		videoEncoder myencoder;
		VideoCapture cap;
		myencoder.videoEncode(filename, qualityPrm, blockSize, nFrames, opFname);

		waitKey();
		cin.get();

	}

	waitKey();
	cin.get();

	return 0;
}
