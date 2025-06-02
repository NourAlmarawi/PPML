#include <time.h>
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
#include "MultinomialNB_Email.cpp"
#include "Client.cpp"
namespace fs = std::experimental;
using std::vector;
using namespace std;
using namespace seal;


int main_client()
{
	// int polyModulus = 16384;
	int polyModulus = 4096;
	int m = 16;

	cout << "Starting Client..." << endl;
	Client client = Client();
	client.initializeSEAL(polyModulus);

	cout << "Starting MultinomialNB_Email..." << endl;
	MultinomialNB_Email MNBE = MultinomialNB_Email();

	cout << "getting selected features vector..." << endl;
	MNBE.loadSelectedFeatures();
	vector<string> selectedFeatures;
	MNBE.getSelectedFeatures(selectedFeatures);

	cout << "loading model..." << endl;
	MNBE.loadTrainingVectors();
	cout << "MultinomialNB_Email loaded successfully." << endl;

	cout << "creating query according to selected features..." << endl;
	string filename = "test_data/testSpam.txt";
	// string filename = "test_data/testHam.txt";
	
	Ciphertext query = client.createQueryAccordingToSelectedFeauters(polyModulus, filename, selectedFeatures);
	cout << "Query created successfully." << endl;
	
	cout << "Doing homomorphic operations now.." << endl;
	Ciphertext processedQuery = MNBE.evaluateEncryptedQuery(query, *client.evaluator, *client.batch_encoder, client.gal_keys);

	cout << "Query processed successfully." << endl;
	cout << "Decrypting result..." << endl;
	client.decryptResult(processedQuery);

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
	int polyModulus = 1024;
	long long int timeElapsed = 0;

	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed);
	MNB_Email.train(timeElapsed);
	
	// vector<long double> trainingVector(polyModulus, 0.0);
	// MNB_Email.getTrainingVector(trainingVector, polyModulus);
	// vector<string> selectedFeatures;
	// MNB_Email.getSelectedFeatures(selectedFeatures);
	// MNB_Email.saveModel();
	
	MNB_Email.saveTrainingVectors(polyModulus);
	MNB_Email.saveSelectedFeatures();

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

int main_plain_classification()
{
	// string path = "enron5";
	string path = "dataset_reduced";
	// string path = "test_data";
	// int m = 2047;
	int m = 16;
	// int polyModulus = 16384;
	int polyModulus = 4096;
	// int polyModulus = 1000;
	long long int timeElapsed = 0;

	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed);
	MNB_Email.train(timeElapsed);
	
	vector<string> selectedFeatures;
	MNB_Email.getSelectedFeatures(selectedFeatures);

	MNB_Email.saveSelectedFeatures();
	MNB_Email.saveTrainingVectors(polyModulus);

	cout << endl << "CLASSIFICATION ON ENRON5" << endl;
	MNB_Email.classifyPlain(path);

	vector<vector<int>> confusionMatrix;
	MNB_Email.getConfusionMatrix(confusionMatrix);
	
	cout << "\nConfusion Matrix:\n";
	cout << "                Predicted HAM   Predicted SPAM\n";
	cout << "Actual HAM   " << setw(10) << confusionMatrix[0][0] << setw(18) << confusionMatrix[0][1] << endl;
	cout << "Actual SPAM  " << setw(10) << confusionMatrix[1][0] << setw(18) << confusionMatrix[1][1] << endl;


	vector<int> testSecureSum = {1,2,3,4,5,6,7,8};
	testSecureSum = MNB_Email.secureSum(testSecureSum);
	cout << "Secure sum of testSecureSum: ";
	for (const auto &val : testSecureSum) {
		cout << val << " ";
	}

	return 0;
}

int main()
{
	while (true)
	{
		int choice;
		cout << "Choose an option:\n";
		cout << "1. Run Client\n";
		cout << "2. Train MNBE and save\n";
		cout << "3. Run Plain Classification\n";
		cout << "0. Exit\n";
		cin >> choice;

		switch (choice)
		{
		case 1:
			main_client();
			break;
		case 2:
			main_train();
			break;
		case 3:
			main_plain_classification();
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