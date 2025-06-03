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
	int polyModulus = 16384;
	int m = 2047;

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
	string filename = "test_data/ham/test.txt";
	// string filename = "test_data/spam/test.txt";
	
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

int main_plain_train(){
	string path = "enron5";
	int m = 2047;
	int polyModulus = 16384;
	long long int timeElapsed = 0;
	
	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed);
	MNB_Email.train(timeElapsed);
	MNB_Email.saveSelectedFeatures();
	MNB_Email.saveTrainingVectors(polyModulus);
	return 0;
}

int main_plain_classification()
{
	string path = "enron5";
	int m = 2047;
	int polyModulus = 16384;
	long long int timeElapsed = 0;

	// train & save
	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed);
	MNB_Email.loadSelectedFeatures();
	MNB_Email.loadTrainingVectors();
	// classify plain
	vector<string> selectedFeatures;
	MNB_Email.getSelectedFeatures(selectedFeatures);

	cout << endl << "CLASSIFICATION ON ENRON5" << endl;
	MNB_Email.classifyPlain(path);

	vector<vector<int>> confusionMatrix;
	MNB_Email.getConfusionMatrix(confusionMatrix);
	cout << "\nConfusion Matrix:\n";
	cout << "                Predicted HAM   Predicted SPAM\n";
	cout << "Actual HAM   " << setw(10) << confusionMatrix[0][0] << setw(18) << confusionMatrix[0][1] << endl;
	cout << "Actual SPAM  " << setw(10) << confusionMatrix[1][0] << setw(18) << confusionMatrix[1][1] << endl;
	return 0;
}

int main()
{
	while (true)
	{
		int choice;
		cout << "Update final" << endl;
		cout << "Choose an option:\n";
		cout << "1. Run Client\n";
		cout << "2. Run Training and save\n";
		cout << "3. Run Plain Classification\n";
		cout << "0. Exit\n";
		cin >> choice;

		switch (choice)
		{
		case 1:
			main_client();
			break;
		case 2:
			main_plain_train();
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