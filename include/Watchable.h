#ifndef SPLFLIX_WATCHABLE_H
#define SPLFLIX_WATCHABLE_H

#include <string>
#include <vector>
using namespace std;
class Session;

class Watchable{
public:
    //***constructor & destructors***//
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual ~Watchable();
    //getters & setters
    long getId() const;
    int getLength() const;
    virtual std::string getTagsString() const;
    bool checkTag (std::string);
    std::vector<std::string> getTagsR() const;
    //pure virtual methods
    virtual Watchable* getNextWatchable(Session&) const = 0;
    virtual std::string toString() const = 0;
    virtual Watchable* clone() = 0;
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};
class Movie : public Watchable{
public:
    //***constructor***//
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    //***setters and getters***//
    virtual Watchable* getNextWatchable(Session&) const;
    //***other methods***//
    virtual std::string toString() const;
    Movie* clone();
private:
    std::string const name;

};

class Episode: public Watchable{
public:
    //***constructor***//
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    //***setters and getters***//
    virtual Watchable* getNextWatchable(Session&) const;
    void setLast();
    //***other methods***//
    virtual std::string toString() const;
    Episode* clone();
private:
    std::string const seriesName;
    int const season;
    int const episode;
    bool isLast;
    long const nextEpisodeId;
};
#endif //SPLFLIX_WATCHABLE_H
