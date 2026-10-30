
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <openssl/sha.h>

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
    string transaction;
    string hashPrecedent;
    string hash;
    int nonce;
    string validateur;
};

// Structure d'un validateur
struct Validateur {
    string nom;
    int stake;
};

// Calculer le hash d'un bloc
string calculHash(Bloc b) {
    string texte = to_string(b.numero)
                 + b.transaction
                 + b.hashPrecedent
                 + to_string(b.nonce)
                 + b.validateur;

    return hashage(texte);
}

// Proof of Work
void proofOfWork(Bloc &b, int difficulte) {
    string zeros(difficulte, '0');

    b.nonce = 0;
    b.validateur = "";
    b.hash = calculHash(b);

    while (b.hash.substr(0, difficulte) != zeros) {
        b.nonce++;
        b.hash = calculHash(b);
    }
}

// Proof of Stake
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
            b.validateur = v[i].nom;
            break;
        }
    }

    b.nonce = 0;
    b.hash = calculHash(b);
}

// Ajouter un bloc
void ajouterBloc(vector<Bloc> &chaine, string transaction,
                 bool pow, int difficulte,
                 vector<Validateur> validateurs) {

    Bloc b;
    b.numero = chaine.size() + 1;
    b.transaction = transaction;

    if (chaine.empty()) {
        b.hashPrecedent = "0";
    } else {
        b.hashPrecedent = chaine.back().hash;
    }

    if (pow) {
        proofOfWork(b, difficulte);
    } else {
        proofOfStake(b, validateurs);
    }

    chaine.push_back(b);
}

// Verifier l'integrite de la blockchain
bool verifier(vector<Bloc> chaine, bool pow,
              int difficulte, vector<Validateur> validateurs) {

    string zeros(difficulte, '0');

    for (int i = 0; i < chaine.size(); i++) {

        // Verifier le numero
        if (chaine[i].numero != i + 1) {
            return false;
        }

        // Verifier le hash du bloc
        if (chaine[i].hash != calculHash(chaine[i])) {
            return false;
        }

        // Verifier le chainage
        if (i == 0) {
            if (chaine[i].hashPrecedent != "0") {
                return false;
            }
        } else {
            if (chaine[i].hashPrecedent != chaine[i - 1].hash) {
                return false;
            }
        }

        if (pow) {
            // Verifier la difficulte
            if (chaine[i].hash.substr(0, difficulte) != zeros) {
                return false;
            }
        } else {
            // Verifier que le validateur existe
            bool trouve = false;

            for (int j = 0; j < validateurs.size(); j++) {
                if (chaine[i].validateur == validateurs[j].nom
                    && validateurs[j].stake > 0) {
                    trouve = true;
                }
            }

            if (!trouve || chaine[i].nonce != 0) {
                return false;
            }
        }
    }

    return true;
}

// Afficher la blockchain
void afficher(vector<Bloc> chaine) {
    for (int i = 0; i < chaine.size(); i++) {
        cout << "\nBloc " << chaine[i].numero << endl;
        cout << "Transaction : " << chaine[i].transaction << endl;
        cout << "Hash precedent : " << chaine[i].hashPrecedent << endl;
        cout << "Hash : " << chaine[i].hash << endl;
        cout << "Nonce : " << chaine[i].nonce << endl;

        if (chaine[i].validateur != "") {
            cout << "Validateur : " << chaine[i].validateur << endl;
        }
    }
}

int main() {
    srand(time(0));

    vector<Validateur> validateurs = {
        {"Alice", 50},
        {"Bob", 30},
        {"Charlie", 20}
    };

    vector<Bloc> chainePoW;
    vector<Bloc> chainePoS;

    int difficulte = 4;

    // Blockchain avec Proof of Work
    cout << "===== PROOF OF WORK =====" << endl;

    auto debut1 = chrono::steady_clock::now();

    ajouterBloc(chainePoW, "Transaction A", true, difficulte, validateurs);
    ajouterBloc(chainePoW, "Transaction B", true, difficulte, validateurs);
    ajouterBloc(chainePoW, "Transaction C", true, difficulte, validateurs);

    auto fin1 = chrono::steady_clock::now();

    double tempsPoW =
        chrono::duration<double>(fin1 - debut1).count();

    afficher(chainePoW);

    cout << "\nIntegrite PoW : "
         << (verifier(chainePoW, true, difficulte, validateurs)
             ? "Valide" : "Invalide") << endl;

    // Blockchain avec Proof of Stake
    cout << "\n===== PROOF OF STAKE =====" << endl;

    auto debut2 = chrono::steady_clock::now();

    ajouterBloc(chainePoS, "Transaction A", false, difficulte, validateurs);
    ajouterBloc(chainePoS, "Transaction B", false, difficulte, validateurs);
    ajouterBloc(chainePoS, "Transaction C", false, difficulte, validateurs);

    auto fin2 = chrono::steady_clock::now();

    double tempsPoS =
        chrono::duration<double>(fin2 - debut2).count();

    afficher(chainePoS);

    cout << "\nIntegrite PoS : "
         << (verifier(chainePoS, false, difficulte, validateurs)
             ? "Valide" : "Invalide") << endl;

    // Comparaison des temps
    cout << "\n===== COMPARAISON =====" << endl;
    cout << "Temps PoW : " << tempsPoW << " secondes" << endl;
    cout << "Temps PoS : " << tempsPoS << " secondes" << endl;

    if (tempsPoW < tempsPoS) {
        cout << "PoW est plus rapide" << endl;
    } else {
        cout << "PoS est plus rapide" << endl;
    }

    // Test de modification
    cout << "\n===== TEST MODIFICATION =====" << endl;

    chainePoW[1].transaction = "Transaction modifiee";
    chainePoS[1].transaction = "Transaction modifiee";

    cout << "Integrite PoW apres modification : "
         << (verifier(chainePoW, true, difficulte, validateurs)
             ? "Valide" : "Invalide") << endl;

    cout << "Integrite PoS apres modification : "
         << (verifier(chainePoS, false, difficulte, validateurs)
             ? "Valide" : "Invalide") << endl;

    return 0;
}
