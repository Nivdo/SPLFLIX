
#include "../include/User.h"
#include "../include/Watchable.h"
#include "../include/Session.h"
#include <math.h>
#include <limits>
#include <vector>
#include <algorithm>


using namespace std;

User::User(const string& name): history() , name(name){};

User::~User(){clearUser();}

string User::getName()const{return name;};

vector<Watchable*> User::get_history() const {return history;};

bool User:: isWatched (Watchable* contentP) const {
    for(Watchable* watched : history){
        if(watched==contentP)
            return true;
    }
    return false;
};

string User::lower(std::string s) {
    string output="";
    for (char c: s){
        output+=tolower(c);
    }
    return output;
}

void User::updateUserHistory(std::vector<Watchable *> newHistory) {
    history.clear();
    for(size_t i=0 ; i<newHistory.size();i++){
        history.push_back(newHistory.at(i));
    }
}


//Length Recommender
LengthRecommenderUser::LengthRecommenderUser(const std::string& name): User(name) , sumofLength(0) {};

Watchable* LengthRecommenderUser:: getRecommendation(Session& s){
    double minDiff=std::numeric_limits<double>::max();
    double avgLength;
    if(history.empty())
        avgLength=0;
    else
        avgLength=sumofLength/history.size();
    Watchable* nextP = nullptr;
    for (Watchable* contentP : s.getContentListR()){
        int length = contentP->getLength();
        double diff =abs(length-avgLength);

        if(diff<=minDiff && !isWatched(contentP) ){
            if(diff<minDiff || lower(contentP->toString())<(lower(nextP->toString()))) {
                nextP=contentP;
                minDiff=diff;
            }
        }
    }
    return nextP;
}

void LengthRecommenderUser::maintainAlgorithm(Watchable *contentP) {
    sumofLength=sumofLength+contentP->getLength();
}

LengthRecommenderUser::~LengthRecommenderUser(){}

User *LengthRecommenderUser::clone(std::string username) {
    LengthRecommenderUser * clonedUser = new LengthRecommenderUser(username);
    clonedUser->updateUserHistory(history);
    clonedUser->setSumOfLength(sumofLength);
    return clonedUser;
}

void LengthRecommenderUser::setSumOfLength(long otherSumofLength) {
    sumofLength=otherSumofLength;
}


//Rerun Recommender
RerunRecommenderUser::RerunRecommenderUser(const std::string &name): User(name) , reWatchIndex(0) {};

void RerunRecommenderUser:: maintainAlgorithm(Watchable* contentP){};

Watchable* RerunRecommenderUser:: getRecommendation(Session& s) {
    Watchable *recommendation = history.at(reWatchIndex % history.size());
    reWatchIndex = reWatchIndex + 1;
    return recommendation;
}

RerunRecommenderUser::~RerunRecommenderUser(){}

User *RerunRecommenderUser::clone(std::string username) {
    RerunRecommenderUser * clonedUser = new RerunRecommenderUser(username);
    clonedUser->updateUserHistory(history);
    clonedUser->setReWatchIndex(reWatchIndex);
    return clonedUser;
}

void RerunRecommenderUser::setReWatchIndex(int Index) {
    reWatchIndex=Index;
}

//Gener Reccomender
GenreRecommenderUser::GenreRecommenderUser(const std::string &name): User(name) , genreVector(){};

Watchable* GenreRecommenderUser:: getRecommendation(Session& s){
    sort(genreVector.begin(),genreVector.end());
    for(pair<int,string> genre : genreVector) {
        for (Watchable *contentP : s.getContentListR()) {
            if (contentP->checkTag(genre.second) && !isWatched(contentP)) {
                return contentP;
            }
        }
    }
    return nullptr;
}

void GenreRecommenderUser::maintainAlgorithm(Watchable *contentP) {
    for(string tag:contentP->getTagsR()){
        for(size_t i =0 ; i<genreVector.size(); i++){
            if(genreVector[i].second==tag){
                genreVector[i].first--;
            }
        }
    }
}


GenreRecommenderUser::~GenreRecommenderUser(){genreVector.clear();}

void GenreRecommenderUser::initialiseGenreVector(Session &sess) {
    for (size_t i=0 ; i<sess.getTagsVector().size() ; i++){
        pair<int , string> p;
        p.first=0;
        p.second=sess.getTagsVector().at(i);
        genreVector.push_back(p);
    }
}

User *GenreRecommenderUser::clone(std::string username) {
    GenreRecommenderUser * clonedUser = new GenreRecommenderUser(username);
    clonedUser->updateUserHistory(history);
    clonedUser->setGenreVector(genreVector);
    return clonedUser;
    }

void const GenreRecommenderUser::setGenreVector(std::vector<std::pair<int, std::string>> otherGenreVector) {
    for(size_t i=0 ; i<otherGenreVector.size() ; i++){
        pair<int , string> p (otherGenreVector.at(i));
        genreVector.push_back(p);
    }
}


void User:: clearUser(){
    history.clear();
}


void User::addWatchableToHistory(Watchable *watchable) {
    history.push_back(watchable);
}

void User::setName(std::string username) {
    name=username;

}

void User::duplicateUserHistory(Session &sess, std::vector<Watchable *> historyToDuplicate) {
    history.clear();
    for(Watchable* element : historyToDuplicate){
        history.push_back(sess.getContentListR().at(element->getId()-1));
    }
}