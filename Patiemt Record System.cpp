#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
using namespace std;

int getNumber(string message)
{
    int number;

    while (true)
    {
        cout << message;
        cin >> number;

        if (cin.fail())
        {
            cout << "Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return number;
        }
    }
}

string getText(string message)
{
    string value;

    while (true)
    {
        cout << message;
        getline(cin, value);

        if (value.empty() || value.find('|') != string::npos)
        {
            cout << "Please enter valid information.\n";
        }
        else
        {
            return value;
        }
    }
}

class Patient
{
private:
    int patientID;
    string name;
    int age;
    string gender;
    string contact;

public:
    Patient()
    {
        patientID = 0;
        name = "";
        age = 0;
        gender = "";
        contact = "";
    }

    Patient(int id, string n, int a, string g, string c)
    {
        patientID = id;
        name = n;
        age = a;
        gender = g;
        contact = c;
    }

    int getPatientID() const
    {
        return patientID;
    }

    string getName() const
    {
        return name;
    }

    int getAge() const
    {
        return age;
    }

    string getGender() const
    {
        return gender;
    }

    string getContact() const
    {
        return contact;
    }

    void setDetails(string n, int a, string g, string c)
    {
        name = n;
        age = a;
        gender = g;
        contact = c;
    }

    void display() const
    {
        cout << "\nPatient ID: " << patientID;
        cout << "\nName: " << name;
        cout << "\nAge: " << age;
        cout << "\nGender: " << gender;
        cout << "\nContact: " << contact;
        cout << "\n";
    }
};

class Appointment
{
private:
    int appointmentID;
    int patientID;
    string date;
    string time;

public:
    Appointment()
    {
        appointmentID = 0;
        patientID = 0;
        date = "";
        time = "";
    }

    Appointment(int id, int pID, string d, string t)
    {
        appointmentID = id;
        patientID = pID;
        date = d;
        time = t;
    }

    int getAppointmentID() const
    {
        return appointmentID;
    }

    int getPatientID() const
    {
        return patientID;
    }

    string getDate() const
    {
        return date;
    }

    string getTime() const
    {
        return time;
    }

    void display() const
    {
        cout << "\nAppointment ID: " << appointmentID;
        cout << "\nPatient ID: " << patientID;
        cout << "\nDate: " << date;
        cout << "\nTime: " << time;
        cout << "\n";
    }
};

class PatientManager
{
private:
    vector<Patient> patients;

public:
    bool patientExists(int id) const
    {
        for (int i = 0; i < patients.size(); i++)
        {
            if (patients[i].getPatientID() == id)
            {
                return true;
            }
        }

        return false;
    }

    void addPatient()
    {
        int id = getNumber("\nEnter Patient ID: ");

        if (id <= 0 || patientExists(id))
        {
            cout << "\nInvalid or duplicate Patient ID.\n";
            return;
        }

        string name = getText("Enter Patient Name: ");

        int age = getNumber("Enter Age: ");

        if (age <= 0 || age > 120)
        {
            cout << "\nPlease enter a valid age.\n";
            return;
        }

        string gender = getText("Enter Gender: ");
        string contact = getText("Enter Contact Number: ");

        patients.push_back(Patient(id, name, age, gender, contact));

        cout << "\nPatient registered successfully!\n";
    }

    void viewPatients() const
    {
        if (patients.empty())
        {
            cout << "\nNo patient records available.\n";
            return;
        }

        cout << "\n========== ALL PATIENTS ==========\n";

        for (int i = 0; i < patients.size(); i++)
        {
            patients[i].display();
            cout << "----------------------------------\n";
        }
    }

    void searchPatient() const
    {
        int id = getNumber("\nEnter Patient ID to search: ");

        for (int i = 0; i < patients.size(); i++)
        {
            if (patients[i].getPatientID() == id)
            {
                cout << "\nPatient Found!\n";
                patients[i].display();
                return;
            }
        }

        cout << "\nPatient not found.\n";
    }

    void updatePatient()
    {
        int id = getNumber("\nEnter Patient ID to update: ");

        for (int i = 0; i < patients.size(); i++)
        {
            if (patients[i].getPatientID() == id)
            {
                string name = getText("Enter New Name: ");

                int age = getNumber("Enter New Age: ");

                if (age <= 0 || age > 120)
                {
                    cout << "\nInvalid age.\n";
                    return;
                }

                string gender = getText("Enter New Gender: ");
                string contact = getText("Enter New Contact Number: ");

                patients[i].setDetails(name, age, gender, contact);

                cout << "\nPatient updated successfully!\n";
                return;
            }
        }

        cout << "\nPatient not found.\n";
    }

