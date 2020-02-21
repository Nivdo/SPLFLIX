#include "../include/Watchable.h"
#include "../include/Session.h"

using namespace std;
Watchable::Watchable(long id, int length, const std::vector<string>& tags): id(id) , length(length) , tags(tags){};
long Watchable:: getId() const {return id;};
int Watchable:: getLength() const {return length;};
vector<string> Watchable:: getTagsR() const {return tags;};
string Watchable:: getTagsString() const{
    string output = "[";
    for(string tag : getTagsR()){
        output=output+tag+", ";
    }
    return  output.substr(0,output.length()-2)+"]";
}

bool Watchable::checkTag(std::string tag) {
    for(string s:tags){
        if(s==tag){
            return true;
        }
    }
    return false;
}

Watchable::~Watchable() {
    tags.clear();
}

Movie:: Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags): Watchable(id, length, tags),name(name){}
string Movie::toString() const {
    return name;
}

Watchable* Movie:: getNextWatchable(Session& sess) const {
    return sess.getActiveUser()->getRecommendation(sess);
}

Movie* Movie::clone() {
    return new Movie(*this);
}


Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags): Watchable(id, length, tags ),seriesName(seriesName),season(season),episode(episode), isLast(false) , nextEpisodeId(0){}

string Episode::toString() const {
    string seasonS=to_string(season);
    string episodeS=to_string(episode);
    if(season<10)
        seasonS="0"+seasonS;
    if(episode<10)
        episodeS="0"+episodeS;
    return seriesName + " " + "S" + seasonS +"E"+episodeS;
}

Watchable * Episode::getNextWatchable(Session & sess) const {
    if(isLast)
        return sess.getActiveUser()->getRecommendation(sess) ;
    else
        return sess.getContent(getId()+1);
}

void Episode::setLast() {isLast=true;}


Episode* Episode::clone() {
    return  new Episode(*this);
}




