#include "../include/Action.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include <iostream>

using namespace std;


// BaseAction class method
BaseAction:: BaseAction(): errorMsg() , status(){}
BaseAction:: ~BaseAction(){}
ActionStatus BaseAction:: getStatus() const {return status;}
void BaseAction::act(Session& sess) {};
void BaseAction::complete() {status=COMPLETED;}
void BaseAction::error(const std::string& Msg) {
    status=ERROR;
    errorMsg=Msg;
    std::cout<< "Error - " + Msg; cout<<"\n";
}

std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}

//CreateUser Class methods
CreateUser::CreateUser(string userName, string rec_Code): userName(userName), reccomendation_Code(rec_Code){}

void CreateUser::act(Session &sess) {
    if(!checkRecCode(reccomendation_Code)){
        error("recommendation algorithm is invalid");
    }
    else if(sess.isUserNameInUse(userName)) {
        error("username is already in use");
    }
    else {
        sess.addUserToMap(userCreator(reccomendation_Code,userName,sess));
        complete();
    }
}

string CreateUser:: toString() const {
    return "CreateUser";
}

bool CreateUser::checkRecCode(const std::string recCode) {
    return (recCode == "len" || recCode == "rer" || recCode == "gen");
}

User *CreateUser::userCreator(const std::string recCode, const std::string userName , Session &sess) {
    if(recCode=="len") {
        LengthRecommenderUser *newUser = new LengthRecommenderUser(userName);
        return newUser;
    }
    if(recCode=="rer") {
        RerunRecommenderUser *newUser = new RerunRecommenderUser(userName);
        return newUser;
    }
    if(recCode=="gen") {
        GenreRecommenderUser *newUser = new GenreRecommenderUser(userName);
        newUser->initialiseGenreVector(sess);
        return newUser;
    }
    return nullptr;
}


CreateUser *CreateUser::clone() {
    return new CreateUser(*this);
}

ChangeActiveUser::ChangeActiveUser(string name): userName(name){}
void ChangeActiveUser::act(Session& sess) {
    if (sess.isUserNameInUse(userName)){
        sess.setActiveUser(sess.getUser(userName));
        complete();
    }
    else
        error("username does not exist");

}

string ChangeActiveUser:: toString() const {
    return "ChangeActiveUser";
}

ChangeActiveUser *ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}

//DeleteUser Class methods

DeleteUser::DeleteUser(string userName): userName(userName){};
void DeleteUser::act(Session & sess) {
    if(sess.isUserNameInUse(userName)){
        User * toDelete=sess.getUser(userName);
        sess.removeUserFromMap(toDelete);
        delete (toDelete);
        complete();
    }
    else
        error("username does not exist");
}
string DeleteUser:: toString() const {
    return "DeleteUser";
}

DeleteUser *DeleteUser::clone() {
    return new DeleteUser(*this);
}

//DuplicateUser Class methods
DuplicateUser:: DuplicateUser(string orgUserName, string newUserName): orgUserName(orgUserName), newUserName(newUserName){};
void DuplicateUser::act(Session &sess) {
    if(sess.isUserNameInUse(orgUserName)){
        if(sess.isUserNameInUse(newUserName))
            error("username is already in use");
        else {
            User *dupUser = sess.getUser(orgUserName)->clone(newUserName);
            dupUser->setName(newUserName);
            sess.addUserToMap(dupUser);
            complete();
        }
    }
    else
        error("username does not exist");
}
string DuplicateUser:: toString() const {
    return "DuplicateUser";
}

DuplicateUser *DuplicateUser::clone() {
    return new DuplicateUser(*this) ;
}

//PrintContentList Class methods
void PrintContentList:: act(Session& sess) {
    vector<Watchable*>& contentR = sess.getContentListR();
    if(contentR.empty())
        error("no content available");
    else{
        int i =1;
        for(Watchable* contentP : contentR) {
            cout <<to_string(i) + ". " + contentP->toString() + " " + to_string(contentP->getLength()) + " minutes" + contentP->getTagsString();
            cout << "\n";
            i++;
        }
        complete();
    }
}

string PrintContentList::toString() const {
    return "PrintContentList";
}

PrintContentList *PrintContentList::clone() {
    return new PrintContentList(*this);
}

//PrintWatchHistory Class methods
void PrintWatchHistory:: act(Session& sess){
    User* user =  sess.getActiveUser();
    vector<Watchable*> history = user->get_history();
    if(history.empty())
        error("no content history available");
    else {
        cout << "Watch history for " + user->getName();
        cout << "\n";
        int i=1;
        for(Watchable* content : history){
            cout << to_string(i) + ". " + content->toString();
            cout << "\n";
            i++;
        }
        complete();
    }
}

string PrintWatchHistory::toString() const {
    return "PrintWatchHistory";
}

PrintWatchHistory *PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);
}

//Watch Class  methods
Watch:: Watch(long contentId): contentId(contentId) {}

void Watch::act(Session& sess){
    User* user =  sess.getActiveUser();
    if(sess.getContent(contentId)== nullptr){
        error("no content available with this ID");
        return;
    }
    Watchable* content = sess.getContent(contentId);
    cout << "Watching " +content->toString()+"\n";
    user->addWatchableToHistory(content);
    user->maintainAlgorithm(sess.getContent(contentId));
    Watchable* next =content->getNextWatchable(sess);
    char input=' ';
    if(next==nullptr) {
        error("no recommendation available, you have watched everything. get a life...");
        input = 'n';
    }
    else
        cout << "We recommend watching " +next->toString() + ", continue watching?[y/n]\n";
    while (input!='y' && input!='n') {
        cin >> input;
        if (input == 'y') {
            Watch *binge = new Watch(next->getId());
            binge->act(sess);
            sess.getActionLogR().push_back(binge);
        }
    }
    complete();
}

string Watch::toString() const {
    return "Watch";
}

Watch *Watch::clone() {
    return new Watch(*this) ;
}

//PrintActionsLog Class  methods
void PrintActionsLog:: act(Session& sess){
    std::vector<BaseAction*> logR = sess.getActionLogR();
    int size=logR.size();
    for(int i=0 ; i<size ; i++){
        BaseAction* action=logR.at(size-i-1);

        if(action->getStatus()==PENDING)
                cout<<action->toString() + " PENDING";
        else if(action->getStatus()==COMPLETED)
                cout<<action->toString() + " COMPLETED";
        else if(action->getStatus()==ERROR) {
            cout << (action->toString() + " ERROR: " + action->getErrorMsg());
        }
        cout << "\n";
    }
    complete();
}

std::string PrintActionsLog::toString() const {
    return "PrintActionsLog";
}

PrintActionsLog *PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}


void Exit::act(Session &sess) {
    complete();
}

std::string Exit::toString() const {
    return "Exit";
}

Exit *Exit::clone() {
    return new Exit(*this);
}


