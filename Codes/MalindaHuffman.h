#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <fstream>
#include <stdint.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

struct huff_tree {
	string  symbol;
	double  probability;
	vector<huff_tree> child;
	bool  code;
};

struct code_book {
	int idx;
	map<string, vector<bool>> code;
	vector<bool> acc;
};


class coolCodec {


public:
	map<string, vector<bool>> Huffman_dict(Mat image, string filename);
	vector<huff_tree> h_sort(vector<huff_tree> tree);
	code_book get_code(code_book book, vector<huff_tree> tree);
	void HuffmanEncode(map<string, vector<bool>> dictionary, Mat image, string filename);

private:
	int block_size;
	vector<huff_tree> tree;
	map<string, int> dict[3];
	streampos Read_marker;
	streampos Write_marker;
	

};


