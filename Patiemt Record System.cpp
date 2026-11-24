#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <regex>
#include <sqlite3.h>

using namespace std;

struct Patient {
    int id;
    string name;
    int age;
    string gender;
    string contact;
};

struct Appointment {
    int id;
    int patientId;
    string patientName;
    string date;
    string time;
    string status;
};

struct User {
    string username;
    string role;
};

class Database {
private:
    sqlite3* db = nullptr;

public:
    Database(const string& filename) {
        if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
            cout << "Database connection failed.\n";
        }

        if (db) {
            sqlite3_busy_timeout(db, 3000);
        }
    }

    ~Database() {
        if (db) {
            sqlite3_close(db);
        }
    }

    sqlite3* getConnection() {
        return db;
    }

    bool execute(const string& sql) {
        char* errorMessage = nullptr;

        int result = sqlite3_exec(
            db,
            sql.c_str(),
            nullptr,
            nullptr,
            &errorMessage
        );

        if (result != SQLITE_OK) {
            cout << "Database Error: "
                 << (errorMessage ? errorMessage : "Unknown error")
                 << endl;

            sqlite3_free(errorMessage);
            return false;
        }

        return true;
    }

    bool initialize() {
        if (!db) {
            return false;
        }

        execute("PRAGMA foreign_keys = ON;");

        string sql =
            "CREATE TABLE IF NOT EXISTS patients ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "age INTEGER NOT NULL CHECK(age BETWEEN 1 AND 120),"
            "gender TEXT NOT NULL,"
            "contact TEXT NOT NULL UNIQUE"
            ");"

            "CREATE TABLE IF NOT EXISTS appointments ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "patient_id INTEGER NOT NULL,"
            "date TEXT NOT NULL,"
            "time TEXT NOT NULL,"
            "status TEXT NOT NULL DEFAULT 'Scheduled',"
            "FOREIGN KEY(patient_id) REFERENCES patients(id) ON DELETE CASCADE"
            ");"

            "CREATE TABLE IF NOT EXISTS users ("
            "username TEXT PRIMARY KEY,"
            "password TEXT NOT NULL,"
            "role TEXT NOT NULL"
            ");";

        if (!execute(sql)) {
            return false;
        }

        sqlite3_stmt* statement;

        sqlite3_prepare_v2(
            db,
            "SELECT COUNT(*) FROM users",
            -1,
            &statement,
            nullptr
        );

        int count = 0;

        if (sqlite3_step(statement) == SQLITE_ROW) {
            count = sqlite3_column_int(statement, 0);
        }

        sqlite3_finalize(statement);

        if (count == 0) {
            execute(
                "INSERT INTO users VALUES"
                "('admin','admin123','Admin'),"
                "('reception','recep123','Receptionist'),"
                "('viewer','view123','Viewer');"
            );
        }

        return true;
    }

    bool backup(const string& filename) {
        sqlite3* backupDB = nullptr;

        if (sqlite3_open(filename.c_str(), &backupDB) != SQLITE_OK) {
            if (backupDB) {
                sqlite3_close(backupDB);
            }
            return false;
        }

        sqlite3_backup* backupHandle =
            sqlite3_backup_init(backupDB, "main", db, "main");

        if (!backupHandle) {
            sqlite3_close(backupDB);
            return false;
        }

        int result = sqlite3_backup_step(backupHandle, -1);

        sqlite3_backup_finish(backupHandle);
        sqlite3_close(backupDB);

        return result == SQLITE_DONE;
    }
};

class Auth {
private:
    Database& database;

public:
    Auth(Database& db) : database(db) {}

    bool login(User& user) {
        string username;
        string password;

        cout << "\n========== PATIENTCARE LOGIN ==========\n";

        cout << "Username: ";
        getline(cin, username);

        cout << "Password: ";
        getline(cin, password);

        sqlite3_stmt* statement;

        string sql =
            "SELECT username, role FROM users "
            "WHERE username=? AND password=?";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        sqlite3_bind_text(
            statement, 1, username.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement, 2, password.c_str(), -1, SQLITE_TRANSIENT
        );

        bool success = false;

        if (sqlite3_step(statement) == SQLITE_ROW) {
            user.username =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 0)
                );

