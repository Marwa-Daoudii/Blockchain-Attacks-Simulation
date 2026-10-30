
#include <iostream>
#include <vector>
#include <string>
#include <openssl/sha.h>

using namespace std;

// Fonction de hashage
string hashage(string texte) {
    unsigned char resultat[SHA256_DIGEST_LENGTH];

    SHA256((const unsigned char*)texte.c_str(),
           texte.size(), resultat);

    string hash = "";
    char chiffres[] = "0123456789abcdef";

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hash += chiffres[resultat[i] / 16];
        hash += chiffres[resultat[i] % 16];
    }

    return hash;
}

// Fonction pour construire l'arbre de Merkle
string merkle(vector<string> transactions) {

    vector<string> hashes;

    for (int i = 0; i < transactions.size(); i++) {
        hashes.push_back(hashage(transactions[i]));
    }

    if (hashes.empty()) {
        return "";
    }

    while (hashes.size() > 1) {

        if (hashes.size() % 2 != 0) {
            hashes.push_back(hashes.back());
        }

        vector<string> nouveau;

        for (int i = 0; i < hashes.size(); i += 2) {
            string h = hashes[i] + hashes[i + 1];
            nouveau.push_back(hashage(h));
        }

        hashes = nouveau;
    }

    return hashes[0];
}

int main() {

    vector<string> transactions = {
        "Transaction A",
        "Transaction B",
        "Transaction C",
        "Transaction D"
    };

    cout << "Merkle Root initial : "
         << merkle(transactions) << endl;

    transactions[0] = "Transaction X";

    cout << "Merkle Root apres modification : "
         << merkle(transactions) << endl;

    return 0;
}
