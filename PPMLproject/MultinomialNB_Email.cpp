#include <time.h> // for time()
#include <cmath>
#include <fstream> // for ifstream
#include <cctype>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <experimental/filesystem>
#include <numeric>
#include <algorithm>
#include "seal/seal.h"
namespace fs = std::experimental;
using std::vector;
using namespace std;
using namespace seal;
class MultinomialNB_Email
{
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

	vector<int64_t> TVHamProbs, TVSpamProbs;

	vector<vector<int>> confusionMatrix{{0, 0}, {0, 0}};

public:
	bool debug = false;
	
	MultinomialNB_Email(bool debugMode)
	{
		spamMails = 0;
		hamMails = 0;
		wordsGlobalVector.push_back("");
		spamWordsInDocumentsCount.push_back(0);
		hamWordsInDocumentsCount.push_back(0);
		spamWordsFreq.push_back(0);
		hamWordsFreq.push_back(0);
		debug = debugMode;
	}
	
	void preprocessWord(string &word)
	{
		string cleaned;
		for (char c : word) {
			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
				cleaned += (c >= 'A' && c <= 'Z') ? (c + 32) : c;
			}
		}
		word = cleaned;
	}

	MultinomialNB_Email(string path, int mValue, long long int &timeElapsed)
	{
		m = mValue;
		wordsGlobalVector.push_back("");
		spamWordsInDocumentsCount.push_back(0);
		hamWordsInDocumentsCount.push_back(0);
		spamWordsFreq.push_back(0);
		hamWordsFreq.push_back(0);

		chrono::high_resolution_clock::time_point time_start, time_end;
		time_start = chrono::high_resolution_clock::now();
		time_end = chrono::high_resolution_clock::now();
		auto time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
		cout << "Initializing now.." << endl;

		// reading ham
		string hamPath = path + "\\ham", filename, ANSIWord;
		cout << "Reading ham files: " << endl;
		time_start = chrono::high_resolution_clock::now();
		int hamFileCount = 0;
		const int totalHamFiles = std::distance(fs::filesystem::recursive_directory_iterator(path + "\\ham"), fs::filesystem::recursive_directory_iterator());
		for (const auto &entry : fs::filesystem::recursive_directory_iterator(hamPath))
		{
			filename = entry.path().string();
			// cout <<"opening file: "<< filename << endl;

			// Progress bar
			hamFileCount++;
			int progress = (hamFileCount * 100) / totalHamFiles;
			cout << "\r[";
			int barWidth = 50;
			int pos = (progress * barWidth) / 100;
			for (int i = 0; i < barWidth; ++i)
			{
				if (i < pos)
					cout << "=";
				else if (i == pos)
					cout << ">";
				else
					cout << " ";
			}
			cout << "] " << progress << "% (" << hamFileCount << "/" << totalHamFiles << ")" << flush;
			if (hamFileCount == totalHamFiles)
				cout << endl;

			ifstream inputFile(filename);
			vector<string> tmpLocalWords;
			vector<long long int> tmpLocalWordsInDoc;
			vector<long long int> tmpLocalWordsFreq;
			bool existsInList = false;

			while (inputFile >> ANSIWord)
			{
				try{
					preprocessWord(ANSIWord);
					if (ANSIWord.empty()) continue;
				}
				catch (const std::exception &e)
				{
					cerr << "Error processing word: " << ANSIWord << " in file: " << filename << ". Error: " << e.what() << endl;
					// continue; // Skip this word and continue with the next
				}

				existsInList = false;
				for (int i = 0; i < tmpLocalWords.size(); i++)
				{
					if (ANSIWord == tmpLocalWords[i])
					{
						existsInList = true;
						// tmpLocalWordsInDoc[i] = 1;
						tmpLocalWordsFreq[i] += 1;
						break;
					}
				}
				if (!existsInList)
				{
					tmpLocalWords.push_back(ANSIWord);
					tmpLocalWordsInDoc.push_back(1);
					tmpLocalWordsFreq.push_back(1);
				}
			}
			existsInList = false;
			for (int i = 0; i < tmpLocalWords.size(); i++)
			{
				existsInList = false;
				for (int j = 0; j < wordsGlobalVector.size(); j++)
				{
					if (tmpLocalWords[i] == wordsGlobalVector[j])
					{
						existsInList = true;
						hamWordsInDocumentsCount[j]++;
						hamWordsFreq[j] += tmpLocalWordsFreq[i];
						break;
					}
				}
				if (!existsInList)
				{
					wordsGlobalVector.push_back(tmpLocalWords[i]);
					hamWordsInDocumentsCount.push_back(1);
					hamWordsFreq.push_back(tmpLocalWordsFreq[i]);

					spamWordsInDocumentsCount.push_back(0);
					spamWordsFreq.push_back(0);
				}
			}
			hamMails++;
		}

		// reading spam
		cout << "Reading spam files: " << endl;
		int spamFileCount = 0;
		const int totalSpamFiles = std::distance(fs::filesystem::recursive_directory_iterator(path + "\\spam"), fs::filesystem::recursive_directory_iterator());
		string spamPath = path + "\\spam";
		for (const auto &entry : fs::filesystem::recursive_directory_iterator(spamPath))
		{
			filename = entry.path().string();
			// cout <<"opening file: "<< filename << endl;

			// Progress bar
			spamFileCount++;
			int progress = (spamFileCount * 100) / totalSpamFiles;
			cout << "\r[";
			int barWidth = 50;
			int pos = (progress * barWidth) / 100;
			for (int i = 0; i < barWidth; ++i)
			{
				if (i < pos)
					cout << "=";
				else if (i == pos)
					cout << ">";
				else
					cout << " ";
			}
			cout << "] " << progress << "% (" << spamFileCount << "/" << totalSpamFiles << ")" << flush;
			if (spamFileCount == totalSpamFiles)
				cout << endl;

			ifstream inputFile(filename);
			vector<string> tmpLocalWords;
			vector<long long int> tmpLocalWordsInDoc;
			vector<long long int> tmpLocalWordsFreq;
			bool existsInList = false;

			while (inputFile >> ANSIWord)
			{
				try{
					preprocessWord(ANSIWord);
					if (ANSIWord.empty()) continue;
				}
				catch (const std::exception &e)
				{
					cerr << "Error processing word: " << ANSIWord << " in file: " << filename << ". Error: " << e.what() << endl;
					// continue; // Skip this word and continue with the next
				}

				existsInList = false;
				for (int i = 0; i < tmpLocalWords.size(); i++)
				{
					if (ANSIWord == tmpLocalWords[i])
					{
						existsInList = true;
						// tmpLocalWordsInDoc[i] = 1;
						tmpLocalWordsFreq[i] += 1;
						break;
					}
				}
				if (!existsInList)
				{
					tmpLocalWords.push_back(ANSIWord);
					tmpLocalWordsInDoc.push_back(1);
					tmpLocalWordsFreq.push_back(1);
				}
			}
			existsInList = false;
			for (int i = 0; i < tmpLocalWords.size(); i++)
			{
				existsInList = false;
				for (int j = 0; j < wordsGlobalVector.size(); j++)
				{
					if (tmpLocalWords[i] == wordsGlobalVector[j])
					{
						existsInList = true;
						spamWordsInDocumentsCount[j]++;
						spamWordsFreq[j] += tmpLocalWordsFreq[i];
						break;
					}
				}
				if (!existsInList)
				{
					wordsGlobalVector.push_back(tmpLocalWords[i]);
					spamWordsInDocumentsCount.push_back(1);
					spamWordsFreq.push_back(tmpLocalWordsFreq[i]);

					hamWordsInDocumentsCount.push_back(0);
					hamWordsFreq.push_back(0);
				}
			}
			spamMails++;
		}

		time_end = chrono::high_resolution_clock::now();
		time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
		timeElapsed = time_diff.count() / 1000;
		cout << "In total we have " << wordsGlobalVector.size() << " words in this dataset" << endl;
		cout << "The initialization was done in [" << time_diff.count() / 1000 << " ms]" << endl;

		// //?debug preprocessing
		// cout << "Words in global vector: " << endl;
		// for (int i = 0; i < wordsGlobalVector.size(); i++)
		// {
		// 	cout << wordsGlobalVector[i] << " ";
		// }
		// cout << endl;

	}
	
	void train(long long int &timeElapsed)
	{
		chrono::high_resolution_clock::time_point time_start, time_end;
		time_start = chrono::high_resolution_clock::now();
		time_end = chrono::high_resolution_clock::now();
		auto time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);

		vector<string> tmpSelectedFeatures(m + 1, "");
		selectedFeatures = tmpSelectedFeatures;
		vector<long long int> spamWordsNOTInDocumentsCount;
		spamWordsNOTInDocumentsCount.push_back(0);
		vector<long long int> hamWordsNOTInDocumentsCount;
		hamWordsNOTInDocumentsCount.push_back(0);
		vector<long double> tmpLogOfProbWordIsHam(m + 1, 0), tmpLogOfProbWordIsSpam(m + 1, 0);
		logOfProbWordIsHam = tmpLogOfProbWordIsHam;
		logOfProbWordIsSpam = tmpLogOfProbWordIsSpam;

		time_start = chrono::high_resolution_clock::now();
		for (int i = 1; i < wordsGlobalVector.size(); i++)
		{
			hamWordsNOTInDocumentsCount.push_back(hamMails - hamWordsInDocumentsCount[i]);
			spamWordsNOTInDocumentsCount.push_back(spamMails - spamWordsInDocumentsCount[i]);
		}

		int totalNrMails = spamMails + hamMails;
		long double probSpam = (long double)(spamMails + m) / (long double)(totalNrMails + 2 * m), probHam = 1 - probSpam;
		logProbHam = log(probHam);
		logProbSpam = log(probSpam);

		vector<long double> probWordInDoc(wordsGlobalVector.size(), 0),
			probWordNOTInDoc(wordsGlobalVector.size(), 0); // p(tk) and p(not(tk))
		vector<long double> probWordInHamDocs(wordsGlobalVector.size(), 0),
			probWordNOTInHamDocs(wordsGlobalVector.size(), 0),
			probWordInSpamDocs(wordsGlobalVector.size(), 0),
			probWordNOTInSpamDocs(wordsGlobalVector.size(), 0);
		vector<long double> informationGain(wordsGlobalVector.size(), -9999999999.9);

		// calculating information gain
		// ignoring words that appear in less than 5 documents
		// due to their low information gain
		int countOfLessThanFive = 0;
		for (int i = 0; i < wordsGlobalVector.size(); i++)
		{
			if ((hamWordsInDocumentsCount[i] + spamWordsInDocumentsCount[i]) >= 5)
			{
				probWordInDoc[i] = ((long double)hamWordsInDocumentsCount[i] + (long double)spamWordsInDocumentsCount[i]);
				probWordNOTInDoc[i] = (long double)(totalNrMails - probWordInDoc[i]);

				probWordInHamDocs[i] = (long double)(hamWordsInDocumentsCount[i] + 1);
				probWordNOTInHamDocs[i] = (long double)(hamWordsNOTInDocumentsCount[i] + 1);
				probWordInSpamDocs[i] = (long double)(spamWordsInDocumentsCount[i] + 1);
				probWordNOTInSpamDocs[i] = (long double)(spamWordsNOTInDocumentsCount[i] + 1);

				const long double smoothing = 1e-10; // Small constant to avoid log(0)
				informationGain[i] = probWordInHamDocs[i] * log((probWordInHamDocs[i] + smoothing) / ((probWordInDoc[i] * probHam) + smoothing)) + probWordNOTInHamDocs[i] * log((probWordNOTInHamDocs[i] + smoothing) / ((probWordNOTInDoc[i] * probHam) + smoothing)) + probWordInSpamDocs[i] * log((probWordInSpamDocs[i] + smoothing) / ((probWordInDoc[i] * probSpam) + smoothing)) + probWordNOTInSpamDocs[i] * log((probWordNOTInSpamDocs[i] + smoothing) / ((probWordNOTInDoc[i] * probSpam) + smoothing));
			}
			else
			{
				countOfLessThanFive++;
			}
		}
		cout << "words ignored = countOfLessThanFive=" << countOfLessThanFive << endl;

		// sorting the words based on information gain
		string tmpString;
		long long int tmpInt;
		long double tmpDouble;
		for (int i = 1; i < wordsGlobalVector.size() - 1; i++)
		{
			for (int j = i; j < wordsGlobalVector.size() - 1; j++)
			{
				if (informationGain[i] < informationGain[j])
				{
					// swap all the stuff
					// wordsGlobalVector
					tmpString = wordsGlobalVector[i];
					wordsGlobalVector[i] = wordsGlobalVector[j];
					wordsGlobalVector[j] = tmpString;

					// spamWordsInDocumentsCount
					tmpDouble = spamWordsInDocumentsCount[i];
					spamWordsInDocumentsCount[i] = spamWordsInDocumentsCount[j];
					spamWordsInDocumentsCount[j] = tmpDouble;

					// hamWordsInDocumentsCount
					tmpDouble = hamWordsInDocumentsCount[i];
					hamWordsInDocumentsCount[i] = hamWordsInDocumentsCount[j];
					hamWordsInDocumentsCount[j] = tmpDouble;

					// mutual information
					tmpDouble = informationGain[i];
					informationGain[i] = informationGain[j];
					informationGain[j] = tmpDouble;

					// spamWordsFreq
					tmpInt = spamWordsFreq[i];
					spamWordsFreq[i] = spamWordsFreq[j];
					spamWordsFreq[j] = tmpInt;

					// hamWordsFreq
					tmpInt = hamWordsFreq[i];
					hamWordsFreq[i] = hamWordsFreq[j];
					hamWordsFreq[j] = tmpInt;
				}
			}
		}

		if (wordsGlobalVector.size() < m)
			m = wordsGlobalVector.size();

		cout << "The features with the top " << m << " highest mutual information are:\n\n";
		cout << setw(15) << "Feature"
			 << setw(20) << "Info Gain"
			 << setw(25) << "Log(P(Ham|Word))"
			 << setw(25) << "Log(P(Spam|Word))"
			 << endl;
		for (int i = 1; i <= m; i++)
		{
			selectedFeatures[i] = wordsGlobalVector[i];
			const long double smoothing = 1e-10; // Small constant to avoid log(0)
			logOfProbWordIsHam[i] = log(((long double)hamWordsFreq[i] + 1 + smoothing) / (long double)(hamMails + m + smoothing));
			logOfProbWordIsSpam[i] = log(((long double)spamWordsFreq[i] + 1 + smoothing) / (long double)(spamMails + m + smoothing));

			cout << setw(15) << selectedFeatures[i]
				 << setw(20) << informationGain[i]
				 << setw(25) << logOfProbWordIsHam[i]
				 << setw(25) << logOfProbWordIsSpam[i]
				 << endl;
		}
		time_end = chrono::high_resolution_clock::now();
		time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
		timeElapsed = time_diff.count() / 1000;
		cout << "The real trainig of the selected features was done in " << timeElapsed << " ms" << endl;
	}
	
	void getSelectedFeatures(vector<string> &selectedFeaturesOutput)
	{
		selectedFeaturesOutput = selectedFeatures;
	}
	
	void saveTrainingVectors(int polyModulus){
		// create the vectors
		TVHamProbs.resize(polyModulus, 0);
		TVSpamProbs.resize(polyModulus, 0);
		int K = 8;
		TVHamProbs[0] = logProbHam * K;
		TVSpamProbs[0] = logProbSpam * K;
		for (int i = 1; i <= m ; i++)
		{
			TVHamProbs[i] = static_cast<int64_t>(logOfProbWordIsHam[i] * K);
			TVSpamProbs[i] = static_cast<int64_t>(logOfProbWordIsSpam[i] * K);
		}

		//save the vectors
		ofstream ham_probs_out("TVHamProbs.txt");
		if (ham_probs_out.is_open())
		{
			int K = 8;
			for (int i = 0; i < m && i < TVHamProbs.size(); ++i)
			{
				ham_probs_out << static_cast<int64_t>(TVHamProbs[i]) << "\n";
			}
			ham_probs_out.close();
			cout << "TVHamProbs saved to TVHamProbs.txt" << endl;
		}
		else
		{
			cerr << "Error: Unable to open TVHamProbs.txt for writing." << endl;
		}

		ofstream spam_probs_out("TVSpamProbs.txt");
		if (spam_probs_out.is_open())
		{
			int K = 8;
			for (int i = 0; i < m && i < TVSpamProbs.size(); ++i)
			{
				spam_probs_out << static_cast<int64_t>(TVSpamProbs[i]) << "\n";
			}
			spam_probs_out.close();
			cout << "TVSpamProbs saved to TVSpamProbs.txt" << endl;
		}
		else
		{
			cerr << "Error: Unable to open TVSpamProbs.txt for writing." << endl;
		}

	}

	void loadTrainingVectors(string TVHamProbsPath, string TVSpamProbsPath)
	{
		ifstream ham_probs_in(TVHamProbsPath);
		if (ham_probs_in.is_open())
		{
			TVHamProbs.clear();
			int64_t val;
			while (ham_probs_in >> val)
			{
				TVHamProbs.push_back(val);
			}
			ham_probs_in.close();
			if (debug) cout << "TVHamProbs loaded from TVHamProbs.txt" << endl;
		}
		else
		{
			cerr << "Error: Unable to open TVHamProbs.txt for reading." << endl;
		}

		ifstream spam_probs_in(TVSpamProbsPath);
		if (spam_probs_in.is_open())
		{
			TVSpamProbs.clear();
			int64_t val;
			while (spam_probs_in >> val)
			{
				TVSpamProbs.push_back(val);
			}
			spam_probs_in.close();
			if (debug) cout << "TVSpamProbs loaded from TVSpamProbs.txt" << endl;
		}
		else
		{
			cerr << "Error: Unable to open TVSpamProbs.txt for reading." << endl;
		}
		
	}

	void saveSelectedFeatures()
	{
		ofstream selected_features_out("selected_features.txt");
		if (selected_features_out.is_open())
		{
			selected_features_out << setprecision(10);
			for (const auto &val : selectedFeatures)
			{
				selected_features_out << val << "\n";
			}
			selected_features_out.close();
			cout << "Selected Features saved to selected_features.txt" << endl;
		}
		else
		{
			cerr << "Error: Unable to open file for saving the selected features." << endl;
		}
	}
	
	void loadSelectedFeatures(string features_path)
	{
		ifstream selected_features_input(features_path);
		if (selected_features_input.is_open())
		{
			selectedFeatures.clear();
			selectedFeatures.push_back("");
			string val;
			while (selected_features_input >> val)
			{
				selectedFeatures.push_back(val);
			}
			selected_features_input.close();
			if (debug) cout << "Selected Features loaded from selected_features.txt" << endl;
		}
		else
		{
			cerr << "Error: Unable to open file for loading the selected features." << endl;
		}
	}
	
	void getConfusionMatrix(vector<vector<int>> &confusionMatrixOutput)
	{
		confusionMatrixOutput = confusionMatrix;
	}
	
	vector<int> secureSum(vector<int> &multiplied_query)
	{
		vector<int> result = multiplied_query;
		int n = static_cast<int>(log2(result.size()));
		for (int i = 0; (1 << i) < result.size(); ++i)
		{
			vector<int> rotated = result;
			rotate(rotated.begin(), rotated.begin() + (1 << i), rotated.end());
			for (size_t idx = 0; idx < result.size(); ++idx)
				result[idx] += rotated[idx];
		}
		
		return result;
	}
	
	bool queryPlain(string filename) // returns true if classified as HAM, false if classified as SPAM
	{
		ifstream infile(filename);
		if (!infile.is_open())
		{
			cout << "Error opening file: " << filename << endl;
			return false;
		}

		vector<int> query(selectedFeatures.size(), 0);
		string word;
		while (infile >> word)
		{
			for (size_t i = 1; i < selectedFeatures.size(); ++i)
			{
				if (word == selectedFeatures[i])
				{
					query[i]++;
				}
			}
		}
		infile.close();

		query[0] = 1;
		vector<int> query_ham = query, query_spam = query;
		
		for (size_t i = 0; i < query.size() && i < TVHamProbs.size() && TVSpamProbs.size(); ++i){
			query_ham[i] = query_ham[i] * TVHamProbs[i];
			query_spam[i] = query_spam[i] * TVSpamProbs[i];
		}
			
		vector<int> sum_ham = secureSum(query_ham);
		vector<int> sum_spam = secureSum(query_spam);


		if (sum_ham[0] > sum_spam[0])
		{
			return true; // classified as HAM
		}
		else
		{
			return false; // classified as SPAM
		}
	}
	
	void classifyPlain(string path)
	{
		// Confusion matrix counters
		int true_ham = 0, false_spam = 0, true_spam = 0, false_ham = 0;
		vector<pair<string, int>> test_dirs = {{"ham", 0}, {"spam", 1}};
		for (const auto &dirinfo : test_dirs)
		{
			// progress bar
			int total_files = std::distance(fs::filesystem::directory_iterator(path + "/" + dirinfo.first), fs::filesystem::directory_iterator());
			int processed_files = 0;
			auto print_progress = [&](int processed) {
				int percent = static_cast<int>(100.0 * processed / total_files);
				cout << "\rProgress: [";
				int barWidth = 50;
				int pos = percent * barWidth / 100;
				for (int i = 0; i < barWidth; ++i)
					cout << (i < pos ? '=' : ' ');
				cout << "] " << percent << "% (" << processed << "/" << total_files << ")" << flush;
			};

			string dirpath = path + "/" + dirinfo.first;

			for (const auto &entry : fs::filesystem::directory_iterator(dirpath))
			{
				bool is_ham = queryPlain(entry.path().string());

				if (dirinfo.second == 0)
				{ // actual ham
					if (is_ham)
					{
						// true_ham;
						confusionMatrix[0][0]++;
					}
					else
					{
						// false_spam
						confusionMatrix[0][1]++;
					}
				}
				else
				{ // actual spam
					if (!is_ham)
					{
						// true_spam
						confusionMatrix[1][1]++;
					}
					else
					{
						// false_ham
						confusionMatrix[1][0]++;
					}
				}
			}
		}
	}
	
	Ciphertext evaluateEncryptedQuery(Ciphertext &encrypted_query, Evaluator &evaluator, BatchEncoder &encoder, GaloisKeys &galois_keys)
	{
		Ciphertext result = encrypted_query;

		// //? debug
		// cout << "TVHamProbsSize()=" << TVHamProbs.size() << endl;
		// for (size_t i = 0; i < TVHamProbs.size(); ++i)
		// {
		// 	cout << "|" << TVHamProbs[i] << "|";
		// }
		// cout << endl;
		// cout << "TVSpamProbsSize()=" << TVSpamProbs.size() << endl;
		// for (size_t i = 0; i < TVSpamProbs.size(); ++i)
		// {
		// 	cout << "|" << TVSpamProbs[i] << "|";
		// }
		// cout << endl;

		Plaintext plain_ham, plain_spam;
		encoder.encode(TVHamProbs, plain_ham);
		encoder.encode(TVSpamProbs, plain_spam);
		Ciphertext query_ham, query_spam;
		evaluator.multiply_plain(encrypted_query, plain_ham, query_ham);
		evaluator.multiply_plain(encrypted_query, plain_spam, query_spam);

		// Secure sum for Ham
		Ciphertext sum_ham = query_ham;
		int ham_size = static_cast<int>(TVHamProbs.size());
		for (int i = 0; (1 << i) < ham_size; ++i)
		{
			Ciphertext rotated;
			evaluator.rotate_rows(sum_ham, 1 << i, galois_keys, rotated);
			evaluator.add_inplace(sum_ham, rotated);
		}

		// Secure sum for Spam
		Ciphertext sum_spam = query_spam;
		int spam_size = static_cast<int>(TVSpamProbs.size());
		for (int i = 0; (1 << i) < spam_size; ++i)
		{
			Ciphertext rotated;
			evaluator.rotate_rows(sum_spam, 1 << i, galois_keys, rotated);
			evaluator.add_inplace(sum_spam, rotated);
		}

		Ciphertext diff;
		evaluator.sub(sum_ham, sum_spam, diff);
		return diff;
	}
};