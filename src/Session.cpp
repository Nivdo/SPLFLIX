
#include "fstream"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include "../include/json.hpp"
#include <iostream>


using namespace std;
//****constructor****//
Session:: Session(const string &configFilePath) : content(), actionsLog(), userMap(), activeUser() ,allTags()  {
    LengthRecommenderUser* P_default_User = new LengthRecommenderUser("default"); //creating a default user
    addUserToMap(P_default_User);
    setActiveUser(P_default_User);

    //parsing json
    using json= nlohmann::json;
    std::ifstream t(configFilePath);
    json js;
    t>>js;
    json movies=js["movies"];
    int movieID=1;      //set first movieID to 1
    for (auto& input : movies.items()){
        json currentMovie = input.value();
        content.push_back(new Movie(movieID, currentMovie["name"], currentMovie["length"], currentMovie["tags"]));
        for(string tag:currentMovie["tags"]){
            if(find(allTags.begin(),allTags.end(),tag)==allTags.end())
                allTags.push_back(tag);
        }
        movieID++;
    }
    json TVseries = js["tv_series"];
    long episodeID=content.size()+1;
    for (auto& input: TVseries.items()){
        int seasonNumber=1;
        json currentTVseries = input.value();
        vector<int> seasons= currentTVseries ["seasons"];
        for (int currentSeason : seasons){
            for (int episodeNumber=1; episodeNumber<=currentSeason; episodeNumber++){
                Episode* newEpisode = new Episode(episodeID, currentTVseries["name"], currentTVseries["episode_length"], seasonNumber, episodeNumber, currentTVseries["tags"]);
                if(((episodeNumber==currentSeason) && (seasonNumber==(int)seasons.size()))){
                    newEpisode->setLast();
                }
                content.push_back(newEpisode);
                for(string tag:currentTVseries["tags"]){
                    if(find(allTags.begin(),allTags.end(),tag)==allTags.end())
                        allTags.push_back(tag);
                }
                episodeID++;
            }
            seasonNumber++;
        }
    }
    std::vector<BaseAction*> actionsLog();
}

//****rule of five****//

Session::~Session(){        //destructor//
    clear();
}

void Session:: clear() {
    //Delete UserMap
    for(pair<string, User*> element : userMap){
        element.second->clearUser();
        delete(element.second);         //delete user in this element in the map
    }
    userMap.clear();
    //delete content
    for(size_t i =0 ; i< content.size() ; i++){
        delete(content.at(i));
    }
    content.clear();
    for(size_t i =0 ; i<actionsLog.size();i++){
        delete(actionsLog.at(i));
    }
    actionsLog.clear();
    allTags.clear();
}

Session:: Session (const Session &other): content() , actionsLog() , userMap() , activeUser() , allTags(){   //copy constructor
    for(Watchable* element: other.content){     //copying the content from other to this
        content.push_back(element->clone());
    }
    //copying actionsLog from other to this
    for (BaseAction* element: other.actionsLog){
        actionsLog.push_back(element->clone());
    }
    //copying the userMap from other to this
    for(pair<string, User*> element : other.userMap){
        User * copiedUser = element.second->clone(element.first);
        copiedUser->duplicateUserHistory(*this , element.second->get_history());
        addUserToMap(copiedUser);
        if(other.activeUser->getName()==element.second->getName())
            activeUser=getUser(other.activeUser->getName());
    }
    allTags=other.allTags;
}

//copy assignment operator
Session& Session::operator=(const Session &other){
    //delete current content
    for(size_t i =0 ; i< content.size() ; i++){
        delete(content.at(i));
    }
    content.clear();
    //update to other's content
    for(size_t  i=0 ; i< other.content.size(); i++){
        content.push_back(other.content.at(i)->clone());
    }

    //delete current actionsLog
    for(size_t i =0 ; i<actionsLog.size();i++){
        delete(actionsLog.at(i));
    }
    actionsLog.clear();
    //update to other's actionsLog
    for (BaseAction* element: other.actionsLog){
        actionsLog.push_back(element->clone());
    }
    //delete current map
    for(pair<string, User*> element : userMap){
        element.second->clearUser();
        delete (element.second);
    }
    userMap.clear();
    //update to other's map
    for(pair<string, User*> element : other.userMap){
        User * copiedUser = element.second->clone(element.first);
        copiedUser->duplicateUserHistory(*this , element.second->get_history());
        addUserToMap(copiedUser);
        if(element.second==other.activeUser)
            activeUser=userMap.at(element.first);
    }
    allTags.clear();
    allTags=other.allTags;
    return *this;
}

//Move constructor
Session::Session(Session &&other): content(other.content), actionsLog(other.actionsLog), userMap(other.userMap), activeUser(other.activeUser), allTags(other.allTags) {

    other.content.clear();
    other.actionsLog.clear();
    other.userMap.clear();
    other.allTags.clear();
    other.activeUser= nullptr;
}

