#ifndef SPLFLIX_USER_H
#define SPLFLIX_USER_H

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

class Watchable;
class Session;

class User{
public:
    //***constructors***/
    User(const std::string& name);
    virtual ~ User();
    User (const User& other)= default;
    //***getters & setters***/
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void updateUserHistory(std::vector<Watchable*> newHistory);//for duplicating userHistory in the same session
    void duplicateUserHistory(Session &sess , std::vector<Watchable*> historyToDuplicate); //for duplicating userHistory in a different Session
    void addWatchableToHistory(Watchable* watchable);

    //***pure virtual methods***//
    virtual User* clone(std::string)=0;
    virtual void maintainAlgorithm(Watchable* contentP)=0;

    //***other methods***//
    void clearUser();
    bool isWatched (Watchable* contentP) const;
    std::string lower(std::string s);
    void setName (std::string username);
protected:
    std::vector<Watchable*> history;
private:
    std::string name;

};

class LengthRecommenderUser : public User {
public:
    //***constructors***/
    LengthRecommenderUser(const std::string& name);
    virtual ~ LengthRecommenderUser();
    //***getters & setters***/
    virtual Watchable* getRecommendation(Session& s);
    void setSumOfLength(long otherSumofLength);
    //***other methods***//
    virtual User* clone(std::string username);
    void maintainAlgorithm(Watchable* contentP);
private:
    long sumofLength;
};

class RerunRecommenderUser : public User {
public:
    //***constructors***/
    RerunRecommenderUser(const std::string& name);
    virtual ~ RerunRecommenderUser();
    //***getters & setters***/
    virtual Watchable* getRecommendation(Session& s);
    void setReWatchIndex (int Index);

    //***other methods***//
    virtual User* clone(std::string username);
    void maintainAlgorithm(Watchable* contentP);
private:
    int reWatchIndex;
};

class GenreRecommenderUser : public User {
public:
    //***constructors***/
    GenreRecommenderUser(const std::string& name);
    virtual ~ GenreRecommenderUser();
    //***getters & setters***/
    virtual Watchable* getRecommendation(Session& s);
    void const setGenreVector(std::vector<std::pair<int,std::string>> otherGenreVector);
    //***other methods***//
    virtual User* clone(std::string username);
    void maintainAlgorithm(Watchable* contentP);
    void initialiseGenreVector (Session &sess);
private:
    std::vector<std::pair<int,std::string>> genreVector;

};

#endif //SPLFLIX_USER_H