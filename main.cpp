#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <locale.h>


using namespace std;



struct Patient {
    int age;
    string name;
    string lastname;
    char gender;
    string ID;
    string blood;
    vector<string> illness; 
};

// Dosya bos ise baslik satirini yazar
void writeHeaderIfEmpty(const string& filename) {
    ifstream testFile(filename);
    bool isEmpty = testFile.peek() == ifstream::traits_type::eof(); // Dosya bos mu kontrolu
    testFile.close();

    if (isEmpty) {
        ofstream file(filename);
        
        file << left
                << setw(15) << "Name"
                << setw(15) << "Last Name"
                << setw(5)  << "Age"
                << setw(8)  << "Gender"
                << setw(10) << "ID"
                << setw(10) << "Blood"
                << setw(35) << "Illnesses"
                << "\n";
        file << "----------------------------------------------------------------------------------\n";
        file.close();
    }
}

// Kullaniciya hastaliklari sormak icin kullanilir
vector<string> inputIllnesses() {
    vector<string> illnesses;
    string ill;
    char more;

    cin.ignore(); // Onceki cin'den kalan newline'i temizler

    do {
        cout << "Enter illness: ";
        getline(cin, ill); // Bosluklu giris icin getline
        illnesses.push_back(ill);

        cout << "Add another illness? (y/n): ";
        cin >> more;
        cin.ignore(); // newline temizlenir
    } while (more == 'y' || more == 'Y'); // Kullanici y dedigi surece devam eder

    return illnesses;
}

// Yeni hasta bilgisi ekleyen fonksiyon
void addPatient(const string& filename){
    // UTF-8 olarak yazabilmek için wide stream kullan
    wofstream outFile(filename, ios::app);
    outFile.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

    if (!outFile) {
        wcerr << L"Dosya yazma hatası!\n";
        return;
    }

    Patient p;

    // Girişleri string değil wstring olarak oku
    wstring name, lastname, blood, ID;
    wchar_t gender;
    int age;

    wcout << L"Yeni hasta ekleniyor...\n";
    wcout << L"Adı: "; wcin >> name;
    wcout << L"Soyadı: "; wcin >> lastname;
    wcout << L"Cinsiyet: "; wcin >> gender;
    wcout << L"Kan grubu: "; wcin >> blood;
    wcout << L"Yaş: "; wcin >> age;
    wcout << L"TC Kimlik No: "; wcin >> ID;

    // wcin sonrası buffer temizliği
    wcin.ignore();

    // Hastalıkları string olarak al, sonra wstring'e çevir
    vector<wstring> illnesses;
    wchar_t more;
    do {
        wstring ill;
        wcout << L"Hastalık: ";
        getline(wcin, ill);
        illnesses.push_back(ill);
        wcout << L"Başka hastalık eklensin mi? (y/n): ";
        wcin >> more;
        wcin.ignore();
    } while (more == L'y' || more == L'Y');

    // Hastalıkları tek stringe çevir
    wstring allIllnesses;
    for (const auto& ill : illnesses)
        allIllnesses += ill + L", ";

    // Dosyaya yaz (Türkçe karakter destekli)
    outFile << left << setw(15) << name
                    << setw(15) << lastname
                    << setw(5)  << age
                    << setw(8)  << gender
                    << setw(10) << ID
                    << setw(10) << blood
                    << setw(35) << allIllnesses << L"\n";

    wcout << L"Hasta başarıyla eklendi!\n";
    outFile.close();
}

    Patient p;

    // Hasta bilgileri kullanicidan alinir
    cout << "ADDING NEW PATIENT\n";
    cout << "Name: \n";
    cin >> p.name;
    cout << "Last Name: \n";
    cin >> p.lastname;
    cout << "Gender: \n";
    cin >> p.gender;
    cout << "Blood Type: \n";
    cin >> p.blood;
    cout << "Age: \n";
    cin >> p.age;
    cout << "ID Number: \n";
    cin >> p.ID;

    // Hastaliklar girilir
    p.illness = inputIllnesses();

    // Tum hastaliklar tek string'e birlestirilir
    string allIllnesses;
    for (const auto& ill : p.illness) {
        allIllnesses += ill + ", ";
    }

    // Hasta bilgileri dosyaya yazilir
    outFile << left << setw(15) << p.name
                    << setw(15) << p.lastname
                    << setw(5) << p.age
                    << setw(8) << p.gender
                    << setw(10) << p.ID
                    << setw(10) << p.blood
                    << setw(35) << allIllnesses << "\n";

    cout << "Thanks! ADDED!\n";

    outFile.close(); // Dosya kapatilir
}

