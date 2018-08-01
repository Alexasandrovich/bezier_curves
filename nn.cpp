/*
Copyright (c) 2013, Taiga Nomi and the respective contributors
All rights reserved.

Use of this source code is governed by a BSD-style license that can be found
in the LICENSE file.
*/
#include <iostream>
#include "tiny_dnn/tiny_dnn.h"
#include <windows.h>
#include <fstream>
#include <filesystem>

using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace std;

// rescale output to 0-100
template <typename Activation>
double rescale(double x) {
	Activation a;
	return 100.0 * (x - a.scale().first) / (a.scale().second - a.scale().first);
}

void convert_image(const std::string& imagefilename,
	double minv,
	double maxv,
	int w,
	int h,
	vec_t& data) {

	image<> img(imagefilename, image_type::grayscale);
	image<> resized = resize_image(img, w, h);

	// mnist dataset is "white on black", so negate required
	std::transform(resized.begin(), resized.end(), std::back_inserter(data),
		[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
}

void recognize(const std::string& dictionary, const std::string& filename) {
	network<sequential> nn;

	nn.load(dictionary);

	// convert imagefile to vec_t
	vec_t data;
	convert_image(filename, -1.0, 1.0, 32, 32, data);

	// recognize
	auto res = nn.predict(data);
	vector<pair<double, int> > scores;

	// sort & print top-3
	for (int i = 0; i < 10; i++)
		scores.emplace_back(rescale<tanh>(res[i]), i);

	sort(scores.begin(), scores.end(), greater<pair<double, int>>());

	for (int i = 0; i < 3; i++)
		cout << scores[i].second << "," << scores[i].first << endl;

	// save outputs of each layer
	for (size_t i = 0; i < nn.depth(); i++) {
		auto out_img = nn[i]->output_to_image();
		auto filename = "layer_" + std::to_string(i) + ".png";
		out_img.save(filename);
	}
	// save filter shape of first convolutional layer
	{
		auto weight = nn.at<convolutional_layer>(0).weight_to_image();
		auto filename = "weights.png";
		weight.save(filename);
	}
}
std::ofstream file2("result.txt");

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "please specify image file" << std::endl;
		return 0;
	}
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	hf = FindFirstFile("E://Course/course_work/final_images*.jpeg", &FindFileData);
	if (hf != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string YourPath;
			YourPath = FindFileData.cFileName;
			file2 << YourPath << std::endl;
			recognize("LeNet-model", "E://Course/course_work/final_images" + YourPath);
			file2 << std::endl;
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
	}
}