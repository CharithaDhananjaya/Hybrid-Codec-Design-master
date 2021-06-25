#include "MalindaHuffman.h"

map<string, vector<bool>> coolCodec::Huffman_dict(Mat image, string filename)
{
	huff_tree temptree;

	Mat img = image.clone();
	double min, max, min1, max1;

	double * prob = (double *)calloc(image.rows * image.cols, sizeof(double));
	double * ptr;

	// Reshape image into an array
	img = img.reshape(1, img.rows * img.cols);
	minMaxLoc(img, &min1, &max1);
	// Shift symbols above negative
	for (size_t i = 0; i < img.size().height; i++)
	{
		img.at<int16_t>(i, 0) = img.at<int16_t>(i, 0) - (int16_t)min1;
	}

	minMaxLoc(img, &min, &max);
	ptr = prob;


	// Determining the occurances of each symbol
	for (size_t i = 0; i < img.rows; i++)
	{
		int16_t val = img.at<int16_t>(i, 0);
		if (val <= max && val >= min) {
			ptr = prob + val;
			*ptr = *ptr + 1;
		}
	}

	int * symbol = (int *)calloc(img.rows, sizeof(int));
	int * sptr = symbol;
	int * probability = (int *)calloc(img.rows, sizeof(int));
	int * pptr = probability;

	ptr = prob;
	int sz = 1;
	for (size_t i = 0; i < img.rows; i++)
	{
		if (*ptr != 0) {
			*pptr = *ptr;
			*sptr = i + min1;
			pptr++; sptr++;
			sz++;
		}
		ptr++;
	}

	pptr = probability;
	sptr = symbol;
	for (int i = 0; i < sz; i++) {
		temptree.symbol = to_string(*sptr);
		temptree.probability = *pptr;//probability[i];
		temptree.code = 0;
		tree.push_back(temptree);
		ptr++;
		sptr++;
	}


	tree = h_sort(tree);


	int i = 0;
	while (tree.size() > 1) {
		tree = h_sort(tree);

		huff_tree newNode;
		tree[0].code = 1;
		tree[1].code = 0;

		newNode.probability = tree[0].probability + tree[1].probability;
		newNode.symbol = to_string(i) + "(" + tree[0].symbol + "," + tree[1].symbol + ")";

		newNode.child.push_back(tree[0]);
		newNode.child.push_back(tree[1]);

		tree.erase(tree.begin(), tree.begin() + 2);
		tree.push_back(newNode);

		tree = h_sort(tree);
		i = i + 1;
	}


	code_book cb;
	cb = get_code(cb, tree);

	ofstream file(filename, ios::out | ios::app);
	stringstream outputs;
	file.seekp(this->Write_marker);
	for (map<string, vector<bool>>::iterator it = cb.code.begin(); it != cb.code.end(); it++) {

		file << it->first << ' ';

		for (vector<bool>::iterator inner = it->second.begin(); inner != it->second.end(); inner++) {	
			file << *inner;

		}

		file << ' ';

	}

	file << 's' << 'e' << 'c' << 't' << 'n' << ' ';
	this->Write_marker = file.tellp();
	file.close();
	tree.clear();
	return cb.code;
}

vector<huff_tree> coolCodec::h_sort(vector<huff_tree>  htree)
{
	huff_tree temp;
	for (int i = 1; i < htree.size(); i++) {
		for (int j = 1; j < htree.size(); j++) {
			if (htree[i].probability < htree[j].probability) {
				temp = htree[i];
				htree[i] = htree[j];
				htree[j] = temp;
			}
		}
	}
	return  htree;
}

code_book coolCodec::get_code(code_book book, vector<huff_tree> tree)
{
	for (int j = 0; j < tree.size(); j++) {
		if (tree[j].child.size() == 0) {
			book.acc.push_back(tree[j].code);

			code_book final_acc;
			for (int i = 0; i < (book.acc.size() - 1); i++) {
				final_acc.acc.push_back(book.acc[i + 1]);
			}

			book.code.insert(make_pair(tree[j].symbol, final_acc.acc));
			book.acc.pop_back();
		}
		else {
			book.acc.push_back(tree[j].code);
			book = get_code(book, tree[j].child);
			book.acc.pop_back();
		}

	}

	return book;
}

void coolCodec::HuffmanEncode(map<string, vector<bool>> dictionary, Mat image, string filename)
{
	int cnt = 0;
	uint8_t outbuff = 0;
	int pixcnt = 0;
	bool flag = true; // True if byte of data is complete
	ofstream file(filename, ios::out | ios::app);
	file.seekp(this->Write_marker);

	for (size_t i = 0; i < image.rows; i++)
	{
		for (size_t j = 0; j < image.cols; j++)
		{
			// Get the (i,j)th Pixel value
			stringstream ss;
			ss << (image.at<int16_t>(i, j));
			string s = ss.str();
			// Get the relavent Huffman code from the Table
			vector<bool> temp = dictionary.find(s)->second;
			pixcnt++; // Pixel count reference

					  // clear output buffer if Byte is filled
			if (flag == true) { outbuff = 0; flag == false; }
			else { flag = true; }

			for (vector<bool>::iterator it = temp.begin(); it != temp.end(); it++) {
				if (*it == true) {
					// push 1 into outbuffer
					outbuff <<= 1;
					outbuff |= 0x01;
					cnt++;
				}
				else {
					//push 0 into outbuffer
					outbuff <<= 1;
					cnt++;
				}
				if (cnt == 8) {
					// Save buffer after filling 8bits
					cnt = 0;  // pushed bit connt
					file.write((char*)&outbuff, sizeof(uint8_t));
					outbuff = 0; // Clear output buffer
					flag = true; // Done filling a Byte to output buffer
				}
				else { flag = false; } // Write buffer is still not filled
			}
		}
	}

	if (flag == false) {
		outbuff <<= 8 - cnt;
		file.write((char*)&outbuff, sizeof(uint8_t));

	}

	outbuff = ' ';
	file.write((char*)&outbuff, sizeof(uint8_t));
	this->Write_marker = file.tellp();
	cout << "Write marker " << this->Write_marker;
	cout << " Pixnt " << pixcnt << endl;
	file.close();
}
