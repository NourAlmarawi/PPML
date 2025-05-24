#include <time.h>
#include "MultinomialNB_Email.h"
#include <cmath>
#include <iostream>
#include <iomanip>
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

int main()
{
	// string path = "enron5";
	string path = "dataset_reduced";
	// string path = "test_data";
	// int m = 2047;
	int m = 16;
	// int polyModulus = 16384;
	// int polyModulus = 4096;
	int polyModulus = 1000;
	long long int timeElapsed = 0;

	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed);
	MNB_Email.train(timeElapsed);
	
	vector<long double> trainingVector(polyModulus, 0.0);
	MNB_Email.getTrainingVector(trainingVector, polyModulus);

	MNB_Email.saveModel();

	vector<long double> restoredModel(polyModulus, 0.0);
	MNB_Email.loadModel(restoredModel);

	// create single query
	string filename = "test_data/test_file.txt";
	MNB_Email.createSingleQuery(filename);

	// create query for all dataset
	cout << endl << "CLASSIFICATION ON ENRON5" << endl;

	MNB_Email.classify(path);

	vector<vector<int>> confusionMatrix;
	MNB_Email.getConfusionMatrix(confusionMatrix);
	
	cout << "\nConfusion Matrix:\n";
	cout << "                Predicted HAM   Predicted SPAM\n";
	cout << "Actual HAM   " << setw(10) << confusionMatrix[0][0] << setw(18) << confusionMatrix[0][1] << endl;
	cout << "Actual SPAM  " << setw(10) << confusionMatrix[1][0] << setw(18) << confusionMatrix[1][1] << endl;

	return 0;
}