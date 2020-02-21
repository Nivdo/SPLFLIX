#ifndef SPLFLIX_ACTION_H
#define SPLFLIX_ACTION_H

#include <string>
#include "../include/User.h"
using namespace std;

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};

class BaseAction{
public:
    //***constructors destructor***//
    BaseAction();
    virtual ~BaseAction();

    //***getters and setters***//
    ActionStatus getStatus() const;
    //***other methods***//
    virtual void act(Session& sess)=0;
    virtual std::string toString() const=0;
    virtual BaseAction* clone()=0;
    std::string getErrorMsg() const;
protected:
    void complete();
    void error(const std::string& Msg);
private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    //***constructors destructor***//
    CreateUser(std::string userName, std::string rec_Code);

    //***other methods***//
    virtual void act(Session &sess);
    virtual std::string toString() const;
    virtual CreateUser* clone();
private:
    bool checkRecCode(const std::string reccomendation_Code);
    User* userCreator(const std::string reccomendation_Code , const std::string userName,Session &sess);
    std::string const userName;
    std::string const reccomendation_Code;
};


class ChangeActiveUser : public BaseAction {
public:
    //***constructors destructor***//
    ChangeActiveUser(string userName);

    //***other methods***//
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual ChangeActiveUser* clone();
private:
    string const userName;
};

class DeleteUser : public BaseAction {
public:
    //***constructors destructor***//
    DeleteUser(string userName);

    //***other methods***//
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DeleteUser* clone();
private:
    string const userName;
};


class DuplicateUser : public BaseAction {
public:
    //***constructors destructor***//
    DuplicateUser(std::string original_user_name, std::string new_user_name);

    //***other methods***//
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DuplicateUser* clone();
private:
    std::string const orgUserName;
    std::string const newUserName;
};

class PrintContentList : public BaseAction {
public:
    //***other methods***//
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintContentList* clone();
};

class PrintWatchHistory : public BaseAction {
public:
    //***other methods***//
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintWatchHistory* clone();
};


class Watch : public BaseAction {
public:
    //***constructors destructor***//
    Watch(long contentId);

    //***other methods***//
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Watch* clone();
private:
    long const contentId;
};


class PrintActionsLog : public BaseAction {
public:
    //***other methods***//
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual PrintActionsLog* clone();
};

class Exit : public BaseAction {
public:
    //***other methods***//
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Exit* clone();
};

#endif //SPLFLIX_ACTION_H