            user.role =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 1)
                );

            success = true;
        }

        sqlite3_finalize(statement);

        if (!success) {
            cout << "Invalid username or password.\n";
        }

        return success;
    }
};

class PatientManager {
private:
    Database& database;

public:
    PatientManager(Database& db) : database(db) {}

    bool addPatient(const Patient& patient) {
        sqlite3_stmt* statement;

        string sql =
            "INSERT INTO patients(name, age, gender, contact) "
            "VALUES(?,?,?,?)";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        sqlite3_bind_text(
            statement, 1, patient.name.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_int(statement, 2, patient.age);

        sqlite3_bind_text(
            statement, 3, patient.gender.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement, 4, patient.contact.c_str(), -1, SQLITE_TRANSIENT
        );

        bool success = sqlite3_step(statement) == SQLITE_DONE;

        sqlite3_finalize(statement);

        if (!success) {
            cout << "Unable to register patient. Contact may already exist.\n";
        }

        return success;
    }

    bool getPatient(int id, Patient& patient) {
        sqlite3_stmt* statement;

        string sql =
            "SELECT id, name, age, gender, contact "
            "FROM patients WHERE id=?";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        sqlite3_bind_int(statement, 1, id);

        bool found = false;

        if (sqlite3_step(statement) == SQLITE_ROW) {
            patient.id = sqlite3_column_int(statement, 0);

            patient.name =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 1)
                );

            patient.age = sqlite3_column_int(statement, 2);

            patient.gender =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 3)
                );

            patient.contact =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 4)
                );

            found = true;
        }

        sqlite3_finalize(statement);

        return found;
    }

    vector<Patient> getAllPatients(string sortBy = "id") {
        vector<Patient> patients;

        string sql =
            "SELECT id, name, age, gender, contact "
            "FROM patients ORDER BY ";

        sql += (sortBy == "name")
            ? "name COLLATE NOCASE"
            : "id";

        sqlite3_stmt* statement;

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        while (sqlite3_step(statement) == SQLITE_ROW) {
            Patient patient;

            patient.id = sqlite3_column_int(statement, 0);

            patient.name =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 1)
                );

            patient.age = sqlite3_column_int(statement, 2);

            patient.gender =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 3)
                );

            patient.contact =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 4)
                );

            patients.push_back(patient);
        }

        sqlite3_finalize(statement);

        return patients;
    }

    vector<Patient> searchPatients(const string& searchTerm) {
        vector<Patient> patients;

        sqlite3_stmt* statement;

        string sql =
            "SELECT id, name, age, gender, contact FROM patients "
            "WHERE CAST(id AS TEXT)=? OR name LIKE ? OR contact LIKE ? "
            "ORDER BY id";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        string pattern = "%" + searchTerm + "%";

        sqlite3_bind_text(
            statement, 1, searchTerm.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement, 2, pattern.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement, 3, pattern.c_str(), -1, SQLITE_TRANSIENT
        );

        while (sqlite3_step(statement) == SQLITE_ROW) {
            Patient patient;

            patient.id = sqlite3_column_int(statement, 0);

            patient.name =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 1)
                );

            patient.age = sqlite3_column_int(statement, 2);

            patient.gender =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 3)
                );

            patient.contact =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 4)
                );

            patients.push_back(patient);
        }

        sqlite3_finalize(statement);

        return patients;
    }

    bool updatePatient(int id, const Patient& patient) {
        sqlite3_stmt* statement;

        string sql =
            "UPDATE patients SET name=?, age=?, gender=?, contact=? "
            "WHERE id=?";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        sqlite3_bind_text(
            statement, 1, patient.name.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_int(statement, 2, patient.age);

        sqlite3_bind_text(
            statement, 3, patient.gender.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement, 4, patient.contact.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_int(statement, 5, id);

        bool success =
            sqlite3_step(statement) == SQLITE_DONE &&
            sqlite3_changes(database.getConnection()) > 0;

        sqlite3_finalize(statement);

        return success;
    }

    bool deletePatient(int id) {
        sqlite3_stmt* statement;

        sqlite3_prepare_v2(
            database.getConnection(),
            "DELETE FROM patients WHERE id=?",
            -1,
            &statement,
            nullptr
        );

        sqlite3_bind_int(statement, 1, id);

        bool success =
            sqlite3_step(statement) == SQLITE_DONE &&
            sqlite3_changes(database.getConnection()) > 0;

        sqlite3_finalize(statement);

        return success;
    }
};

class AppointmentManager {
private:
    Database& database;

public:
    AppointmentManager(Database& db) : database(db) {}

    bool addAppointment(
        int patientId,
        const string& date,
        const string& time
    ) {
        sqlite3_stmt* check;

        string checkSQL =
            "SELECT COUNT(*) FROM appointments "
            "WHERE date=? AND time=? AND status='Scheduled'";

        sqlite3_prepare_v2(
            database.getConnection(),
            checkSQL.c_str(),
            -1,
            &check,
            nullptr
        );

        sqlite3_bind_text(
            check, 1, date.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            check, 2, time.c_str(), -1, SQLITE_TRANSIENT
        );

        bool conflict =
            sqlite3_step(check) == SQLITE_ROW &&
            sqlite3_column_int(check, 0) > 0;

        sqlite3_finalize(check);

        if (conflict) {
            cout << "This appointment time is already booked.\n";
            return false;
        }

        sqlite3_stmt* statement;

        string sql =
            "INSERT INTO appointments(patient_id, date, time) "
            "VALUES(?,?,?)";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        sqlite3_bind_int(statement, 1, patientId);

        sqlite3_bind_text(
            statement, 2, date.c_str(), -1, SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement, 3, time.c_str(), -1, SQLITE_TRANSIENT
        );

        bool success = sqlite3_step(statement) == SQLITE_DONE;

        sqlite3_finalize(statement);

        return success;
    }

    bool cancelAppointment(int id) {
        sqlite3_stmt* statement;

        string sql =
            "UPDATE appointments SET status='Cancelled' "
            "WHERE id=? AND status='Scheduled'";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        sqlite3_bind_int(statement, 1, id);

        bool success =
            sqlite3_step(statement) == SQLITE_DONE &&
            sqlite3_changes(database.getConnection()) > 0;

        sqlite3_finalize(statement);

        return success;
    }

    vector<Appointment> getAppointments(string date = "") {
        vector<Appointment> appointments;

        string sql =
            "SELECT a.id, a.patient_id, p.name, a.date, a.time, a.status "
            "FROM appointments a "
            "JOIN patients p ON a.patient_id=p.id ";

        if (!date.empty()) {
            sql += "WHERE a.date=? ";
        }

        sql += "ORDER BY a.date, a.time";

        sqlite3_stmt* statement;

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        if (!date.empty()) {
            sqlite3_bind_text(
                statement, 1, date.c_str(), -1, SQLITE_TRANSIENT
            );
        }

        while (sqlite3_step(statement) == SQLITE_ROW) {
            Appointment appointment;

            appointment.id = sqlite3_column_int(statement, 0);
            appointment.patientId = sqlite3_column_int(statement, 1);

            appointment.patientName =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 2)
                );

            appointment.date =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 3)
                );

            appointment.time =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 4)
                );

            appointment.status =
                reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, 5)
                );

            appointments.push_back(appointment);
        }

        sqlite3_finalize(statement);

        return appointments;
    }

    void showDashboard() {
        sqlite3_stmt* statement;

        string sql =
            "SELECT "
            "(SELECT COUNT(*) FROM patients),"
            "(SELECT COUNT(*) FROM appointments),"
            "(SELECT COUNT(*) FROM appointments "
            "WHERE status='Scheduled' AND date=date('now','localtime')),"
            "(SELECT COUNT(*) FROM appointments "
            "WHERE status='Scheduled' AND date>date('now','localtime')),"
            "(SELECT COUNT(*) FROM appointments "
            "WHERE status='Cancelled')";

        sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement,
            nullptr
        );

        if (sqlite3_step(statement) == SQLITE_ROW) {
            cout << "\n========== DASHBOARD ==========\n";

            cout << "Total Patients: "
                 << sqlite3_column_int(statement, 0) << endl;

            cout << "Total Appointments: "
                 << sqlite3_column_int(statement, 1) << endl;

            cout << "Today's Appointments: "
                 << sqlite3_column_int(statement, 2) << endl;

            cout << "Upcoming Appointments: "
                 << sqlite3_column_int(statement, 3) << endl;

            cout << "Cancelled Appointments: "
                 << sqlite3_column_int(statement, 4) << endl;
        }

        sqlite3_finalize(statement);
    }
};

