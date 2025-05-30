#include <cmath>
#include <iostream>
#include <iostream>
#include <fstream>          // for ifstream
#include <cctype>           // for tolower(), isalpha()
#include <string>
#include <vector>
#include <seal/seal.h>
using namespace std;
using namespace seal;

#ifndef _MultinomialNB_Email_H
#define _MultinomialNB_Email_H

struct fileNameNode
{
	string filename;
	fileNameNode * next;
	fileNameNode(string s, fileNameNode * ptr)
	{
		filename = s;
		next = ptr;
	}
	fileNameNode()
	{
		filename = "";
		next = nullptr;
	}
};

class MultinomialNB_Email
{
	public:
		MultinomialNB_Email();
		MultinomialNB_Email(string path, int mValue, long long int &timeElapsed);
		void train(long long int &timeElapsed);
		void getSelectedFeaturesParameters(vector<string> &selectedFeaturesOutput,
			vector<long double>& logOfProbWordIsHamOutput,
			vector<long double>& logOfProbWordIsSpamOutput,
			long double &logProbHamOutput, long double & logProbSpamOutput);
		void getSelectedFeatures(vector<string> &selectedFeaturesOutput);
		void getTrainingVector(vector<long double> &trainingVector, int polyModulus);
		void getConfusionMatrix(vector<vector<int>> &confusionMatrixOutput);
		vector<long double> secureSum(vector<long double> &multiplied_query);
		void createSingleQuery(string filename);
        bool queryPlain(string filename);
        void classifyPlain(string path);
		void saveModel();
		void loadModel();
		void saveSelectedFeatures();
		void loadSelectedFeatures();
		vector<int64_t> getTVHamProbs();
		vector<int64_t> getTVSpamProbs();
		Ciphertext evaluateEncryptedQuery(Ciphertext &encrypted_query, Evaluator &evaluator, BatchEncoder &encoder, GaloisKeys &galois_keys, RelinKeys &relin_keys);
		Ciphertext encryptedSecureSum(Ciphertext &encrypted_query, Evaluator &evaluator, BatchEncoder &encoder, GaloisKeys &galois_keys, RelinKeys &relin_keys);

	
	private:
		int m, hamMails = 0, spamMails = 0;
		vector<string> wordsGlobalVector;
		vector<string> selectedFeatures;

		vector<long long int> spamWordsInDocumentsCount;
		vector<long long int> hamWordsInDocumentsCount;

		vector<long long int> spamWordsFreq;
		vector<long long int> hamWordsFreq;

		long double logProbSpam, logProbHam;
		vector<long double> logOfProbWordIsHam, logOfProbWordIsSpam;		

		vector<long double> finalModel;
		string modelPath;
		string selectedFeaturesPath;
		vector<vector<int>> confusionMatrix{ {0,0},{0,0} };
};

#endif