#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>  
#include <map>

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

    public:
        Client() : _fd(-1) {} // important permet à std::map::operator[] de creer automatiquement un objet Client() quand la cle n’existe pas encore
        Client(int fd);
        ~Client();

        void appendToInputBuffer(const char *data, int bytes);
        bool hasCompleteLine() const;
        std::string extractLine();

        const std::string &getInputBuffer() const;
        const std::string &getOutputBuffer() const;

            
            // Getters and setters
                std::string	getNickname(void) const;
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
};

#endif