// Kayitli hasta bilgisi bulan fonksiyon
void findPatientByID(const string& filename) {
    ifstream file(filename);

    if (!file) {
        cerr << "Error opening file for reading.\n";
        return;
    }

    string searchID;
    cout << "Enter patient ID: ";
    cin >> searchID;

    string line;
    bool found = false;

    getline(file, line); // Skip header
    getline(file, line); // Skip empty line

    while (getline(file, line)) {
        // If the line contains the ID we're looking for
        if (line.find(searchID) != string::npos) {
            cout << "\nPatient Found:\n";
            cout << line << "\n";
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No patient with ID " << searchID << " found.\n";
    }

    file.close();
}

// Kayitli hasta bilgisi silen fonksiyon
void deletePatientByID(const string& filename) {
    string targetID;
    cout << "Enter ID of the patient to delete: ";
    cin >> targetID;

    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error opening file for reading\n";
        return;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        cerr << "Error opening temporary file\n";
        return;
    }

    string line;
    bool found = false;

    // Basliklari gec
    getline(inFile, line);
    tempFile << line << "\n";
    getline(inFile, line);
    tempFile << line << "\n";

    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, lastname, ageStr, genderStr, id, blood, illnesses;

        ss >> name >> lastname >> ageStr >> genderStr >> id >> blood;
        getline(ss, illnesses); // hastaliklar kalan satirin devamidir

        if (id == targetID) {
            found = true; // Bu satiri atla, yazma
            continue;
        }

        tempFile << line << "\n"; // ID eslesmiyorsa yaz
    }

    inFile.close();
    tempFile.close();

    if (found) {
        remove(filename.c_str());
        rename("temp.txt", filename.c_str());
        cout << "Patient with ID " << targetID << " deleted successfully.\n";
    } else {
        remove("temp.txt");
        cout << "No patient with ID " << targetID << " found.\n";
    }
}

// Hasta bilgilerini duzenlemek icin kullanilir
void editPatientIllnesses(const string& filename) {
    string targetID;
    cout << "Enter patient ID to edit illnesses: ";
    cin >> targetID;

    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error opening file for reading\n";
        return;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        cerr << "Error opening temporary file\n";
        return;
    }

    string line;
    bool found = false;

    // Baslik ve cizgi satirlarini kopyala
    getline(inFile, line);
    tempFile << line << "\n";
    getline(inFile, line);
    tempFile << line << "\n";

    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, lastname, ageStr, genderStr, id, blood, illnesses;

        ss >> name >> lastname >> ageStr >> genderStr >> id >> blood;
        getline(ss, illnesses); // Kalan hastalik bilgisi

        if (id == targetID) {
            found = true;

            // Mevcut hastaliklari parcalayalim
            vector<string> illnessList;
            stringstream illnessStream(illnesses);
            string tempIll;

            // Hastaliklar ';' veya ', ' ile ayrilmis olabilir, biz ',' olarak ayiralim:
            while (getline(illnessStream, tempIll, ',')) {
                // Trim: basta ve sonda bosluklari kaldiralim
                while (!tempIll.empty() && isspace(tempIll.front())) tempIll.erase(tempIll.begin());
                while (!tempIll.empty() && isspace(tempIll.back())) tempIll.pop_back();
                if (!tempIll.empty()) illnessList.push_back(tempIll);
            }

            // Hastalik duzenleme menu
            int choice;
            do {
                cout << "\nCurrent illnesses:\n";
                for (size_t i = 0; i < illnessList.size(); i++) {
                    cout << i+1 << ". " << illnessList[i] << "\n";
                }
                cout << "\n1. Add illness\n2. Remove illness\n3. Finish editing\n";
                cout << "Enter your choice: ";
                cin >> choice;

                cin.ignore();

                if (choice == 1) {
                    cout << "Enter illness to add: ";
                    string newIll;
                    getline(cin, newIll);
                    illnessList.push_back(newIll);
                } else if (choice == 2) {
                    cout << "Enter illness number to remove: ";
                    int rmv;
                    cin >> rmv;
                    if (rmv >= 1 && rmv <= (int)illnessList.size()) {
                        illnessList.erase(illnessList.begin() + rmv - 1);
                    } else {
                        cout << "Invalid number.\n";
                    }
                }
                else if (choice == 3) {
                    cout << "Finishing editing...\n";
                    break;
                }
            } while (true);

            // Hastaliklari yeniden stringe cevir
            string newIllnesses;
            for (const auto& ill : illnessList) {
                newIllnesses += ill + ", ";
            }

            // Satiri dosyaya yaz
            tempFile << left << setw(15) << name
                     << setw(15) << lastname
                     << setw(5) << ageStr
                     << setw(8) << genderStr
                     << setw(10) << id
                     << setw(10) << blood
                     << setw(35) << newIllnesses << "\n";

        } else {
            tempFile << line << "\n";
        }
    }

    inFile.close();
    tempFile.close();

    if (found) {
        remove(filename.c_str());
        rename("temp.txt", filename.c_str());
        cout << "Illness information updated for patient ID " << targetID << ".\n";
    } else {
        remove("temp.txt");
        cout << "No patient with ID " << targetID << " found.\n";
    }
}

int main(){
    setlocale(LC_ALL, "Turkish");
    string file = "tracker.txt";
    writeHeaderIfEmpty(file); // Dosya bos ise baslik ekle
    int choice;

    while(1) {
        // Menu ekrani
        cout << "\nwelcome to disease tracker system!\n";
        cout << "choose one of the below\n";
        cout << "1.add new patient\n";
        cout << "2.search patient by ID\n";
        cout << "3.delete patient by ID\n";
        cout << "4.edit patient illnesses by ID\n";
        cout << "5.exit\n";
        cout << "enter your choice: ";
        cin >> choice;

        // Kullanici secimine gore islem yapilir
        switch (choice) {
        case 1:
            addPatient(file);
            break;
        case 2:
            findPatientByID(file);
            break;
        case 3:
            deletePatientByID(file);
            break;
        case 4:
            editPatientIllnesses(file);
            break;
        case 5:
            return 0;
        default:
            cout << "invalid number try again!";
            break;
        }
    }
}