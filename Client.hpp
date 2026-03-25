#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <map>
#include <set>

class Channel;

class Client
{
    private:
        int _fd;
        std::string _inputBuffer;   // stocke les donnees recues via recv()
        std::string _outputBuffer;  // stocke les donnees a envoyer plus tard (lignes completes)

            std::string	_nickname;
            std::string _username;
            std::string	_realname;
            bool		_hasNickname;
            bool		_hasPassword;
            bool		_hasUser;
            bool		_isRegistered;
            std::set<Channel*>  _joinedChannels;

    public:
        Client() : _fd(-1) {} // important permet à std::map::operator[] de creer automatiquement un objet Client() quand la cle n’existe pas encore
        Client(int fd);
        ~Client();

        void appendToInputBuffer(const char *data, int bytes);
        void appendToOutputBuffer(const std::string& data);    
        bool hasCompleteLine() const;
        std::string extractLine();

        std::string &getInputBuffer();
        std::string &getOutputBuffer();

            // Getters and setters
                std::string	getNickname(void) const;
                std::string getUsername(void) const;
                int			getFd(void) const;
                bool		getHasPass(void) const;
                bool		getHasNickname(void) const;
                bool		getHasUser(void) const;
                bool		getIsRegistered(void) const;

                void		setNickname(const std::string& nick);
                void		setUsername(const std::string& username);
                void		setRealname(const std::string& realname);

                // Registration
                void		setHasPass(bool newStatus);
                void		setHasNickname(bool newStatus);
                void		setHasUser(bool newStatus);
                void		setIsRegistered(bool newStatus);

                // Channels set handling
                void        addChannel(Channel* chan);
                void        removeChannel(Channel* chan);
                const std::set<Channel*>& getChannels(void) const;
};

#endif
