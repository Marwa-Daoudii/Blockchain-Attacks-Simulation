
#include <iostream>
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <chrono>

using namespace std;

// Fonction de hashage SHA-256
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

// Structure d'un bloc
struct Bloc {
    int numero;
    string donnees;
    string hashPrecedent;
    string hash;
    int nonce;
};

// Fonction Proof of Work
void miner(Bloc &b, int difficulte) {

    string zeros(difficulte, '0');

    b.nonce = 0;

    while (true) {
        string texte = to_string(b.numero)
                     + b.donnees
                     + b.hashPrecedent
                     + to_string(b.nonce);

        b.hash = hashage(texte);

        if (b.hash.substr(0, difficulte) == zeros) {
            break;
        }

        b.nonce++;
    }
}

int main() {

    for (int difficulte = 1; difficulte <= 4; difficulte++) {

        vector<Bloc> chaine;

        auto debut = chrono::steady_clock::now();

        for (int i = 0; i < 3; i++) {

            Bloc b;
            b.numero = i + 1;
            b.donnees = "Transaction " + to_string(i + 1);

            if (i == 0) {
                b.hashPrecedent = "0";
            } else {
                b.hashPrecedent = chaine[i - 1].hash;
            }

            miner(b, difficulte);
            chaine.push_back(b);

            cout << "Bloc " << b.numero << endl;
            cout << "Nonce : " << b.nonce << endl;
            cout << "Hash : " << b.hash << endl;
        }

        auto fin = chrono::steady_clock::now();

        double temps = chrono::duration<double>(fin - debut).count();

        cout << "Difficulte : " << difficulte << endl;
        cout << "Temps : " << temps << " secondes" << endl;
        cout << "------------------------" << endl;
    }

    return 0;
}
