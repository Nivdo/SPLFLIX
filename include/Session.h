#ifndef SPLFLIX_SESSION_H
#define SPLFLIX_SESSION_H
#include <vector>
#include <unordered_map>
#include <string>
#include "../include/Action.h"
#include "../include/User.h"

class User;
class Watchable;
class BaseAction;

class Session{
public:
    //****Rule of five****//
    Session(const std::string &configFilePath);
    ~Session();                         //destructor
    Session (const Session &other);             //Copy Constructor
    Session& operator=(const Session &other);   //Copy Assignment Operator
    Session(Session &&other);                   // Move Constructor
    Session& operator=(Session &&other);        // Move Assignment Operator
    void clear();
    void removeUserFromMap (User* user);
    //**********//
    void start();

    //****getters and setters****//
    bool isUserNameInUse(const std::string UserName);
    void setActiveUser (User* user);
    User* const getActiveUser ();
    User* const getUser (std::string userName);
    std::vector<Watchable*>& getContentListR();
    Watchable* const getContent (long contentId);
    std::vector<BaseAction*>& getActionLogR();
    std::vector<string>& getTagsVector();
    void addUserToMap (User* user);
private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    std::vector<string> allTags;
    bool checkStringIsNumber (std::string s);
};

#endif //SPLFLIX_SESSION_H
