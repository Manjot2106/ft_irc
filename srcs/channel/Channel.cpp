#include "IRC.hpp"

Channel::Channel(const std::string &name)
	: _name(name), _inviteOnly(false), _topicRestricted(false),
	  _hasKey(false), _hasLimit(false), _limit(0)
{
}

Channel::~Channel()
{
}

const std::string &Channel::getName() const { return (_name); }
const std::string &Channel::getTopic() const { return (_topic); }
void Channel::setTopic(const std::string &topic) { _topic = topic; }

void Channel::addMember(Client &client)
{
	if (!hasMember(client))
		_members.push_back(&client);
}

void Channel::removeMember(Client &client)
{
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it == &client)
		{
			_members.erase(it);
			break;
		}
	}
	removeOperator(client);
	removeInvite(client);
}

bool Channel::hasMember(const Client &client) const
{
	return (std::find(_members.begin(), _members.end(), &client) != _members.end());
}

bool Channel::empty() const { return (_members.empty()); }
std::size_t Channel::memberCount() const { return (_members.size()); }
const std::vector<Client *> &Channel::getMembers() const { return (_members); }

void Channel::addOperator(Client &client)
{
	if (hasMember(client) && !isOperator(client))
		_operators.push_back(&client);
}

void Channel::removeOperator(Client &client)
{
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (*it == &client)
		{
			_operators.erase(it);
			break;
		}
	}
}

bool Channel::isOperator(const Client &client) const
{
	return (std::find(_operators.begin(), _operators.end(), &client) != _operators.end());
}

void Channel::invite(Client &client)
{
	if (!isInvited(client))
		_invited.push_back(&client);
}

void Channel::removeInvite(Client &client)
{
	for (std::vector<Client *>::iterator it = _invited.begin(); it != _invited.end(); ++it)
	{
		if (*it == &client)
		{
			_invited.erase(it);
			break;
		}
	}
}

bool Channel::isInvited(const Client &client) const
{
	return (std::find(_invited.begin(), _invited.end(), &client) != _invited.end());
}

bool Channel::isInviteOnly() const { return (_inviteOnly); }
void Channel::setInviteOnly(bool value) { _inviteOnly = value; }
bool Channel::isInvitedOnly() const { return (isInviteOnly()); }
void Channel::setInvitedOnly(bool value) { setInviteOnly(value); }
bool Channel::isTopicRestricted() const { return (_topicRestricted); }
void Channel::setTopicRestricted(bool value) { _topicRestricted = value; }
bool Channel::hasKey() const { return (_hasKey); }
const std::string &Channel::getKey() const { return (_key); }

void Channel::setKey(const std::string &key)
{
	_key = key;
	_hasKey = true;
}
void Channel::clearKey()
{
	_key.clear();
	_hasKey = false;
}
bool Channel::hasLimit() const { return (_hasLimit); }
std::size_t Channel::getLimit() const { return (_limit); }
void Channel::setLimit(std::size_t limit)
{
	_limit = limit;
	_hasLimit = true;
}
void Channel::clearLimit()
{
	_limit = 0;
	_hasLimit = false;
}

std::string Channel::modeString() const
{
	std::string modes = "+";
	if (_inviteOnly)
		modes += "i";
	if (_topicRestricted)
		modes += "t";
	if (_hasKey)
		modes += "k";
	if (_hasLimit)
		modes += "l";
	if (modes == "+")
		modes.clear();
	return (modes);
}

void Channel::broadcast(Server &server, const std::string &message, const Client *excluded) const
{
	for (std::vector<Client *>::const_iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it != excluded)
			server.sendMessage((*it)->getFd(), message);
	}
}