    bool deletePatient()
    {
        int id = getNumber("\nEnter Patient ID to delete: ");

        for (int i = 0; i < patients.size(); i++)
        {
            if (patients[i].getPatientID() == id)
            {
                patients.erase(patients.begin() + i);

                cout << "\nPatient deleted successfully!\n";
                return true;
            }
        }

        cout << "\nPatient not found.\n";
        return false;
    }

    const vector<Patient>& getPatients() const
    {
        return patients;
    }

    void setPatients(const vector<Patient>& data)
    {
        patients = data;
    }
};

class AppointmentManager
{
private:
    vector<Appointment> appointments;

public:
    void addAppointment(const PatientManager& manager)
    {
        int id = getNumber("\nEnter Appointment ID: ");

        if (id <= 0)
        {
            cout << "\nInvalid Appointment ID.\n";
            return;
        }

        for (int i = 0; i < appointments.size(); i++)
        {
            if (appointments[i].getAppointmentID() == id)
            {
                cout << "\nAppointment ID already exists.\n";
                return;
            }
        }

        int patientID = getNumber("Enter Patient ID: ");

        if (!manager.patientExists(patientID))
        {
            cout << "\nPatient does not exist. Register first.\n";
            return;
        }

        string date = getText("Enter Appointment Date: ");
        string time = getText("Enter Appointment Time: ");

        appointments.push_back(Appointment(id, patientID, date, time));

        cout << "\nAppointment added successfully!\n";
    }

    void viewAppointments() const
    {
        if (appointments.empty())
        {
            cout << "\nNo appointments available.\n";
            return;
        }

        cout << "\n========== ALL APPOINTMENTS ==========\n";

        for (int i = 0; i < appointments.size(); i++)
        {
            appointments[i].display();
            cout << "--------------------------------------\n";
        }
    }

    void deletePatientAppointments(int patientID)
    {
        for (int i = 0; i < appointments.size(); i++)
        {
            if (appointments[i].getPatientID() == patientID)
            {
                appointments.erase(appointments.begin() + i);
                i--;
            }
        }
    }

    const vector<Appointment>& getAppointments() const
    {
        return appointments;
    }

    void setAppointments(const vector<Appointment>& data)
    {
        appointments = data;
    }
};

class FileHandler
{
public:
    bool savePatients(const vector<Patient>& patients)
    {
        ofstream file("patients.txt");

        if (!file)
        {
            return false;
        }

        for (int i = 0; i < patients.size(); i++)
        {
            file << patients[i].getPatientID() << "|"
                 << patients[i].getName() << "|"
                 << patients[i].getAge() << "|"
                 << patients[i].getGender() << "|"
                 << patients[i].getContact() << "\n";
        }

        file.close();
        return true;
    }

    bool loadPatients(vector<Patient>& patients)
    {
        ifstream file("patients.txt");

        if (!file)
        {
            return true;
        }

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string id, name, age, gender, contact;

            if (getline(ss, id, '|') &&
                getline(ss, name, '|') &&
                getline(ss, age, '|') &&
                getline(ss, gender, '|') &&
                getline(ss, contact))
            {
                try
                {
                    size_t p1, p2;

                    int patientID = stoi(id, &p1);
                    int patientAge = stoi(age, &p2);

                    if (p1 != id.size() || p2 != age.size() ||
                        patientID <= 0 || patientAge <= 0 ||
                        patientAge > 120 || name.empty() ||
                        gender.empty() || contact.empty())
                    {
                        continue;
                    }

                    bool duplicate = false;

                    for (int i = 0; i < patients.size(); i++)
                    {
                        if (patients[i].getPatientID() == patientID)
                        {
                            duplicate = true;
                            break;
                        }
                    }

                    if (!duplicate)
                    {
                        patients.push_back(Patient(
                            patientID, name, patientAge, gender, contact
                        ));
                    }
                }
                catch (...)
                {
                    continue;
                }
            }
        }

