#include "Rulerunner.h"
using std::string;
#include "Cmd.h"

#include <memory>
#include <stdexcept>
using std::logic_error;


#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

bool Rulerunner::isDeepEnough() {
    return _rulestates.size() > _maxdepth || _turtle.getscale() < _minscale;
}

void Rulerunner::handlerule(const string &rr, bool rulerev, bool ruleflip, double localscale) {
    const auto & rule = _therules[rr];
    bool willflip = rulerev ^ ruleflip;
    if (isDeepEnough()) {
        _agraphic = _turtle.draw(rule,willflip?-1.0:1.0,localscale);
    } else {
        _backwards ^= rulerev;
        _rulestates.push(Rulestate(&rule, _backwards, _turtle.getscale(), willflip));

        if (willflip)
            _turtle.flip();
        _turtle.scaleby(localscale * rule.cachedscalefac);//localscale is A@ 2 notation, cachedscalefac is A ? localscale 1/sqrt(2) notation
    }
}

void Rulestate::doit(Rulerunner *towho) {//!!! Need to wrap rule so this can use reverse iterators
    if (backwards)
        (*--mypos)->execute(towho);
    else
        (*mypos++)->execute(towho);
}

std::shared_ptr<Graphic> Rulerunner::nextpoint() {
    if (_agraphic == nullptr) //!!! Should this be an assert?
        throw logic_error("Called nextpoint() on a Rulerunner with no graphic ready\n");
    std::shared_ptr<Graphic> temp(_agraphic);
    makeapoint();
    return temp;
}

void Rulerunner::makeapoint() {
    while (_agraphic == nullptr) {
        if (_rulestates.top().done()) {
            _turtle.setscale(_rulestates.top().oldscale);
            if (_rulestates.top().flipped)
                _turtle.flip();
            _rulestates.pop();
            if (_rulestates.empty()) {
                _finished = true;
                return;
            }
            _backwards = !_rulestates.empty() && _rulestates.top().backwards;
        } else
            _rulestates.top().doit(this);
    }
}

void Rulerunner::drawnextpoint() {
#ifdef _DEBUG
    if (_agraphic == nullptr)
        throw logic_error("Called drawnextpoint() on a Rulerunner with no graphic ready\n");
#endif
    _agraphic->draw();
    _agraphic.reset();
    makeapoint();
}
