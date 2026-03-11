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

    public:
        Client() : _fd(-1) {} // important permet à std::map::operator[] de creer automatiquement un objet Client() quand la cle n’existe pas encore
        Client(int fd);
        ~Client();

        void appendToInputBuffer(const char *data, int bytes);
        bool hasCompleteLine() const;
        std::string extractLine();

        const std::string &getInputBuffer() const;
        const std::string &getOutputBuffer() const;
};

#endif