#include "Admin.hpp"
#include <iostream>

Admin::Admin() : Person() {}

Admin::Admin(int id, const char* name, const char* password)
    : Person(id, name, password, "") {}

void Admin::displayMenu() {
    std::cout << "\nAdmin Panel - MediCore\n";
    std::cout << "======================\n";
    std::cout << " 1. Add Doctor\n";
    std::cout << " 2. Remove Doctor\n";
    std::cout << " 3. Add Patient\n";
    std::cout << " 4. Remove Patient\n";
    std::cout << " 5. View All Patients\n";
    std::cout << " 6. View All Doctors\n";
    std::cout << " 7. View All Appointments\n";
    std::cout << " 8. View Unpaid Bills\n";
    std::cout << " 9. Discharge Patient\n";
    std::cout << "10. View Security Log\n";
    std::cout << "11. Generate Daily Report\n";
    std::cout << "12. Logout\n";
    std::cout << "Enter choice: ";
}

void Admin::displayProfile() {
    std::cout << "Admin: " << name << " (ID: " << id << ")\n";
}