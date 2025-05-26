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
#include "seal/seal.h"
#include "Client.cpp"
namespace fs = std::experimental;
using std::vector;
using namespace std;
using namespace seal;

int main1()
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
	vector<string> selectedFeatures;
	MNB_Email.getSelectedFeatures(selectedFeatures);

	MNB_Email.saveSelectedFeatures();
	MNB_Email.saveModel();

	MNB_Email.loadModel();

	vector<string> restoredSelectedFeatures;
	MNB_Email.loadSelectedFeatures();

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

int main_client()
{
	int polyModulus = 4096;

	cout << "Starting Client..." << endl;
	Client client = Client();
	client.initializeSEAL(polyModulus);

	cout << "Starting MultinomialNB_Email..." << endl;
	MultinomialNB_Email MNBE = MultinomialNB_Email();

	cout << "getting selected features vector..." << endl;
	MNBE.loadSelectedFeatures();

	cout << "loading model..." << endl;
	MNBE.loadModel();
	cout << "MultinomialNB_Email loaded successfully." << endl;

	cout << "creating query according to selected features..." << endl;
	string filename = "test_data/testSpam.txt";
	cout <<  MNBE.Query(filename) << endl;

	//TODO : create 


	cout << endl << endl << endl;
	return 0;
}

int main_train(){
	// string path = "enron5";
	string path = "dataset_reduced";
	// string path = "test_data";
	// int m = 2047;
	int m = 16;
	// int polyModulus = 16384;
	int polyModulus = 4096;
	long long int timeElapsed = 0;

	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed);
	MNB_Email.train(timeElapsed);
	
	vector<long double> trainingVector(polyModulus, 0.0);
	MNB_Email.getTrainingVector(trainingVector, polyModulus);
	vector<string> selectedFeatures;
	MNB_Email.getSelectedFeatures(selectedFeatures);

	MNB_Email.saveSelectedFeatures();
	MNB_Email.saveModel();

	//? debug
	// vector<string> restoredSelectedFeatures;
	// MNB_Email.loadSelectedFeatures(restoredSelectedFeatures);
	// // compare selected features
	// cout << "Restored Selected Features size:"<< restoredSelectedFeatures.size()  << endl;
	// for (size_t i = 0; i < restoredSelectedFeatures.size(); ++i) {
	// 	cout << i << ":[" << restoredSelectedFeatures[i] << "]" << endl;
	// }
	// cout << "Original Selected Features size:"<< selectedFeatures.size() << endl;
	// for (size_t i = 0; i < selectedFeatures.size(); ++i) {
	// 	cout << i << ":[" << selectedFeatures[i] << "]" << endl;
	// }

	return 0;
}

int main()
{
	while (true)
	{
		int choice;
		cout << "Choose an option:\n";
		cout << "1. Run Full code MultinomialNB_Email\n";
		cout << "2. Run Client\n";
		cout << "3. Train MNBE and save\n";
		cout << "0. Exit\n";
		cin >> choice;

		switch (choice)
		{
		case 1:
			main1();
			break;
		case 2:
			main_client();
			break;
		case 3:
			main_train();
			break;
		case 0:
			return 0;
		default:
			cout << "Invalid choice. Try again." << endl;
			break;
		}
		cout << endl;
	}
}