//Move Assignment Operator
Session& Session:: operator=(Session&& other){
    if (this != &other){
        //delete current content of this
        for(size_t i =0 ; i< content.size() ; i++){
            delete(content.at(i));
        }
        content.clear();

        //steal other's content and delete other's content
        for(size_t i =0 ; i<other.content.size() ; i++){
            content.push_back(other.content.at(i));
        }
        other.content.clear();
        //delete current actionsLog of this
        for(size_t i =0 ; i<actionsLog.size();i++) {
            delete (actionsLog.at(i));
        }
        actionsLog.clear();

        //steal other's content
        for(size_t i=0 ; i<other.actionsLog.size() ; i++){
            actionsLog.push_back(other.actionsLog.at(i));
        }
        other.actionsLog.clear();
        //delete current map of this
        for(pair<string, User*> element : userMap){
            delete(element.second);
        }
        userMap.clear();
        //copies the other users to this userMap
        for(pair<string, User*> element : other.userMap){
            addUserToMap(element.second);
        }
        activeUser=other.activeUser;
        other.userMap.clear();
        allTags.clear();
        allTags=other.allTags;
        other.allTags.clear();
        other.activeUser=nullptr;
    }
    return *this;
}


void Session:: start(){
    std::cout<<"SPLFLIX IS NOW ON!";
    cout <<"\n";
    string input=" ";
    string action;
    string first_arg;
    string second_arg;
    bool isExit=false;
    int spaces=-1;
    while(!isExit){
        if((action=="createuser") && (spaces==2)){
            BaseAction* CU = new CreateUser(first_arg,second_arg);
            CU->act(*this);
            actionsLog.push_back(CU);}

        else if((action=="changeuser") && (spaces==1)){
            BaseAction* CAU = new ChangeActiveUser(first_arg);
            CAU->act(*this);
            actionsLog.push_back(CAU);}

        else if((action=="deleteuser") && (spaces==1)){
            BaseAction* DU = new DeleteUser(first_arg);

            DU->act(*this);
            actionsLog.push_back(DU);}

        else if((action=="dupuser") && (spaces==2)){
            BaseAction* DupU = new DuplicateUser(first_arg,second_arg);
            DupU->act(*this);
            actionsLog.push_back(DupU);}

        else if((action=="content") && (spaces==0)){
            BaseAction* PCL = new PrintContentList();
            PCL->act(*this);
            actionsLog.push_back(PCL);}

        else if((action=="watchhist") && (spaces==0)){
            BaseAction* PWH = new PrintWatchHistory();
            PWH->act(*this);
            actionsLog.push_back(PWH);}

        else if((action=="watch") && (spaces==1) && checkStringIsNumber(first_arg)){
            BaseAction* w = new Watch(stol(first_arg));
            w->act(*this);
            actionsLog.push_back(w);}

        else if((action=="log") && (spaces==0)){
            BaseAction* PAL = new PrintActionsLog();
            PAL->act(*this);
            actionsLog.push_back(PAL);}

        else if((action=="exit") && (spaces==0)){
            BaseAction* Ex = new Exit();
            Ex->act(*this);
            actionsLog.push_back(Ex);
            isExit=true;}
        if(!isExit){
            getline(cin, input);
            spaces=0;
            for(char c: input){
                if(c==' ')
                    spaces++;
            }
            if(spaces==0)
                action=input;
            else if (spaces==1){
                action = input.substr(0, input.find_first_of(' '));
                first_arg=input.substr(input.find_first_of(' ')+1,input.length()-input.find_first_of(' ')-1);
            }
            else if (spaces==2){
                action = input.substr(0, input.find_first_of(' '));
                first_arg=input.substr(input.find_first_of(' ')+1,input.find_last_of(' ')-input.find_first_of(' ')-1);
                second_arg=input.substr(input.find_last_of(' ')+1,input.length()-input.find_last_of(' ')-1);
            }
        }
    }
}
//****getters and setters****//
void Session::addUserToMap (User* user){
    userMap.insert({user->getName(),user});
}

void Session::removeUserFromMap(User *user) {
    userMap.erase(user->getName());
}

bool Session::isUserNameInUse(const string UserName){
    for(pair<string,User*> userP : userMap) {
        if (userP.first == UserName)
            return true;
    }
    return false;
}

Watchable* const Session::getContent (long contentId){
    if((contentId>(long)content.size()) || (contentId==0))
        return nullptr;
    return content.at(contentId-1);
}

User* const Session::getActiveUser() {
    return activeUser;
}

void Session::setActiveUser (User* user){
    activeUser = user;
}

vector<Watchable*>& Session::getContentListR(){
    return content;
}

vector<BaseAction*>& Session::getActionLogR(){
    return actionsLog;
}

User* const Session::getUser(std::string userName){
   return userMap.at(userName);
}

std::vector<string> &Session::getTagsVector() {
    return allTags;
}

bool Session::checkStringIsNumber(string s) {
    for(char c : s){
        if(!isdigit(c))
            return false;
    }
    return true;
}

