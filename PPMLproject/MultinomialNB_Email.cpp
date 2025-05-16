#include <time.h>                // for time()
#include "MultinomialNB_Email.h"
#include <cmath>
#include <iostream>
#include <iostream>
#include <fstream>          // for ifstream
#include <cctype>           
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <experimental/filesystem>
namespace fs = std::experimental;
using std::vector;
using namespace std;

MultinomialNB_Email::MultinomialNB_Email()
{
	spamMails = 0;
	hamMails = 0;
	wordsGlobalVector.push_back("");
	spamWordsInDocumentsCount.push_back(0);
	hamWordsInDocumentsCount.push_back(0);
	spamWordsFreq.push_back(0);
	hamWordsFreq.push_back(0);
}

MultinomialNB_Email::MultinomialNB_Email(string path, int mValue, long long int &timeElapsed)
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
	std::cout << "Initializing now.." << endl;
	
	// reading ham
	string hamPath = path + "\\ham", filename, ANSIWord;
	time_start = chrono::high_resolution_clock::now();
	for (const auto & entry : fs::filesystem::recursive_directory_iterator(hamPath))
	{
		filename = entry.path().string();
		std::cout <<"opening file: "<< filename << std::endl;

		ifstream inputFile(filename);
		vector<string> tmpLocalWords;
		vector<long long int> tmpLocalWordsInDoc;
		vector<long long int> tmpLocalWordsFreq;
		bool existsInList = false;

		while (inputFile >> ANSIWord)
		{
			existsInList = false;
			for (int i = 0; i < tmpLocalWords.size(); i++)
			{
				if (ANSIWord == tmpLocalWords[i])
				{
					existsInList = true;
					//tmpLocalWordsInDoc[i] = 1;
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
	string spamPath = path + "\\spam";
	for (const auto & entry : fs::filesystem::recursive_directory_iterator(spamPath))
	{
		filename = entry.path().string();
		std::cout <<"opening file: "<< filename << std::endl;
		ifstream inputFile(filename);
		vector<string> tmpLocalWords;
		vector<long long int> tmpLocalWordsInDoc;
		vector<long long int> tmpLocalWordsFreq;
		bool existsInList = false;

		while (inputFile >> ANSIWord)
		{
			existsInList = false;
			for (int i = 0; i < tmpLocalWords.size(); i++)
			{
				if (ANSIWord == tmpLocalWords[i])
				{
					existsInList = true;
					//tmpLocalWordsInDoc[i] = 1;
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
	timeElapsed = time_diff.count()/1000;
	std::cout << "In total we have " << wordsGlobalVector.size() << " words in this dataset" << endl;
	std::cout << "The initialization was done in [" << time_diff.count() / 1000 << " ms]" << endl;
}

void MultinomialNB_Email::getPrivateData(vector<string> &wordsGlobalVectorOutput,
	vector<long long int> &hamWordsInDocumentsCountOutput,
	vector<long long int> &spamWordsInDocumentsCountOutput,	
	int & hamMailsOutput, int & spamMailsOutput)
{
	wordsGlobalVectorOutput = wordsGlobalVector;
	hamWordsInDocumentsCountOutput = hamWordsInDocumentsCount;
	spamWordsInDocumentsCountOutput = spamWordsInDocumentsCount;	
	hamMailsOutput = hamMails;
	spamMailsOutput = spamMails;
}

void MultinomialNB_Email::getWordsVector(vector<string> &wordsGlobalVectorOutput)
{
	wordsGlobalVectorOutput = wordsGlobalVector;
}


void MultinomialNB_Email::getExtendedPrivateData(const vector<string> &wordsGlobalAggregated,
	vector<long long int> &hamWordsInDocumentsCountGlobal,
	vector<long long int> &spamWordsInDocumentsCountGlobal,
	int & hamMailsOutput, int & spamMailsOutput)
{
	hamMailsOutput = hamMails;
	spamMailsOutput = spamMails;

	for (int i = 0; i < wordsGlobalAggregated.size(); i++)
	{
		for (int j = 0; j < wordsGlobalVector.size(); j++)
		{
			if (wordsGlobalAggregated[i] == wordsGlobalVector[j])
			{
				hamWordsInDocumentsCountGlobal[i] = hamWordsInDocumentsCount[j];
				spamWordsInDocumentsCountGlobal[i] = spamWordsInDocumentsCount[j];
				break;
			}			
		}
	}
}


void  MultinomialNB_Email::getSelectedFeaturesParameters(vector<string> &selectedFeaturesOutput,
	vector<long double>& logOfProbWordIsHamOutput,
	vector<long double>& logOfProbWordIsSpamOutput,
	long double &logProbHamOutput, long double & logProbSpamOutput)
{
	selectedFeaturesOutput = selectedFeatures;
	logOfProbWordIsHamOutput = logOfProbWordIsHam;
	logOfProbWordIsSpamOutput = logOfProbWordIsSpam;
	logProbHamOutput = logProbHam;
	logProbSpamOutput = logProbSpam;	
}

void MultinomialNB_Email::getSelectedFeaturesOnly(vector<string> &selectedFeaturesOutput)
{
	selectedFeaturesOutput = selectedFeatures;
}


void MultinomialNB_Email::classify(string path, long long int &timeElapsed)
{
	chrono::high_resolution_clock::time_point time_start, time_end;
	time_start = chrono::high_resolution_clock::now();
	time_end = chrono::high_resolution_clock::now();
	auto time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);

	int countQuery = 0;
	string hamPath = path + "\\ham", filename, word;
	long double logSpamQueryProb = 0.0, logHamQueryProb = 0.0;
	
	time_start = chrono::high_resolution_clock::now();
	for (const auto & entry : fs::filesystem::recursive_directory_iterator(hamPath))
	{
		filename = entry.path().string();
		std::cout <<"opening file: "<< filename << std::endl;
		logSpamQueryProb = logProbSpam;
		logHamQueryProb = logProbHam;

		ifstream inputFile(filename);

		int begIndex = 1, endIndex = m, midIndex = (begIndex + endIndex) / 2;
		while (inputFile >> word)
		{
			begIndex = 1, endIndex = m, midIndex = (begIndex + endIndex) / 2;
			while (!((begIndex + 1) == endIndex))
			{
				if (word == selectedFeatures[midIndex])
				{
					logSpamQueryProb += logOfProbWordIsSpam[midIndex];
					logHamQueryProb += logOfProbWordIsHam[midIndex];
					break;
				}
				else if (word > selectedFeatures[midIndex])
				{
					endIndex = midIndex;
					midIndex = (begIndex + endIndex) / 2;
				}
				else
				{
					begIndex = midIndex;
					midIndex = (begIndex + endIndex) / 2;
				}
			}
			if (word == selectedFeatures[begIndex])
			{
				logSpamQueryProb += logOfProbWordIsSpam[begIndex];
				logHamQueryProb += logOfProbWordIsHam[begIndex];
			}
			else if (word == selectedFeatures[endIndex])
			{
				logSpamQueryProb += logOfProbWordIsSpam[endIndex];
				logHamQueryProb += logOfProbWordIsHam[endIndex];
			}
		}

		if (logHamQueryProb > logSpamQueryProb)
			confusionMatrix[0][0]++;
		else
			confusionMatrix[0][1]++;
	}

	countQuery = 0;
	string spamPath = path + "\\spam";
	for (const auto & entry : fs::filesystem::recursive_directory_iterator(spamPath))
	{
		filename = entry.path().string();
		std::cout <<"opening file: "<< filename << std::endl;
		logSpamQueryProb = logProbSpam;
		logHamQueryProb = logProbHam;

		ifstream inputFile(filename);

		int begIndex = 1, endIndex = m, midIndex = (begIndex + endIndex) / 2;
		while (inputFile >> word)
		{
			//wordCount++;
			//existsInList = false;
			begIndex = 1, endIndex = m, midIndex = (begIndex + endIndex) / 2;
			while (!((begIndex + 1) == endIndex))
			{
				if (word == selectedFeatures[midIndex])
				{
					logSpamQueryProb += logOfProbWordIsSpam[midIndex];
					logHamQueryProb += logOfProbWordIsHam[midIndex];
					break;
				}
				else if (word > selectedFeatures[midIndex])
				{
					endIndex = midIndex;
					midIndex = (begIndex + endIndex) / 2;
				}
				else
				{
					begIndex = midIndex;
					midIndex = (begIndex + endIndex) / 2;
				}
			}
			if (word == selectedFeatures[begIndex])
			{
				logSpamQueryProb += logOfProbWordIsSpam[begIndex];
				logHamQueryProb += logOfProbWordIsHam[begIndex];
			}
			else if (word == selectedFeatures[endIndex])
			{
				logSpamQueryProb += logOfProbWordIsSpam[endIndex];
				logHamQueryProb += logOfProbWordIsHam[endIndex];
			}

		}

		if (logSpamQueryProb > logHamQueryProb)
			confusionMatrix[1][1]++;
		else
			confusionMatrix[1][0]++;		
	}
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	timeElapsed = time_diff.count() / 1000;
	std::cout << "The classification was done in [" << time_diff.count() / 1000 << " ms]" << endl;
}

void MultinomialNB_Email::printConfionMat()
{
	std::cout << "Printing the confusion mat for the improved classification case:" << endl;
	for (int i = 0; i < confusionMatrix.size(); i++)
	{
		std::cout << endl;
		for (int j = 0; j < confusionMatrix[0].size(); j++)
		{
			std::cout << confusionMatrix[i][j] << "\t";
		}
	}
	std::cout << endl << "The accuracy of the improved classification is: " << (long double)100 * (confusionMatrix[0][0] + confusionMatrix[1][1]) / (long double)(hamMails + spamMails) << " %" << endl << endl;
}

void MultinomialNB_Email::train(long long int &timeElapsed)
{
	chrono::high_resolution_clock::time_point time_start, time_end;
	time_start = chrono::high_resolution_clock::now();
	time_end = chrono::high_resolution_clock::now();
	auto time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
		
	vector<string> tmpSelectedFeatures(m + 1, "");
	selectedFeatures = tmpSelectedFeatures;
	vector<long long int> spamWordsNOTInDocumentsCount; spamWordsNOTInDocumentsCount.push_back(0);
	vector<long long int> hamWordsNOTInDocumentsCount; hamWordsNOTInDocumentsCount.push_back(0);
	vector<long double> tmpLogOfProbWordIsHam(m + 1, 0), tmpLogOfProbWordIsSpam(m + 1, 0);
	logOfProbWordIsHam = tmpLogOfProbWordIsHam; logOfProbWordIsSpam = tmpLogOfProbWordIsSpam;


	time_start = chrono::high_resolution_clock::now();
	for (int i = 1; i < wordsGlobalVector.size(); i++)
	{
		hamWordsNOTInDocumentsCount.push_back(hamMails - hamWordsInDocumentsCount[i]);
		spamWordsNOTInDocumentsCount.push_back(spamMails - spamWordsInDocumentsCount[i]);
	}

	int totalNrMails = spamMails + hamMails;
	long double probSpam = (long double)(spamMails + m) / (long double)(totalNrMails + 2*m), probHam = 1 - probSpam;
	logProbHam = log(probHam);
	logProbSpam = log(probSpam);

	vector<long double> probWordInDoc(wordsGlobalVector.size(), 0),
		probWordNOTInDoc(wordsGlobalVector.size(), 0); //p(tk) and p(not(tk))
	vector<long double> probWordInHamDocs(wordsGlobalVector.size(), 0),
		probWordNOTInHamDocs(wordsGlobalVector.size(), 0),
		probWordInSpamDocs(wordsGlobalVector.size(), 0),
		probWordNOTInSpamDocs(wordsGlobalVector.size(), 0);
	vector<long double> informationGain(wordsGlobalVector.size(), -9999999999.9);

	// ignoring words that appear in less than 5 documents
	// due to their low information gain
	int countOfLessThanFive = 0;
	for (int i = 0; i < wordsGlobalVector.size(); i++)
	{
		if ((hamWordsInDocumentsCount[i] + spamWordsInDocumentsCount[i]) >= 5)
		{
			probWordInDoc[i] = ((long double)hamWordsInDocumentsCount[i] + (long double)spamWordsInDocumentsCount[i]); // ((long double)totalNrMails);
			probWordNOTInDoc[i] = (long double)(totalNrMails - probWordInDoc[i]);

			probWordInHamDocs[i] = (long double)(hamWordsInDocumentsCount[i] + 1); // (long double)(totalNrMails + wordsGlobalVector.size());
			probWordNOTInHamDocs[i] = (long double)(hamWordsNOTInDocumentsCount[i] + 1); //(long double)(totalNrMails + wordsGlobalVector.size());
			probWordInSpamDocs[i] = (long double)(spamWordsInDocumentsCount[i] + 1); // (long double)(totalNrMails + wordsGlobalVector.size());
			probWordNOTInSpamDocs[i] = (long double)(spamWordsNOTInDocumentsCount[i] + 1); // (long double)(totalNrMails + wordsGlobalVector.size());

			const long double smoothing = 1e-10; // Small constant to avoid log(0)
			informationGain[i] = probWordInHamDocs[i] * log((probWordInHamDocs[i] + smoothing) / ((probWordInDoc[i] * probHam) + smoothing))
				+ probWordNOTInHamDocs[i] * log((probWordNOTInHamDocs[i] + smoothing) / ((probWordNOTInDoc[i] * probHam) + smoothing))
				+ probWordInSpamDocs[i] * log((probWordInSpamDocs[i] + smoothing) / ((probWordInDoc[i] * probSpam) + smoothing))
				+ probWordNOTInSpamDocs[i] * log((probWordNOTInSpamDocs[i] + smoothing) / ((probWordNOTInDoc[i] * probSpam) + smoothing));

		}
		else
		{
			countOfLessThanFive++;
		}
	}
	std::cout << "At the train f-ion() of MultinomialNB_Email class countOfLessThanFive=" << countOfLessThanFive << endl;

	string tmpString;
	long long int tmpInt;
	long double tmpDouble;
	for (int i = 1; i < wordsGlobalVector.size() - 1; i++)
	{
		for (int j = i; j < wordsGlobalVector.size() - 1; j++)
		{
			if (informationGain[i] < informationGain[j])
			{
				//swap all the stuff
				//wordsGlobalVector
				tmpString = wordsGlobalVector[i];
				wordsGlobalVector[i] = wordsGlobalVector[j];
				wordsGlobalVector[j] = tmpString;

				//spamWordsInDocumentsCount
				tmpDouble = spamWordsInDocumentsCount[i];
				spamWordsInDocumentsCount[i] = spamWordsInDocumentsCount[j];
				spamWordsInDocumentsCount[j] = tmpDouble;

				//hamWordsInDocumentsCount
				tmpDouble = hamWordsInDocumentsCount[i];
				hamWordsInDocumentsCount[i] = hamWordsInDocumentsCount[j];
				hamWordsInDocumentsCount[j] = tmpDouble;

				//mutual information
				tmpDouble = informationGain[i];
				informationGain[i] = informationGain[j];
				informationGain[j] = tmpDouble;
				
				//spamWordsFreq
				tmpInt = spamWordsFreq[i];
				spamWordsFreq[i] = spamWordsFreq[j];
				spamWordsFreq[j] = tmpInt;
				
				//hamWordsFreq
				tmpInt = hamWordsFreq[i];
				hamWordsFreq[i] = hamWordsFreq[j];
				hamWordsFreq[j] = tmpInt;
			}
		}
	}

	if (wordsGlobalVector.size() < m)
		m = wordsGlobalVector.size();


	std::cout << "The features with the top " << m << " highest mutual information are:\n\n";
	std::cout << std::setw(15) << "Feature"
			  << std::setw(20) << "Info Gain"
			  << std::setw(25) << "Log(P(Ham|Word))"
			  << std::setw(25) << "Log(P(Spam|Word))"
			  << std::endl;
	for (int i = 1; i <= m; i++)
	{
		selectedFeatures[i] = wordsGlobalVector[i];
		const long double smoothing = 1e-10; // Small constant to avoid log(0)
		logOfProbWordIsHam[i] = log(((long double)hamWordsFreq[i] + 1 + smoothing) / (long double)(hamMails + m + smoothing));
		logOfProbWordIsSpam[i] = log(((long double)spamWordsFreq[i] + 1 + smoothing) / (long double)(spamMails + m + smoothing));

		std::cout << std::setw(15) << selectedFeatures[i]
				  << std::setw(20) << informationGain[i]
				  << std::setw(25) << logOfProbWordIsHam[i]
				  << std::setw(25) << logOfProbWordIsSpam[i]
				  << std::endl;
	}
	
	for (int i = 1; i < m; i++)
	{
		for (int j = i + 1; j <= m; j++)
		{
			if (selectedFeatures[i] < selectedFeatures[j])
			{
				tmpString = selectedFeatures[i];
				selectedFeatures[i] = selectedFeatures[j];
				selectedFeatures[j] = tmpString;

				tmpDouble = logOfProbWordIsHam[i];
				logOfProbWordIsHam[i] = logOfProbWordIsHam[j];
				logOfProbWordIsHam[j] = tmpDouble;

				tmpDouble = logOfProbWordIsSpam[i];
				logOfProbWordIsSpam[i] = logOfProbWordIsSpam[j];
				logOfProbWordIsSpam[j] = tmpDouble;

				// swap all the stuff
				//wordsGlobalVector
				tmpString = wordsGlobalVector[i];
				wordsGlobalVector[i] = wordsGlobalVector[j];
				wordsGlobalVector[j] = tmpString;

				//spamWordsInDocumentsCount
				tmpDouble = spamWordsInDocumentsCount[i];
				spamWordsInDocumentsCount[i] = spamWordsInDocumentsCount[j];
				spamWordsInDocumentsCount[j] = tmpDouble;

				//hamWordsInDocumentsCount
				tmpDouble = hamWordsInDocumentsCount[i];
				hamWordsInDocumentsCount[i] = hamWordsInDocumentsCount[j];
				hamWordsInDocumentsCount[j] = tmpDouble;

				//mutual information
				tmpDouble = informationGain[i];
				informationGain[i] = informationGain[j];
				informationGain[j] = tmpDouble;

				//spamWordsFreq
				tmpInt = spamWordsFreq[i];
				spamWordsFreq[i] = spamWordsFreq[j];
				spamWordsFreq[j] = tmpInt;

				//hamWordsFreq
				tmpInt = hamWordsFreq[i];
				hamWordsFreq[i] = hamWordsFreq[j];
				hamWordsFreq[j] = tmpInt;
			}
		}
		if ((i % 100) == 0)
		{
			std::cout << "Processing the " << i << "th selected feature" << endl;
		}
	}
	
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	timeElapsed = time_diff.count() / 1000;
	std::cout << "The real trainig of the selected features was done in " << timeElapsed << " ms" << endl;
}

void MultinomialNB_Email::getTVaccordingToSelectedFeatures(vector<string> selectedFeatures,
	vector<int64_t> &trainingVector)
{
	for (int i = 0; i < selectedFeatures.size(); i++)
	{
		for (int j = 0; j < wordsGlobalVector.size(); j++)
		{
			if (selectedFeatures[i] == wordsGlobalVector[j])
			{
				trainingVector[i + 1] = (int64_t) hamWordsFreq[j];
				trainingVector[i + 1 + m + 1] = (int64_t)spamWordsFreq[j];
			}
		}
	}
	trainingVector[0] = hamMails;
	trainingVector[0 + m + 1] = spamMails;
	trainingVector[trainingVector.size() - 1] = hamMails + spamMails;
}