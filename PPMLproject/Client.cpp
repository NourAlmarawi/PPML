#include <cmath>
#include <iostream>
#include <fstream> // for ifstream
#include <cctype>  // for tolower(), isalpha()
#include <string>
#include <vector>
#include <iomanip>
#include "seal/seal.h"
using namespace std;
using namespace seal;

#ifndef _Client_H
#define _Client_H

class Client
{
public:
    shared_ptr<SEALContext> context;
    PublicKey public_key;
    SecretKey secret_key;
    Encryptor *encryptor = nullptr;
    Evaluator *evaluator = nullptr;
    Decryptor *decryptor = nullptr;
    BatchEncoder *batch_encoder = nullptr;
    GaloisKeys gal_keys;
    RelinKeys relin_keys;

    Client()
    {
    }

    void initializeSEAL(int polyModulus)
    {
        EncryptionParameters parms(scheme_type::BFV);
        parms.set_poly_modulus_degree(polyModulus);
        parms.set_coeff_modulus(DefaultParams::coeff_modulus_128(polyModulus));
        if (polyModulus == 4096)
        {
            parms.set_plain_modulus(65537);
        }
        else if (polyModulus == 16384)
        {
            parms.set_plain_modulus(65537);
        }
        else
        {
            parms.set_plain_modulus(65537);
        }
        context = SEALContext::Create(parms);
        print_parameters(context);
        auto qualifiers = context->context_data()->qualifiers();
        cout << "Batching enabled: " << boolalpha << qualifiers.using_batching << endl;
        KeyGenerator keygen(context);
        public_key = keygen.public_key();
        secret_key = keygen.secret_key();
        if (encryptor) delete encryptor;
        if (evaluator) delete evaluator;
        if (decryptor) delete decryptor;
        if (batch_encoder) delete batch_encoder;
        encryptor = new Encryptor(context, public_key);
        evaluator = new Evaluator(context);
        decryptor = new Decryptor(context, secret_key);
        batch_encoder = new BatchEncoder(context);
        gal_keys = keygen.galois_keys(30);
        relin_keys = keygen.relin_keys(30);
        cout << "SEAL initialized successfully" << endl;
    }

    Ciphertext createQueryAccordingToSelectedFeauters(int polyModulus, const string &filename, vector<string> &selectedFeatures)
    {
        ifstream infile(filename);
        vector<uint64_t> query(selectedFeatures.size(), 0);
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

        cout << "Query vector: ";
        for (const auto &q : query) cout << "|"<< q << "|"; cout << endl;
        
        // resize
        size_t slot_count = batch_encoder->slot_count();
        if (query.size() < slot_count){
            query.resize(slot_count, 0ULL);
        }
        // Encode
        Plaintext plain_query;
        batch_encoder->encode(query, plain_query);

        // Encrypt
        Ciphertext encrypted_query;
        encryptor->encrypt(plain_query, encrypted_query);
        cout << "Query encrypted successfully" << endl;
        cout << "Encrypted query size: " << encrypted_query.size() << endl;
        cout << "NB: " << decryptor->invariant_noise_budget(encrypted_query) << endl;

        return encrypted_query;
        
    };

    void decryptResult(Ciphertext processedQuery){
        Plaintext plain_result;
        decryptor->decrypt(processedQuery, plain_result);

        vector<uint64_t> result;
        batch_encoder->decode(plain_result, result);

        uint64_t val = result[0];
        cout << "Decrypted value: " << val << endl;
        int modulus = context->context_data()->parms().plain_modulus().value();
        if (val > modulus / 2)
        {
            cout << "Spam\n";
        }
        else
        {
            cout << "Ham\n";
        }
    }

private:    

    void print_parameters(shared_ptr<SEALContext> context)
    {
        if (!context)
        {
            throw invalid_argument("context is not set");
        }
        auto &context_data = *context->context_data();

        cout << "/ Encryption parameters:" << endl;
        cout << "| poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << endl;

        /*
        Print the size of the true (product) coefficient modulus.
        */
        cout << "| coeff_modulus size: " << context_data.total_coeff_modulus_bit_count() << " bits" << endl;

        /*
        For the BFV scheme print the plain_modulus parameter.
        */
        if (context_data.parms().scheme() == scheme_type::BFV)
        {
            cout << "| plain_modulus: " << context_data.parms().plain_modulus().value() << endl;
        }

        cout << "\\ noise_standard_deviation: " << context_data.parms().noise_standard_deviation() << endl;
        cout << endl;
    }

    void print_matrix(const vector<uint64_t> &matrix, size_t row_size)
    {
        cout << endl;

        size_t print_size = 5;

        cout << "    [";
        for (size_t i = 0; i < print_size; i++)
        {
            cout << setw(3) << matrix[i] << ",";
        }
        cout << setw(3) << " ...,";
        for (size_t i = row_size - print_size; i < row_size; i++)
        {
            cout << setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
        }
        cout << "    [";
        for (size_t i = row_size; i < row_size + print_size; i++)
        {
            cout << setw(3) << matrix[i] << ",";
        }
        cout << setw(3) << " ...,";
        for (size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
        {
            cout << setw(3) << matrix[i] << ((i != 2 * row_size - 1) ? "," : " ]\n");
        }
        cout << endl;
    }
};

#endif