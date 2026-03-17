#include "Server.hpp"

/*
Fonction pour mettre en majuscules une chaîne de caractères

Utile pour le parsing des commandes */
std::string str_toupper(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
		str[i] = std::toupper(str[i]);
	return str;
}

/*
Fonction pour trim uniquement le début d'une chaine de caractères.

On cherche l'index du premier caractère qui n'est pas un whitespace
-si on en trouve, on renvoie toute la chaine après cet index
-sinon en renvoie une chaine de caractères vide (l'input contient
alors uniquement des whitespaces) */
std::string ft_trim(const std::string& str)
{
	if (str.empty())
		return "";

	// On cherche le premier caractère "utile"
	size_t first = str.find_first_not_of(" \t\n\r\v\f");
	if (first == std::string::npos)
		return "";

	// On cherche le dernier caractère "utile"
	size_t last = str.find_last_not_of(" \t\n\r\v\f");

	return str.substr(first, (last - first + 1));
}

/*
Fonction pour tokeniser la ligne d'input, renvoie un vector

Les délimiteurs sont les espaces ' ' et le premier ':' recontré s'il
existe (qui définit le trailing).
Le ':' doit être précédé d'un espace et va faire de tout ce qui suit un
seul bloc (même si ce bloc contient d'autres ':')

-on commence par trim la ligne d'input
-on boucle sur cette ligne trim jusqu'à ce qu'elle soit vide ou qu'on arrive
à la fin
-si le premier caractère est ':' on met tout ce qui suit dans le vecteur,
le caractère ':' exclu, et on return tout de suite
-sinon on cherche le prochain espace de la ligne
-s'il n'y a plus d'espace, on push tout le reste de la string dans le vecteur
-sinon on push les caractères jusqu'à l'index de l'espace qui a été trouvé
-on cherche l'index du premier caractère suivant qui n'est pas un espace (début du
mot ou de la chaine suivant)
-si on ne trouve pas de caractère différent de l'espace, on arrête la boucle
-sinon on update notre temp en skippant tous les caractères déjà mis dans le vecteur
 */
std::vector<std::string> ft_split(const std::string& line)
{
	std::vector<std::string> tokens;
	std::string temp = ft_trim(line);

	while (!temp.empty())
	{
		if (temp[0] == ':')
		{
			tokens.push_back(temp.substr(1));
			return tokens;
		}
		size_t space_pos = temp.find(" ");
		if (space_pos == std::string::npos)
		{
			tokens.push_back(temp);
			break ;
		}
		tokens.push_back(temp.substr(0, space_pos));
		size_t next_word = temp.find_first_not_of(" ", space_pos);
		if (next_word == std::string::npos)
			break ;
		temp = temp.substr(next_word);
	}
	return tokens;
}

std::string getReadableTime(time_t rawtime)
{
	char buffer[80];
	struct tm * timeinfo;

	timeinfo = std::localtime(&rawtime); // Convertit le timestamp en structure (heure, min, sec...)
	std::strftime(buffer, 80, "%d/%m/%Y %H:%M:%S", timeinfo);
	return std::string(buffer);
}
