#include "Cmd.h"
#include "Rulerunner.h"
#include "Turtle.h"

DrawStrategy & Command::artist(const Rulerunner& rulerunner) const
{
    return *rulerunner._drawStrategy;
}

void Command::evaluateExpressions(const Context& /*context*/) const
{}

RotateCommand::RotateCommand(std::unique_ptr<Parsenode> _a) :
		_angleExpression(std::move(_a)) {
}

void RotateCommand::executeOn(Rulerunner& target, int /* unused */) const {
	artist(target).rotate(_angle);
}

void RotateCommand::evaluateExpressions(const Context& context) const{
	_angle = _angleExpression->eval(context);
}

void FlipCommand::executeOn(Rulerunner& target, int /* unused */) const
{
	artist(target).flip();
}

void PushCommand::executeOn(Rulerunner& target, int /* unused */) const
{
	artist(target).push();
}

void PopCommand::executeOn(Rulerunner& target, int /* unused */) const
{
	artist(target).pop();
}

RuleCommand::RuleCommand(std::string_view ruleName,
		bool isReversed, bool isFlipped, ParsenodePtr scaleExpression) :
		_ruleName(ruleName), _isReversed(isReversed), _isFlipped(isFlipped),
        _scaleExpression(std::move(scaleExpression)) {
}

void RuleCommand::executeOn(Rulerunner& target, int depth) const
{
    const auto & rule = target.getRules().at(_ruleName);
    
    if (depth == 0) {
        target._drawStrategy->draw(rule,_isFlipped,_atScale);
        return;
    }
    
    auto oldScale = artist(target).getScale();
    artist(target).scaleby(_atScale*rule.getLocalScale());
    if (_isFlipped)
        artist(target).flip();
    target._backwards ^= _isReversed; //NOLINT

    if(target._backwards)
        for(auto i = rule.getCommands().rbegin(); i != rule.getCommands().rend(); ++i)
            (*i)->executeOn(target,depth-1);
    else
        for(auto &i : rule.getCommands())
            i->executeOn(target,depth-1);

    target._backwards ^= _isReversed; //NOLINT
    if (_isFlipped)
        artist(target).flip();
    artist(target).setScale(oldScale);
}

void RuleCommand::evaluateExpressions(const Context& context) const {
	_atScale = _scaleExpression->eval(context);
}