int readInt(const string& message) {
    int value;

    while (true) {
        cout << message;

        if (cin >> value) {
            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            return value;
        }

        cout << "Please enter a valid number.\n";

        cin.clear();

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );
    }
}

string readText(const string& message) {
    string value;

    cout << message;
    getline(cin, value);

    return value;
}

bool validDate(const string& date) {
    return regex_match(
        date,
        regex("\\d{4}-\\d{2}-\\d{2}")
    );
}

bool validTime(const string& time) {
    if (!regex_match(time, regex("\\d{2}:\\d{2}"))) {
        return false;
    }

    int hour = stoi(time.substr(0, 2));
    int minute = stoi(time.substr(3, 2));

    return hour >= 0 && hour <= 23 &&
           minute >= 0 && minute <= 59;
}

void viewPatients(const vector<Patient>& patients) {
    if (patients.empty()) {
        cout << "No patient records found.\n";
        return;
    }

    cout << "\nID\tName\t\tAge\tGender\tContact\n";

    cout << "--------------------------------------------------------\n";

    for (const auto& patient : patients) {
        cout << patient.id << "\t"
             << patient.name << "\t\t"
             << patient.age << "\t"
             << patient.gender << "\t"
             << patient.contact << endl;
    }
}

void viewAppointments(const vector<Appointment>& appointments) {
    if (appointments.empty()) {
        cout << "No appointments found.\n";
        return;
    }

    cout << "\nAppointment ID\tPatient ID\tPatient Name\tDate\t\tTime\tStatus\n";

    cout << "--------------------------------------------------------------------------\n";

    for (const auto& appointment : appointments) {
        cout << appointment.id << "\t\t"
             << appointment.patientId << "\t\t"
             << appointment.patientName << "\t\t"
             << appointment.date << "\t"
             << appointment.time << "\t"
             << appointment.status << endl;
    }
}

