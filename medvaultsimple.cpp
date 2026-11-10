/*
    MEDVAULT - Simple Hospital Management System
    OOPS Mini Project | C++17

    Unique feature: Emergency Priority Override
    Emergency appointments appear before routine appointments.

    Compile: g++ -std=c++17 main.cpp -o medvault
    Run:     ./medvault  (Windows: medvault.exe)

    Data files are created automatically in the same folder.
*/

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// --------------------------- Simple input helpers ---------------------------

string input(string message) {
    cout << message;
    string value;
    getline(cin, value);
    replace(value.begin(), value.end(), '|', '/');
    return value;
}

int number(string message, int minimum, int maximum) {
    int value;
    while (true) {
        cout << message;
        if (cin >> value && value >= minimum && value <= maximum) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Please enter a number from " << minimum
             << " to " << maximum << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

vector<string> split(string line) {
    vector<string> parts;
    string part;
    stringstream stream(line);
    while (getline(stream, part, '|')) parts.push_back(part);
    return parts;
}

void header(string title) {
    cout << "\n============================================\n";
    cout << "                 MEDVAULT\n";
    cout << "       Simple Hospital Management\n";
    cout << "--------------------------------------------\n";
    cout << title << "\n";
    cout << "============================================\n";
}

// -------------------------------- Person ------------------------------------

class Person {
protected:
    string id, name, phone;
    int age;

public:
    Person(string i, string n, int a, string p)
        : id(i), name(n), phone(p), age(a) {}

    virtual ~Person() {}
    string getId() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getPhone() const { return phone; }
    virtual void display() const = 0; // Abstraction
};

// -------------------------------- Patient -----------------------------------

class Patient : public Person {
protected:
    string gender;

public:
    Patient(string i, string n, int a, string p, string g)
        : Person(i, n, a, p), gender(g) {}

    string getGender() const { return gender; }

    void display() const override {
        cout << left << setw(8) << id << setw(20) << name
             << setw(7) << age << setw(16) << phone << gender << '\n';
    }
};

// ---------------------------- Emergency Patient -----------------------------

class EmergencyPatient : public Patient {
public:
    EmergencyPatient(string i, string n, int a, string p, string g)
        : Patient(i, n, a, p, g) {}

    void display() const override {
        Patient::display();
        cout << "  [EMERGENCY PRIORITY]\n";
    }
};

// -------------------------------- Doctor ------------------------------------

class Doctor : public Person {
    string specialization;

public:
    Doctor(string i, string n, int a, string p, string s)
        : Person(i, n, a, p), specialization(s) {}

    void display() const override {
        cout << left << setw(8) << id << setw(20) << name
             << setw(7) << age << setw(16) << phone
             << specialization << '\n';
    }
};

// ------------------------------- Appointment --------------------------------

class Appointment {
public:
    string id, patientId, doctorId, date;
    bool emergency;

    Appointment(string i, string p, string d, string dt, bool e)
        : id(i), patientId(p), doctorId(d), date(dt), emergency(e) {}

    void display() const {
        cout << left << setw(8) << id << setw(8) << patientId
             << setw(8) << doctorId << setw(16) << date
             << (emergency ? "EMERGENCY" : "Routine") << '\n';
    }
};

// ------------------------------ Hospital ------------------------------------

class Hospital {
    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Appointment> appointments;

    int nextPatient = 1, nextDoctor = 1, nextAppointment = 1;

    bool hasPatient(string id) const {
        for (const auto& p : patients)
            if (p.getId() == id) return true;
        return false;
    }

    bool hasDoctor(string id) const {
        for (const auto& d : doctors)
            if (d.getId() == id) return true;
        return false;
    }

    void save() const {
        ofstream pf("patients.txt");
        for (const auto& p : patients)
            pf << p.getId() << '|' << p.getName() << '|'
               << p.getAge() << '|' << p.getPhone() << '|'
               << p.getGender() << '\n';

        ofstream df("doctors.txt");
        for (const auto& d : doctors)
            df << d.getId() << '|' << d.getName() << '|'
               << d.getAge() << '|' << d.getPhone() << '\n';

        ofstream af("appointments.txt");
        for (const auto& a : appointments)
            af << a.id << '|' << a.patientId << '|'
               << a.doctorId << '|' << a.date << '|'
               << a.emergency << '\n';
    }

    void load() {
        string line;
        ifstream pf("patients.txt");
        while (getline(pf, line)) {
            auto f = split(line);
            if (f.size() == 5) {
                try {
                    patients.emplace_back(f[0], f[1], stoi(f[2]), f[3], f[4]);
                    nextPatient = max(nextPatient, stoi(f[0].substr(1)) + 1);
                } catch (...) {}
            }
        }

        ifstream df("doctors.txt");
        while (getline(df, line)) {
            auto f = split(line);
            if (f.size() == 5) {
                try {
                    doctors.emplace_back(f[0], f[1], stoi(f[2]), f[3], f[4]);
                    nextDoctor = max(nextDoctor, stoi(f[0].substr(1)) + 1);
                } catch (...) {}
            }
        }

        ifstream af("appointments.txt");
        while (getline(af, line)) {
            auto f = split(line);
            if (f.size() == 5) {
                try {
                    appointments.emplace_back(f[0], f[1], f[2], f[3],
                                              stoi(f[4]) == 1);
                    nextAppointment = max(nextAppointment,
                                           stoi(f[0].substr(1)) + 1);
                } catch (...) {}
            }
        }
    }

public:
    Hospital() { load(); }

    void registerPatient() {
        header("REGISTER PATIENT");
        string name = input("Name: ");
        int age = number("Age: ", 1, 120);
        string phone = input("Phone: ");
        string gender = input("Gender: ");

        string id = "P" + to_string(nextPatient++);
        patients.emplace_back(id, name, age, phone, gender);
        save();
        cout << "\nPatient added. Give this ID to the patient: " << id << '\n';
    }

    void registerDoctor() {
        header("REGISTER DOCTOR");
        string name = input("Doctor name: ");
        int age = number("Age: ", 21, 100);
        string phone = input("Phone: ");
        string specialization = input("Specialization: ");

        string id = "D" + to_string(nextDoctor++);
        doctors.emplace_back(id, name, age, phone, specialization);
        save();
        cout << "\nDoctor added. Doctor ID: " << id << '\n';
    }

    void showPatients() const {
        header("PATIENT LIST");
        if (patients.empty()) {
            cout << "No patients registered.\n";
            return;
        }
        cout << left << setw(8) << "ID" << setw(20) << "Name"
             << setw(7) << "Age" << setw(16) << "Phone" << "Gender\n";
        cout << "------------------------------------------------------------\n";
        for (const auto& p : patients) p.display();
    }

    void showDoctors() const {
        header("DOCTOR LIST");
        if (doctors.empty()) {
            cout << "No doctors registered.\n";
            return;
        }
        cout << left << setw(8) << "ID" << setw(20) << "Name"
             << setw(7) << "Age" << setw(16) << "Phone"
             << "Specialization\n";
        cout << "------------------------------------------------------------\n";
        for (const auto& d : doctors) d.display();
    }

    void bookAppointment() {
        header("BOOK APPOINTMENT");
        if (patients.empty() || doctors.empty()) {
            cout << "First register at least one patient and one doctor.\n";
            return;
        }

        string patientId = input("Patient ID (example P1): ");
        if (!hasPatient(patientId)) {
            cout << "Patient not found. Check the ID and try again.\n";
            return;
        }

        string doctorId = input("Doctor ID (example D1): ");
        if (!hasDoctor(doctorId)) {
            cout << "Doctor not found. Check the ID and try again.\n";
            return;
        }

        string date = input("Appointment date (DD-MM-YYYY): ");
        cout << "\n1. Routine appointment\n2. Emergency appointment\n";
        int type = number("Choose: ", 1, 2);
        bool emergency = (type == 2);

        string id = "A" + to_string(nextAppointment++);
        appointments.emplace_back(id, patientId, doctorId, date, emergency);
        save();

        cout << "\nAppointment booked. Appointment ID: " << id << '\n';
        if (emergency)
            cout << "Emergency Priority Override activated!\n";
    }

    void showAppointments() const {
        header("APPOINTMENT QUEUE");
        if (appointments.empty()) {
            cout << "No appointments booked.\n";
            return;
        }

        // Emergency appointments are sorted to the top.
        vector<Appointment> queue = appointments;
        stable_sort(queue.begin(), queue.end(),
            [](const Appointment& a, const Appointment& b) {
                return a.emergency > b.emergency;
            });

        cout << "Emergency patients are shown before routine patients.\n\n";
        cout << left << setw(8) << "App ID" << setw(8) << "Patient"
             << setw(8) << "Doctor" << setw(16) << "Date" << "Priority\n";
        cout << "------------------------------------------------------------\n";
        for (const auto& a : queue) a.display();
    }

    void dashboard() const {
        header("HOSPITAL SUMMARY");
        int emergency = 0;
        for (const auto& a : appointments)
            if (a.emergency) emergency++;

        cout << "Registered patients       : " << patients.size() << '\n';
        cout << "Registered doctors        : " << doctors.size() << '\n';
        cout << "Total appointments        : " << appointments.size() << '\n';
        cout << "Emergency appointments    : " << emergency << '\n';
        cout << "Routine appointments      : "
             << appointments.size() - emergency << '\n';
    }

    void run() {
        int choice;
        do {
            header("MAIN MENU");
            cout << "1. Register patient\n";
            cout << "2. Register doctor\n";
            cout << "3. View patients and doctors\n";
            cout << "4. Book appointment\n";
            cout << "5. View appointment queue\n";
            cout << "6. Hospital summary\n";
            cout << "0. Save and exit\n\n";

            choice = number("Enter choice: ", 0, 6);

            switch (choice) {
                case 1: registerPatient(); break;
                case 2: registerDoctor(); break;
                case 3:
                    showPatients();
                    showDoctors();
                    break;
                case 4: bookAppointment(); break;
                case 5: showAppointments(); break;
                case 6: dashboard(); break;
                case 0:
                    save();
                    cout << "Records saved. Thank you for using MedVault!\n";
                    break;
            }

            if (choice != 0) {
                cout << "\nPress Enter to return to menu...";
                string pause;
                getline(cin, pause);
            }
        } while (choice != 0);
    }
};

// ---------------------------------- Main ------------------------------------

int main() {
    Hospital medvault;
    medvault.run();
    return 0;
}
