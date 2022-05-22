//code processor
//Fatima Bowers
//10/26/21
//Description- Rewards program where users can enter codes via a web site, or register cell phones with their account and text codes, which will register points. Write a server that maintains information about users, prizes, and codes

#include "code_processor.hpp"
#include <fstream>

using namespace std;

map <string, Prize *>::iterator prit; //Prizes iterator
map <string, User *>::iterator nit; //Names iterator
map <string, User *>::iterator phit; //Phones iterator
set <string>::iterator sit; //phone_numbers iterator
set <string>::iterator cit; //codes iterator

//const iterators
map <string, User *>::const_iterator cnit; //const Names iterator
map <string, Prize *>::const_iterator cprit; //const Prizes iterator
map <string, User *>::const_iterator cphit; //const Phones iterator
set <string>::const_iterator ccit; //const codes iterator

unsigned int djb_hash(const string &s);

bool Code_Processor::New_Prize(const string &id, const string &description, int points, int quantity)
{
	//check if prize is already in prize map, compare id with first
	prit = Prizes.find(id);
	if(prit != Prizes.end()) return false;

	//check if points and quanitity are >0
	if(points <= 0 || quantity <= 0) return false;

	//allocate memory, new prize class
	Prize *p;
	p = new Prize;

	//update class contents
	p->id = id;
	p->description = description;
	p->points = points;
	p->quantity = quantity;

	//put into prize map-- associated array method
	Prizes[id] = p;

	return true;
}

bool Code_Processor::New_User(const string &username, const string &realname, int starting_points)
{
	//check if username already exists
	nit = Names.find(username);
	if(nit != Names.end()) return false;

	//check if starting points is < 0
	if(starting_points < 0) return false;

	//allocate memory, new user class
	User *u;
	u = new User;

	//update class contents
	u->username = username;
	u->realname = realname;
	u->points = starting_points;

	//put into names map-- associated array method
	Names[username] = u;

	return true;
}

bool Code_Processor::Delete_User(const string &username)
{
	//check if username already exists
	nit = Names.find(username);
	if(nit == Names.end()) return false;

	//remove phones in User's phone_numbers from Phone map
	//loop through phone_numbers set
	for(sit = nit->second->phone_numbers.begin(); sit != nit->second->phone_numbers.end(); sit++){

		//.find in Phones map, and erase from map
		phit = Phones.find(*sit);
		if (phit != Phones.end()) Phones.erase(phit);
	}

	//delete user's memory
	delete nit->second;

	//remove from names map
	Names.erase(nit);

	return true;
}

bool Code_Processor::Add_Phone(const string &username, const string &phone)
{
	//check if username doesn't exists
	nit = Names.find(username);
	if(nit == Names.end()) return false;

	//check if phone number already exists
	phit = Phones.find(phone);
	if(phit != Phones.end()) return false;

	//insert phone number
	Phones[phone] = nit->second;
	nit->second->phone_numbers.insert(phone);

	return true;
}

bool Code_Processor::Remove_Phone(const string &username, const string &phone)
{
	//check if username doesn't exist
	nit = Names.find(username);
	if(nit == Names.end()) return false;

	//phone number doesn't belong to user
	sit = nit->second->phone_numbers.find(phone);
	if(sit == nit->second->phone_numbers.end()) return false;
	//remove number
	nit->second->phone_numbers.erase(sit);

	//phone number doesn't match user in phones map
	phit = Phones.find(phone);
	if(nit->second->username != phit->second->username) return false;

	//remove from map
	Phones.erase(phit);

	return true;
}

string Code_Processor::Show_Phones(const string &username) const
{
	string allPhones = "";

	//check if username doesn't exist
	cnit = Names.find(username);
	if(cnit == Names.end()) return "BAD USER";

	//iterate through phone_numbers set and add each phone number to the string
	for(sit = cnit->second->phone_numbers.begin(); sit != cnit->second->phone_numbers.end(); sit++){
		allPhones = allPhones + *sit + '\n';
	}

	return allPhones;
}