bool canWrite(const User& user) {
    return user.role == "Admin" ||
           user.role == "Receptionist";
}

int main() {
    Database database("patientcare.db");

    if (!database.initialize()) {
        cout << "Unable to initialize database.\n";
        return 1;
    }

    Auth auth(database);

    User user;

    if (!auth.login(user)) {
        return 0;
    }

    PatientManager patientManager(database);

    AppointmentManager appointmentManager(database);

    cout << "\nWelcome, " << user.username
         << " (" << user.role << ")\n";

    while (true) {
        cout << "\n========== PATIENTCARE VERSION 3 ==========\n";

        cout << "1. Register New Patient\n";
        cout << "2. View All Patients\n";
        cout << "3. Search Patient\n";
        cout << "4. Update Patient\n";
        cout << "5. Delete Patient\n";
        cout << "6. Add Appointment\n";
        cout << "7. View Appointments\n";
        cout << "8. Cancel Appointment\n";
        cout << "9. Dashboard and Reports\n";
        cout << "10. Sort Patients\n";
        cout << "11. Backup Database\n";
        cout << "0. Exit\n";

        int choice = readInt("Enter your choice: ");

        if (choice == 0) {
            cout << "Thank you for using PatientCare.\n";
            break;
        }

        switch (choice) {

        case 1: {
            if (!canWrite(user)) {
                cout << "Permission denied.\n";
                break;
            }

            Patient patient{};

            patient.name = readText("Enter patient name: ");
            patient.age = readInt("Enter age: ");
            patient.gender = readText("Enter gender: ");
            patient.contact = readText("Enter contact number: ");

            if (patient.name.empty() ||
                patient.gender.empty() ||
                patient.contact.empty() ||
                patient.age < 1 ||
                patient.age > 120) {

                cout << "Invalid patient details.\n";
            }
            else if (patientManager.addPatient(patient)) {
                cout << "Patient registered successfully.\n";
            }

            break;
        }

        case 2: {
            viewPatients(patientManager.getAllPatients());
            break;
        }

        case 3: {
            string term = readText(
                "Enter patient ID, name, or contact number: "
            );

            viewPatients(patientManager.searchPatients(term));
            break;
        }

        case 4: {
            if (!canWrite(user)) {
                cout << "Permission denied.\n";
                break;
            }

            int id = readInt("Enter patient ID to update: ");

            Patient patient{};

            if (!patientManager.getPatient(id, patient)) {
                cout << "Patient not found.\n";
                break;
            }

            patient.name = readText("Enter new name: ");
            patient.age = readInt("Enter new age: ");
            patient.gender = readText("Enter new gender: ");
            patient.contact = readText("Enter new contact: ");

            if (patient.name.empty() ||
                patient.gender.empty() ||
                patient.contact.empty() ||
                patient.age < 1 ||
                patient.age > 120) {

                cout << "Invalid patient details.\n";
            }
            else if (patientManager.updatePatient(id, patient)) {
                cout << "Patient updated successfully.\n";
            }
            else {
                cout << "Patient update failed.\n";
            }

            break;
        }

        case 5: {
            if (user.role != "Admin") {
                cout << "Only Admin can delete patient records.\n";
                break;
            }

            int id = readInt("Enter patient ID to delete: ");

            string confirmation = readText(
                "Type YES to confirm deletion: "
            );

            if (confirmation == "YES" &&
                patientManager.deletePatient(id)) {

                cout << "Patient and linked appointments deleted.\n";
            }
            else {
                cout << "Deletion cancelled or patient not found.\n";
            }

            break;
        }

        case 6: {
            if (!canWrite(user)) {
                cout << "Permission denied.\n";
                break;
            }

            int patientId = readInt("Enter patient ID: ");

            Patient patient{};

            if (!patientManager.getPatient(patientId, patient)) {
                cout << "Patient not found. Register first.\n";
                break;
            }

            string date = readText(
                "Enter date (YYYY-MM-DD): "
            );

            string time = readText(
                "Enter time (HH:MM): "
            );

            if (!validDate(date) || !validTime(time)) {
                cout << "Invalid date or time format.\n";
            }
            else if (
                appointmentManager.addAppointment(
                    patientId, date, time
                )
            ) {
                cout << "Appointment added successfully.\n";
            }
            else {
                cout << "Unable to add appointment.\n";
            }

            break;
        }

        case 7: {
            string date = readText(
                "Enter date (YYYY-MM-DD) or press Enter for all: "
            );

            if (!date.empty() && !validDate(date)) {
                cout << "Invalid date format.\n";
                break;
            }

            viewAppointments(
                appointmentManager.getAppointments(date)
            );

            break;
        }

        case 8: {
            if (!canWrite(user)) {
                cout << "Permission denied.\n";
                break;
            }

            int id = readInt(
                "Enter appointment ID to cancel: "
            );

            if (appointmentManager.cancelAppointment(id)) {
                cout << "Appointment cancelled successfully.\n";
            }
            else {
                cout << "Appointment not found or already cancelled.\n";
            }

            break;
        }

        case 9: {
            if (user.role != "Admin") {
                cout << "Only Admin can view dashboard reports.\n";
                break;
            }

            appointmentManager.showDashboard();

            break;
        }

        case 10: {
            cout << "\n1. Sort by Patient ID\n";
            cout << "2. Sort by Patient Name\n";

            int sortChoice = readInt("Enter sorting choice: ");

            if (sortChoice == 1) {
                viewPatients(
                    patientManager.getAllPatients("id")
                );
            }
            else if (sortChoice == 2) {
                viewPatients(
                    patientManager.getAllPatients("name")
                );
            }
            else {
                cout << "Invalid sorting choice.\n";
            }

            break;
        }

        case 11: {
            if (user.role != "Admin") {
                cout << "Only Admin can create database backups.\n";
                break;
            }

            string filename = readText(
                "Enter backup filename: "
            );

            if (filename.empty()) {
                filename = "patientcare_backup.db";
            }

            if (database.backup(filename)) {
                cout << "Database backup created successfully.\n";
            }
            else {
                cout << "Database backup failed.\n";
            }

            break;
        }

        default:
            cout << "Invalid menu choice.\n";
        }
    }

    return 0;
}
