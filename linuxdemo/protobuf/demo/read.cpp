#include <iostream>
#include <fstream>
#include <string>
#include "address.pb.h"
 
using namespace std;
 
void ListPeople(const tutorial::AddressBook& address_book) 
{
     for (int i = 0; i < address_book.person_size(); i++) {
         const tutorial::Person& person = address_book.person(i);
         cout << person.name() << " " << person.age() << endl;
     }
}
 
int main(int argc, char **argv) {
     //GOOGLE_PROTOBUF_VERIFY_VERSION;
 
     if (argc != 2) {
         cerr << "Usage: " << argv[0] << " ADDRESS_BOOL_FILE" << endl;
         return -1;
     }
 
     tutorial::AddressBook address_book;
 
     {
         fstream input(argv[1], ios::in | ios::binary);
         if (!address_book.ParseFromIstream(&input)) {
             cerr << "Filed to parse address book." << endl;
             return -1;
         }
         input.close();
     }
 
     ListPeople(address_book);
 
     // Optional: Delete all global objects allocated by libprotobuf.
     //google::protobuf::ShutdownProtobufLibrary();
 
     return 0;
}
