#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class Client;
class Server;

class Channel
{
	private:
		std::string			_name;
		std::vector<Client *>	_members;
		std::vector<Client *>	_operators;
		std::vector<Client *>	_invited;
		std::string			_topic;
		std::string			_key;
		bool				_inviteOnly;
		bool				_topicRestricted;
		bool				_hasKey;
		bool				_hasLimit;
		std::size_t			_limit;

		Channel(const Channel &other);
		Channel &operator=(const Channel &other);

	public:
		Channel(const std::string &name);
		~Channel();

		const std::string &getName() const;
		const std::string &getTopic() const;
		void setTopic(const std::string &topic);

		void addMember(Client &client);
		void removeMember(Client &client);
		bool hasMember(const Client &client) const;
		bool empty() const;
		std::size_t memberCount() const;
		const std::vector<Client *> &getMembers() const;

		void addOperator(Client &client);
		void removeOperator(Client &client);
		bool isOperator(const Client &client) const;

		void invite(Client &client);
		void removeInvite(Client &client);
		bool isInvited(const Client &client) const;

		bool isInviteOnly() const;
		void setInviteOnly(bool value);
		bool isInvitedOnly() const;
		void setInvitedOnly(bool value);
		bool isTopicRestricted() const;
		void setTopicRestricted(bool value);
		bool hasKey() const;
		const std::string &getKey() const;
		void setKey(const std::string &key);
		void clearKey();
		bool hasLimit() const;
		std::size_t getLimit() const;
		void setLimit(std::size_t limit);
		void clearLimit();
		std::string modeString() const;

		void broadcast(Server &server, const std::string &message, const Client *excluded) const;
};

#endif
