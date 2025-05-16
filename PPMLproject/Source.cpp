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
namespace fs = std::experimental;
using std::vector;
using namespace std;


int main()
{
	// initializing
	// string path = "enron5";
	string path = "dataset_reduced";
	// int m = 2047;
	int m = 100;
	long long int timeElapsed = 0;
	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed), tmpmNEmail;
	tmpmNEmail = MNB_Email;

	// debugging: print vectors
	// vector<string> wordsGlobalVector;
	// vector<long long int> hamWordsInDocumentsCount, spamWordsInDocumentsCount;
	// int hamMails, spamMails;
	// tmpmNEmail.getPrivateData(wordsGlobalVector, hamWordsInDocumentsCount, spamWordsInDocumentsCount, hamMails, spamMails);
	// cout << "wordsGlobalVector: " << endl;
	// for (const auto& word : wordsGlobalVector) cout << word << " ";
	// cout << endl << "hamWordsInDocumentsCount: " << endl;
	// for (const auto& count : hamWordsInDocumentsCount) cout << count << " ";
	// cout << endl << "spamWordsInDocumentsCount: " << endl;
	// for (const auto& count : spamWordsInDocumentsCount) cout << count << " ";
	// cout << endl << "hamMails: " << hamMails << endl;
	// cout << "spamMails: " << spamMails << endl;

	// training
	tmpmNEmail.train(timeElapsed);
	tmpmNEmail.classify(path, timeElapsed);
	tmpmNEmail.printConfionMat();
	return 0;
}