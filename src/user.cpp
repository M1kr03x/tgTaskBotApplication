#include "../include/user.h"
User::User(){

}
User::User(std::string login,std::string password) : _login(login), _password(password){

}
void User::setLogin(std::string log){
    this->_login = log;
}
void User::setPassword(std::string password){
    this->_password = password;
}
std::string User::getPassword()const{return _password;}
std::string User::getLogin()const{return _login;}