        file.close();
        return true;
    }

    bool saveAppointments(const vector<Appointment>& appointments)
    {
        ofstream file("appointments.txt");

        if (!file)
        {
            return false;
        }

        for (int i = 0; i < appointments.size(); i++)
        {
            file << appointments[i].getAppointmentID() << "|"
                 << appointments[i].getPatientID() << "|"
                 << appointments[i].getDate() << "|"
                 << appointments[i].getTime() << "\n";
        }

        file.close();
        return true;
    }

    bool loadAppointments(vector<Appointment>& appointments,
                          const vector<Patient>& patients)
    {
        ifstream file("appointments.txt");

        if (!file)
        {
            return true;
        }

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string id, patientID, date, time;

            if (getline(ss, id, '|') &&
                getline(ss, patientID, '|') &&
                getline(ss, date, '|') &&
                getline(ss, time))
            {
                try
                {
                    size_t p1, p2;

                    int appointmentID = stoi(id, &p1);
                    int pID = stoi(patientID, &p2);

                    if (p1 != id.size() || p2 != patientID.size() ||
                        appointmentID <= 0 || pID <= 0 ||
                        date.empty() || time.empty())
                    {
                        continue;
                    }

                    bool patientFound = false;
                    bool duplicate = false;

                    for (int i = 0; i < patients.size(); i++)
                    {
                        if (patients[i].getPatientID() == pID)
                        {
                            patientFound = true;
                            break;
                        }
                    }

                    for (int i = 0; i < appointments.size(); i++)
                    {
                        if (appointments[i].getAppointmentID() == appointmentID)
                        {
                            duplicate = true;
                            break;
                        }
                    }

                    if (patientFound && !duplicate)
                    {
                        appointments.push_back(Appointment(
                            appointmentID, pID, date, time
                        ));
                    }
                }
                catch (...)
                {
                    continue;
                }
            }
        }

        file.close();
        return true;
    }
};

int main()
{
    PatientManager patientManager;
    AppointmentManager appointmentManager;
    FileHandler fileHandler;

    vector<Patient> savedPatients;
    vector<Appointment> savedAppointments;

    fileHandler.loadPatients(savedPatients);

    patientManager.setPatients(savedPatients);

    fileHandler.loadAppointments(
        savedAppointments, patientManager.getPatients()
    );

    appointmentManager.setAppointments(savedAppointments);

    cout << "\nWelcome to PatientCare!\n";
    cout << "Patient records loaded: "
         << patientManager.getPatients().size() << endl;

    cout << "Appointments loaded: "
         << appointmentManager.getAppointments().size() << endl;

    int choice;

    do
    {
        cout << "\n========== PATIENTCARE ==========\n";
        cout << "1. Register New Patient\n";
        cout << "2. View All Patients\n";
        cout << "3. Search Patient\n";
        cout << "4. Update Patient\n";
        cout << "5. Delete Patient\n";
        cout << "6. Add Appointment\n";
        cout << "7. View Appointments\n";
        cout << "8. Exit\n";
        cout << "=================================\n";

        choice = getNumber("Enter your choice: ");

        switch (choice)
        {
            case 1:
                patientManager.addPatient();
                break;

            case 2:
                patientManager.viewPatients();
                break;

            case 3:
                patientManager.searchPatient();
                break;

            case 4:
                patientManager.updatePatient();
                break;

            case 5:
            {
                int id = getNumber("\nEnter Patient ID to delete: ");

                if (patientManager.patientExists(id))
                {
                    if (patientManager.deletePatient())
                    {
                        appointmentManager.deletePatientAppointments(id);
                    }
                }
                else
                {
                    cout << "\nPatient not found.\n";
                }

                break;
            }

            case 6:
                appointmentManager.addAppointment(patientManager);
                break;

            case 7:
                appointmentManager.viewAppointments();
                break;

            case 8:
                cout << "\nThank you for using PatientCare!\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

        if (choice >= 1 && choice <= 7)
        {
            bool patientSaved = fileHandler.savePatients(
                patientManager.getPatients()
            );

            bool appointmentSaved = fileHandler.saveAppointments(
                appointmentManager.getAppointments()
            );

            if (!patientSaved || !appointmentSaved)
            {
                cout << "\nUnable to save records. Please check file permissions.\n";
            }
        }

    } while (choice != 8);

    fileHandler.savePatients(patientManager.getPatients());

    fileHandler.saveAppointments(
        appointmentManager.getAppointments()
    );

    return 0;
}