int Code_Processor::Enter_Code(const string &username, const string &code)
{
	size_t h;

	//check if code already exists
	cit = Codes.find(code);
	if(cit != Codes.end()) return -1;

	//check if user doesn't exist
	nit = Names.find(username);
	if(nit == Names.end()) return -1;

	h = djb_hash(code);

	//check if code is valid-- divisable by 17 or 13-- add points to user, and add code to Codes
	if(h % 17 == 0){
		nit->second->points += 10;
		Codes.insert(code);
		return 10;
	}
	
	if(h % 13 == 0){
		nit->second->points += 3;
		Codes.insert(code);
		return 3;
	}
	
	return 0;
}

int Code_Processor::Text_Code(const string &phone, const string &code)
{
	size_t h;

	//check if code already exists
	cit = Codes.find(code);
	if(cit != Codes.end()) return -1;
	
	//check if user doesn't exist
	phit = Phones.find(phone);
	if(phit == Phones.end()) return -1;

	h = djb_hash(code);

	//check if code is valid-- divisable by 17 or 13-- add points to user, and add code to Codes
	if(h % 17 == 0){
		phit->second->points += 10;
		Codes.insert(code);
		return 10;
	}
	if(h % 13 == 0){
		phit->second->points += 3;
		Codes.insert(code);
		return 3;
	}

	return 0;
}

bool Code_Processor::Mark_Code_Used(const string &code)
{
	size_t h;

	//check if code already exists
	cit = Codes.find(code);
	if(cit != Codes.end()) return false;

	h = djb_hash(code);

	//check if code is valid and insert code to Codes set
	if((h % 17 != 0) && (h % 13 != 0)) return false;

	Codes.insert(code);

	return true;
}

int Code_Processor::Balance(const string &username) const
{
	//check if user doesn't exist
	cnit = Names.find(username);
	if(cnit == Names.end()) return -1;

	return cnit->second->points;
}

bool Code_Processor::Redeem_Prize(const string &username, const string &prize)
{
	//check if user doesn't exist
	nit = Names.find(username);
	if(nit == Names.end()) return false;

	//check if prize doesn't exist
	prit = Prizes.find(prize);
	if(prit == Prizes.end()) return false;

	//check if user has enough points for the prize
	if(nit->second->points < prit->second->points) return false;

	//decrement prize points from user's account
	nit->second->points -= prit->second->points;

	//decrement prize's quantity
	prit->second->quantity--;

	//check if prize quantity is 0-- remove prize from system
	if(prit->second->quantity == 0){
		delete prit->second;
		Prizes.erase(prit);
	}

	return true;
}

Code_Processor::~Code_Processor()
{
	//delete users
	for(nit = Names.begin(); nit != Names.end(); nit++) delete nit->second;

	//delete prizes
	for(prit = Prizes.begin(); prit != Prizes.end(); prit++) delete prit->second;
}

bool Code_Processor::Write(const string &filename) const
{
	ofstream fout;

	//open file, check if it fails
	fout.open(filename.c_str());
	if(!fout.is_open()) return false;

	//consists of ADD_USER, PRIZE, ADD_PHONE and MARK_USED
	//write PRIZE
	for(cprit = Prizes.begin(); cprit != Prizes.end(); cprit++){
		fout << "PRIZE " << cprit->second->id << " " << cprit->second->points << " " << cprit->second->quantity << " " << cprit->second->description << "\n";
	}

	//write ADD_USER
	for(cnit = Names.begin(); cnit != Names.end(); cnit++){
		fout << "ADD_USER " << cnit->second->username << " " << cnit->second->points << " " << cnit->second->realname << "\n";
	}

	//write ADD_PHONE
	for(cphit = Phones.begin(); cphit != Phones.end(); cphit++){
		fout << "ADD_PHONE " << cphit->second->username << " " << cphit->first << "\n";
	}

	//write MARK_USED
	for(ccit = Codes.begin(); ccit != Codes.end(); ccit++){
		fout << "MARK_USED " << *ccit << "\n";
	}

	fout.close();

	return true;
}

unsigned int djb_hash(const string &s)
	//hash function
{
	size_t i;
	unsigned int h;

	h = 5381;

	for (i = 0; i < s.size(); i++) {
		h = (h << 5) + h + s[i];
	}
	return h;
}
