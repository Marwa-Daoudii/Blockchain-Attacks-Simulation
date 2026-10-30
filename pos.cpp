
#include <iostream>
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <chrono>
#include <cstdlib>
#include <ctime>

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

// Structure d'un bloc
struct Bloc {
    int numero;
    string donnees;
    string hash;
    int nonce;
};

// Structure d'un validateur
struct Validateur {
    string nom;
    int stake;
};

// Validation avec Proof of Work
void proofOfWork(Bloc &b, int difficulte) {
    string zeros(difficulte, '0');
    b.nonce = 0;

    while (true) {
        string texte = to_string(b.numero)
                     + b.donnees
                     + to_string(b.nonce);

        b.hash = hashage(texte);

        if (b.hash.substr(0, difficulte) == zeros) {
            break;
        }

        b.nonce++;
    }
}

// Validation avec Proof of Stake
void proofOfStake(Bloc &b, vector<Validateur> v) {

    int total = 0;

    for (int i = 0; i < v.size(); i++) {
        total += v[i].stake;
    }

    int choix = rand() % total;
    int somme = 0;

    for (int i = 0; i < v.size(); i++) {
        somme += v[i].stake;

        if (choix < somme) {
            cout << "Validateur choisi : "
                 << v[i].nom << endl;
            break;
        }
    }

    b.hash = hashage(to_string(b.numero) + b.donnees);
}

int main() {

    srand(time(0));

    vector<Validateur> validateurs = {
        {"Alice", 50},
        {"Bob", 30},
        {"Charlie", 20}
    };

    Bloc bloc1 = {1, "Transaction A", "", 0};
    Bloc bloc2 = {1, "Transaction A", "", 0};

    // Temps Proof of Work
    auto debut1 = chrono::steady_clock::now();

    proofOfWork(bloc1, 4);

    auto fin1 = chrono::steady_clock::now();

    double tempsPoW =
        chrono::duration<double>(fin1 - debut1).count();

    // Temps Proof of Stake
    auto debut2 = chrono::steady_clock::now();

    proofOfStake(bloc2, validateurs);

    auto fin2 = chrono::steady_clock::now();

    double tempsPoS =
        chrono::duration<double>(fin2 - debut2).count();

    cout << "\n--- Comparaison ---" << endl;

    cout << "PoW : " << tempsPoW << " secondes" << endl;
    cout << "PoS : " << tempsPoS << " secondes" << endl;

    if (tempsPoW < tempsPoS) {
        cout << "PoW est plus rapide" << endl;
    } else {
        cout << "PoS est plus rapide" << endl;
    }

    return 0;
}
