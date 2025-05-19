#include <time.h>
#include "MultinomialNB_Email.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <chrono>
#include <experimental/filesystem>
#include <seal/seal.h>
namespace fs = std::experimental;
using std::vector;
using namespace std;
using namespace seal;

vector<int64_t> secureSum(vector<int64_t> &TVGlobal)
{
	int m = TVGlobal.size();
	vector<int64_t> temp1 = TVGlobal;
	vector<int64_t> temp2 = TVGlobal;
	for (int i = 0; i < (m / 2) - 1; ++i)
	{
		temp1 = temp2;
		// TODO: replace rotation and addition with homomorphic rotation and addition
		rotate(temp2.begin(), temp2.begin() + pow(2, i), temp2.end());
		for (size_t idx = 0; idx < temp2.size(); ++idx)
			temp2[idx] += temp1[idx];
	}
	return temp2;
}

int main()
{
	// string path = "enron5";
	string path = "dataset_reduced";
	// int m = 2047;
	int m = 10;
	int polyModulus = m;
	long long int timeElapsed = 0;

	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed), tmpmNEmail;
	tmpmNEmail = MNB_Email;
	tmpmNEmail.train(timeElapsed);
	tmpmNEmail.classify(path, timeElapsed);
	tmpmNEmail.printConfionMat();

	int64_t K2 = 8;
	vector<string> selectedFeaturesOutput;
	vector<int64_t> TVGlobal_v(2 * selectedFeaturesOutput.size() + 2, 0);
	vector<int64_t> trainedModel_v(polyModulus, 0);

	tmpmNEmail.getSelectedFeaturesOnly(selectedFeaturesOutput);
	tmpmNEmail.getTVaccordingToSelectedFeatures(selectedFeaturesOutput, TVGlobal_v);
	cout << "Selected Features Output:" << endl;
	for (const auto &feature : selectedFeaturesOutput) cout << feature << " "; cout << endl;
	cout << "TVGlobal_v:" << endl;
	for (const auto &feature : TVGlobal_v) cout << feature << " "; cout << endl;

	return 0;
}