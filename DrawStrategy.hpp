//
//  DrawStrategy.hpp
//  lsystems
//
//  Created by Chris Hartman on 4/20/18.
//

#ifndef DrawStrategy_hpp
#define DrawStrategy_hpp

#include "Parsenode.h"
#include "Turtle.h"
#include <memory>
#include <vector>
class Rulerunner;

class DrawStrategy {
public:
    virtual ~DrawStrategy() = default;
    DrawStrategy() = default;
    DrawStrategy(const DrawStrategy&) = delete;
    DrawStrategy& operator=(const DrawStrategy&) = delete;
    DrawStrategy(DrawStrategy&&) = delete;
    DrawStrategy& operator=(DrawStrategy&&) = delete;

    void draw(Rulerunner& ruleRunner, const Rule &rule, bool ruleFlip, double atScale);
    virtual void start()=0;
    virtual void finish()=0;
private:
    virtual void drawImpl(const Rulerunner& ruleRunner, const Motion &m, bool ruleFlip) =0;
};

using DrawStrategyPtr = std::unique_ptr<DrawStrategy>;

DrawStrategyPtr drawStrategyFactory(std::string_view type, std::vector<ParsenodePtr> &&parameters = {});

class LinesDrawStrategy : public DrawStrategy {
public:
    void start() override;
    void finish() override;
private:
    void drawImpl(const Rulerunner& ruleRunner, const Motion &m, bool ruleFlip) override;
};
//!!! eventually might want rules to be able to start and stop drawing
//so an invisible rule will call start and stop so we know when to do glBegin etc.

class DropDrawStrategy : public DrawStrategy {
public:
    DropDrawStrategy(ParsenodePtr dropAngleExpression, ParsenodePtr dropDistanceExpression);
    void start() override;
    void finish() override;
private:
    void drawImpl(const Rulerunner& ruleRunner, const Motion &m, bool ruleFlip) override;

    ParsenodePtr _dropAngleExpression;
    ParsenodePtr _dropDistanceExpression;
    Point _lastDropped;
    bool _hasDroppedPoint=false;
};





#endif /* DrawStrategy_hpp */
