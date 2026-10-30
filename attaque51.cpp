#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <openssl/sha.h>

using namespace std;

struct Bloc {
    int numero;
    string transaction;
    string hashPrecedent;
    string hash;
    int nonce;
};

// Calcul du SHA-256 (comme dans l'atelier 1)
string hashage(string texte) {
    unsigned char resultat[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)texte.c_str(), texte.size(), resultat);

    string hash = "";
    char chiffres[] = "0123456789abcdef";

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hash += chiffres[resultat[i] / 16];
        hash += chiffres[resultat[i] % 16];
    }
    return hash;
}

string calculHash(Bloc b) {
    return hashage(to_string(b.numero) + b.transaction
                   + b.hashPrecedent + to_string(b.nonce));
}

// Miner un bloc avec Proof of Work
void miner(Bloc &b, int difficulte) {
    string zeros(difficulte, '0');
    b.nonce = 0;
    b.hash = calculHash(b);

    while (b.hash.substr(0, difficulte) != zeros) {
        b.nonce++;
        b.hash = calculHash(b);
    }
}

// Ajouter un bloc a la chaine
void ajouterBloc(vector<Bloc> &chaine, string transaction, int difficulte) {
    Bloc b;
    b.numero = chaine.size();
    b.transaction = transaction;
    b.hashPrecedent = chaine.empty() ? "0" : chaine.back().hash;
    miner(b, difficulte);
    chaine.push_back(b);
}

// Verifier les hashes et les liens entre les blocs
bool verifier(vector<Bloc> chaine, int difficulte) {
    if (chaine.empty()) return false;
    string zeros(difficulte, '0');

    for (int i = 0; i < (int)chaine.size(); i++) {
        if (chaine[i].numero != i) return false;
        if (chaine[i].hash != calculHash(chaine[i])) return false;
        if (chaine[i].hash.substr(0, difficulte) != zeros) return false;

        if (i == 0) {
            if (chaine[i].hashPrecedent != "0") return false;
        } else {
            if (chaine[i].hashPrecedent != chaine[i - 1].hash) return false;
        }
    }
    return true;
}

// Chercher une transaction dans une chaine
bool contient(vector<Bloc> chaine, string transaction) {
    for (int i = 0; i < (int)chaine.size(); i++) {
        if (chaine[i].transaction == transaction) return true;
    }
    return false;
}

int main() {
    srand(7); // Meme scenario a chaque execution sur le meme environnement

    int difficulte = 3;
    int puissanceAttaquant = 70; // 70 % de la puissance de minage simulee
    int tours = 20;

    string paiement = "Piece1 : Alice -> Magasin";
    string doubleDepense = "Piece1 : Alice -> Alice";

    vector<Bloc> honnete;
    ajouterBloc(honnete, "Genesis", difficulte);
    ajouterBloc(honnete, paiement, difficulte);

    // L'attaquant commence une branche privee depuis Genesis
    vector<Bloc> attaquant;
    attaquant.push_back(honnete[0]);

    // A chaque tour, un seul cote trouve un bloc.
    // 70 % de chances pour l'attaquant, 30 % pour les honnetes.
    for (int tour = 1; tour <= tours; tour++) {
        int choix = rand() % 100;

        if (choix < puissanceAttaquant) {
            if (attaquant.size() == 1) {
                ajouterBloc(attaquant, doubleDepense, difficulte);
            } else {
                ajouterBloc(attaquant, "Bloc prive " + to_string(tour), difficulte);
            }
        } else {
            ajouterBloc(honnete, "Bloc honnete " + to_string(tour), difficulte);
        }
    }

    cout << "===== SIMULATION 51% ATTACK =====\n";
    cout << "Puissance attaquant : " << puissanceAttaquant << "%\n";
    cout << "Tours : " << tours << "\n";
    cout << "Blocs honnetes (sans Genesis) : " << honnete.size() - 1 << "\n";
    cout << "Blocs attaquant (sans Genesis) : " << attaquant.size() - 1 << "\n";
    cout << "Chaine honnete valide : " << (verifier(honnete, difficulte) ? "Oui" : "Non") << "\n";
    cout << "Chaine attaquant valide : " << (verifier(attaquant, difficulte) ? "Oui" : "Non") << "\n";

    // Regle simplifiee : la chaine valide la plus longue est adoptee
    vector<Bloc> sansProtection = honnete;
    bool reorganisation = false;

    if (verifier(attaquant, difficulte) && attaquant.size() > honnete.size()) {
        sansProtection = attaquant;
        reorganisation = true;
    }

    // Detection : une branche plus longue remplace le paiement par une autre depense
    bool alerte = reorganisation
               && contient(honnete, paiement)
               && contient(attaquant, doubleDepense)
               && !contient(attaquant, paiement);

    cout << "\n===== SANS PROTECTION =====\n";
    cout << "Reorganisation : " << (reorganisation ? "Oui" : "Non") << "\n";
    cout << "Paiement magasin present : "
         << (contient(sansProtection, paiement) ? "Oui" : "Non") << "\n";
    cout << "Double depense presente : "
         << (contient(sansProtection, doubleDepense) ? "Oui" : "Non") << "\n";

    cout << "\n===== DETECTION / PROTECTION =====\n";
    cout << "Alerte de conflit : " << (alerte ? "Oui" : "Non") << "\n";

    // Defense pedagogique : refuser la reorganisation si conflit detecte
    vector<Bloc> protegee = honnete;
    if (reorganisation && !alerte) protegee = attaquant;

    cout << "Paiement magasin conserve : "
         << (contient(protegee, paiement) ? "Oui" : "Non") << "\n";
    cout << "Chaine protegee valide : "
         << (verifier(protegee, difficulte) ? "Oui" : "Non") << "\n";

    return 0;
}
