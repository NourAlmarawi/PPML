# PPMLproject

## Overview

PPMLproject is a C++ implementation of a privacy-preserving multinomial Naive Bayes classifier for email spam detection. The project leverages [Microsoft SEAL](https://github.com/microsoft/SEAL) for homomorphic encryption, enabling secure computation on encrypted data.

## Features

- Multinomial Naive Bayes classifier for spam/ham email classification
- Privacy-preserving computation using homomorphic encryption (SEAL)
- Feature selection using a customizable feature list (`selected_features.txt`)
- Support for batch processing of email datasets
- Example test data for both ham and spam emails

## Project Structure

```
PPMLproject/
│
├── Client.cpp
├── MultinomialNB_Email.cpp
├── MultinomialNB_Email_original.cpp
├── selected_features.txt
├── enron5/
│   ├── ham/
│   └── spam/
└── outputs/
    ├── FINALRUN.txt
    ├── plain run.txt
    └── ...

```

## Getting Started

### Prerequisites

- Visual Studio 2017 or later (or compatible C++ compiler)
- [Microsoft SEAL](https://github.com/microsoft/SEAL) library

### Building

1. Clone the repository.
2. Open `PPMLproject.sln` in Visual Studio.
3. Restore/build dependencies (ensure SEAL is available).
4. Build the solution.

### Running

- Prepare your dataset in `test_data/ham/` and `test_data/spam/`.
- Edit `selected_features.txt` to specify the features (words) to use.
- Run the executable. Output files will be generated in the `outputs/` directory.

## Files

- `Client.cpp`: Client-side logic for encrypted queries.
- `MultinomialNB_Email.cpp` / `.h`: Main classifier implementation.
- `selected_features.txt`: List of selected features (words) used for classification.
- `test_data/`: Example emails for testing.
- `outputs/`: Output files from runs and experiments.

## References

- [Microsoft SEAL](https://github.com/microsoft/SEAL)
- Naive Bayes Classifier: [Wikipedia](https://en.wikipedia.org/wiki/Naive_Bayes_classifier)

```
