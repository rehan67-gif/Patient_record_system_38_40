#include <iostream>
#include <string>
using namespace std;

struct Patient
{
    int patientID;
    string name;
    int age;
    string gender;
    string contact;
};

struct Appointment
{
    int appointmentID;
    int patientID;
    string date;
    string time;
};

Patient patients[100];
Appointment appointments[100];

int patientCount = 0;
int appointmentCount = 0;

int findPatient(int id)
{
    for (int i = 0; i < patientCount; i++)
    {
        if (patients[i].patientID == id)
        {
            return i;
        }
    }

    return -1;
}

void registerPatient()
{
    if (patientCount >= 100)
    {
        cout << "\nPatient limit reached.\n";
        return;
    }

    int id;

    cout << "\nEnter Patient ID: ";
    cin >> id;

    if (findPatient(id) != -1)
    {
        cout << "\nPatient ID already exists.\n";
        return;
    }

    patients[patientCount].patientID = id;

    cin.ignore();

    cout << "Enter Patient Name: ";
    getline(cin, patients[patientCount].name);

    cout << "Enter Age: ";
    cin >> patients[patientCount].age;

    cout << "Enter Gender: ";
    cin >> patients[patientCount].gender;

    cout << "Enter Contact Number: ";
    cin >> patients[patientCount].contact;

    patientCount++;

    cout << "\nPatient registered successfully!\n";
}

void viewPatients()
{
    if (patientCount == 0)
    {
        cout << "\nNo patient records available.\n";
        return;
    }

    cout << "\n========== ALL PATIENTS ==========\n";

    for (int i = 0; i < patientCount; i++)
    {
        cout << "\nPatient ID: " << patients[i].patientID;
        cout << "\nName: " << patients[i].name;
        cout << "\nAge: " << patients[i].age;
        cout << "\nGender: " << patients[i].gender;
        cout << "\nContact: " << patients[i].contact;
        cout << "\n----------------------------------\n";
    }
}

void searchPatient()
{
    int id;

    cout << "\nEnter Patient ID to search: ";
    cin >> id;

    int index = findPatient(id);

    if (index == -1)
    {
        cout << "\nPatient not found.\n";
    }
    else
    {
        cout << "\n========== PATIENT DETAILS ==========\n";

        cout << "Patient ID: " << patients[index].patientID << endl;
        cout << "Name: " << patients[index].name << endl;
        cout << "Age: " << patients[index].age << endl;
        cout << "Gender: " << patients[index].gender << endl;
        cout << "Contact: " << patients[index].contact << endl;
    }
}

void updatePatient()
{
    int id;

    cout << "\nEnter Patient ID to update: ";
    cin >> id;

    int index = findPatient(id);

    if (index == -1)
    {
        cout << "\nPatient not found.\n";
        return;
    }

    cin.ignore();

    cout << "\nEnter New Name: ";
    getline(cin, patients[index].name);

    cout << "Enter New Age: ";
    cin >> patients[index].age;

    cout << "Enter New Gender: ";
    cin >> patients[index].gender;

    cout << "Enter New Contact Number: ";
    cin >> patients[index].contact;

    cout << "\nPatient record updated successfully!\n";
}

void deletePatient()
{
    int id;

    cout << "\nEnter Patient ID to delete: ";
    cin >> id;

    int index = findPatient(id);

    if (index == -1)
    {
        cout << "\nPatient not found.\n";
        return;
    }

    for (int i = index; i < patientCount - 1; i++)
    {
        patients[i] = patients[i + 1];
    }

    patientCount--;

    for (int i = 0; i < appointmentCount; i++)
    {
        if (appointments[i].patientID == id)
        {
            for (int j = i; j < appointmentCount - 1; j++)
            {
                appointments[j] = appointments[j + 1];
            }

            appointmentCount--;
            i--;
        }
    }

    cout << "\nPatient record deleted successfully!\n";
}

void addAppointment()
{
    if (appointmentCount >= 100)
    {
        cout << "\nAppointment limit reached.\n";
        return;
    }

    int patientID;

    cout << "\nEnter Appointment ID: ";
    cin >> appointments[appointmentCount].appointmentID;

    for (int i = 0; i < appointmentCount; i++)
    {
        if (appointments[i].appointmentID == appointments[appointmentCount].appointmentID)
        {
            cout << "\nAppointment ID already exists.\n";
            return;
        }
    }

    cout << "Enter Patient ID: ";
    cin >> patientID;

    if (findPatient(patientID) == -1)
    {
        cout << "\nPatient does not exist. Please register the patient first.\n";
        return;
    }

    appointments[appointmentCount].patientID = patientID;

    cin.ignore();

    cout << "Enter Appointment Date: ";
    getline(cin, appointments[appointmentCount].date);

    cout << "Enter Appointment Time: ";
    getline(cin, appointments[appointmentCount].time);

    appointmentCount++;

    cout << "\nAppointment added successfully!\n";
}

void viewAppointments()
{
    if (appointmentCount == 0)
    {
        cout << "\nNo appointments available.\n";
        return;
    }

    cout << "\n========== ALL APPOINTMENTS ==========\n";

    for (int i = 0; i < appointmentCount; i++)
    {
        cout << "\nAppointment ID: " << appointments[i].appointmentID;
        cout << "\nPatient ID: " << appointments[i].patientID;

        int index = findPatient(appointments[i].patientID);

        if (index != -1)
        {
            cout << "\nPatient Name: " << patients[index].name;
        }

        cout << "\nDate: " << appointments[i].date;
        cout << "\nTime: " << appointments[i].time;
        cout << "\n--------------------------------------\n";
    }
}

int main()
{
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

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                registerPatient();
                break;

            case 2:
                viewPatients();
                break;

            case 3:
                searchPatient();
                break;

            case 4:
                updatePatient();
                break;

            case 5:
                deletePatient();
                break;

            case 6:
                addAppointment();
                break;

            case 7:
                viewAppointments();
                break;

            case 8:
                cout << "\nThank you for using PatientCare!\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 8);

    return 0;
}