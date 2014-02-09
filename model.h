#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <map>
#include <assert.h>

class object {
private:
    std::string name;
public:

    object():
        name()
    {}

    explicit object(const std::string& _name):
        name(_name)
    {}

    operator std::string () const {
        return name;
    }

    bool operator ==(const object& other) const {
        return name == other.name;
    }
};

std::ostream& operator <<(std::ostream& os, const object& o) {
    os<<"o{"<<static_cast<std::string>(o)<<"}";
    return os;
}

class varaible {
private:
    int idx;
public:
    explicit varaible(int _idx):
        idx(_idx)
    {}

    int index() const {
        return idx;
    }
};

std::ostream& operator <<(std::ostream& os, const varaible& v) {
    os<<"v{"<<v.index()<<"}";
}

typedef std::vector<object> objects;
typedef std::vector<varaible> varaibles;

template<typename _ground_relation>
class _state {
private:

    typename std::vector<_ground_relation>::const_iterator findRelation(const _ground_relation& r) const{
        return std::find_if(
                    heldrelations.begin(),
                    heldrelations.end(),
                    [&](const _ground_relation& o){return o==r;});
    }

    typename std::vector<_ground_relation>::iterator findRelation(const _ground_relation& r) {
        return std::find_if(
                    heldrelations.begin(),
                    heldrelations.end(),
                    [&](const _ground_relation& o){return o==r;});
    }

    bool relationExists(const _ground_relation& r) const{
        return  findRelation(r) != heldrelations.end();
    }


public:
    std::vector<_ground_relation > heldrelations;

    _state():
        heldrelations()
    {}


    _state<_ground_relation> add(const _ground_relation& r) const {
        _state<_ground_relation> tmp(*this);
        if(!relationExists(r)){
            tmp.heldrelations.push_back(r);
        }
        return tmp;
    }

    _state<_ground_relation> remove(const _ground_relation& r) const {
        _state<_ground_relation> tmp(*this);
        if(relationExists(r))
            tmp.heldrelations.erase(tmp.findRelation(r));
        return tmp;
    }

};

template <typename _relation>
class _ground_relation: std::unary_function<const _state<_ground_relation<_relation> >&, bool> {
private:
    _relation rel;
    objects obj;
public:
    _ground_relation(const _relation& _rel, const objects& _obj):
        rel(_rel),
        obj(_obj)
    {}

    bool operator ()(const _state<_ground_relation<_relation> >& s) {
        return rel(s, obj);
    }

    bool operator ==(_ground_relation<_relation> other) const {
        return rel == other.rel && obj == other.obj;
    }


    friend class relation;
};
class relation;
typedef _ground_relation<relation> ground_relation;
typedef _state<ground_relation> state;


class relation: std::binary_function<const state&, const objects&, bool> {
private: 
    std::string name;
public:

    relation(const std::string _name):
        name(_name)
    {}

    bool operator()(const state& s, const objects& o) const {
        

        auto nameAndOperandsMatched = [&](const ground_relation& relationHeldOnObjects) {
            bool nameMatched = name == relationHeldOnObjects.rel.name;
			bool operandsMatched = relationHeldOnObjects.obj == o;
            return nameMatched && operandsMatched;
        };
        
        return std::find_if(
                    s.heldrelations.begin(),
                    s.heldrelations.end(),
                    nameAndOperandsMatched
                    ) != s.heldrelations.end();
    }

    bool operator ==(const relation& other) const {
        return other.name == name;
    }

    operator std::string() const{
        return name;
    }
};


typedef relation precondition;

class effect: std::unary_function<const state&, state> {
private:
	relation rel;
    objects  obj;
	bool positive;
	
public:
    effect(const relation& _rel, const objects& _obj, bool _positive = true):
        rel(_rel),
        obj(_obj),
        positive(_positive)
    {}

    state operator()(const state& from) {
        if(positive){
            return from.add(ground_relation(rel, obj));
        } else {
            return from.remove(ground_relation(rel, obj));
        }
    }
	
};

template <typename T>
using binder = std::pair<T, varaibles>;

typedef std::vector<binder<precondition> > preconditions;
typedef std::vector<binder<effect> > effects;

class operation: std::binary_function<const state&, const objects&, state> {
    preconditions pre;
    effects eff;

    static objects bind_varaibles(objects o, varaibles v) {
        objects bound(v.size());

        std::transform(
                    v.begin(),
                    v.end(),
                    bound.begin(),
                    [&](const varaible& vv){
                        assert(vv.index() < o.size());
                        return o[vv.index()];
                    }
                    );

        return bound;
    }

public:
    operation(const preconditions& _pre, const effects& _eff):
        pre(_pre),
        eff(_eff)
    {}

    bool isDefined(const state& s, const objects& o) {
        //failed precondition not found
        //thus all passed and return true
        assert(s.heldrelations.size()>0);
        return std::find_if(
                    pre.begin(),
                    pre.end(),
                    [&](const binder<precondition>& p){
                        return !p.first(s, bind_varaibles(o, p.second));
                    }
                    ) == pre.end();

    }

    state operator ()(const state& s, const objects& o) {
        //TODO implement
        throw std::string("Not implemented");
    }

};

#endif // MODEL_H
