/////////////////////////////////////////////////////////////////////////
// Make a program that stores and prints the info of TAC-HEP students. //
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>


// struct for holding personal information
struct TACHEPPER {
    std::string name;
    std::string email;
    std::string username;
    std::string experiment;
};

void print_hepper(const TACHEPPER &hepper)
{
    std::cout << "Name: " << hepper.name << std::endl;
    std::cout << "Email: " << hepper.email << std::endl;
    std::cout << "Username: " << hepper.username << std::endl;
    std::cout << "Experiment: " << hepper.experiment << std::endl;
    std::cout << std::endl;
}

int main()
{
    // create arrays storing info of TAC-HEP students
    int num_tacheppers = 13;
    std::string names[] = {"Walker", "Roy", "Kayleigh", "Taylor", "Jenna", "Max", "Daniel", "DaiJon", "Kyla", "Justin", "Elias", "Julianne", "Mark"};
    std::string emails[] = {"wsundquist@umass.edu", "royboy@gmail.com", "dailykaileigh@gmail.com", "tayloringsuits@gmail.com", "jenna@gmail.com", "minmax@gmail.com", "dhumphreys@umass.edu", "djjames@umass.edu", "kyla@gmail.com", "justinoughathis@gmail.com", "metticulous@gmail.com", "jstarzee@umass.edu", "mmurdy@umass.edu"};
    std::string usernames[] = {"wsund", "royboy", "dailykaileigh", "tayloringsuits", "jenna", "minmax", "dhumphreys", "djjames", "kyla", "justinenoughathis", "metticulous", "jstarzee", "mmurdy"};
    std::string experiments[] = {"ATLAS", "CMS", "Rubin", "CMS", "CMS", "CMS", "ATLAS", "ATLAS", "CMS", "CMS", "CMS", "ATLAS", "LZ"};

    TACHEPPER hep_arr[num_tacheppers];
    for (int i = 0; i < num_tacheppers; i++) {
        hep_arr[i].name = names[i];
        hep_arr[i].email = emails[i];
        hep_arr[i].username = usernames[i];
        hep_arr[i].experiment = experiments[i];
    }

    for (int i = 0; i < num_tacheppers; i++) {
        print_hepper(hep_arr[i]);
    }
    return 0;
}