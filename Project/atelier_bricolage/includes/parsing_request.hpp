#ifndef PARSING_REQUEST_HPP
# define PARSING_REQUEST_HPP

# include <iostream>
# include <map>

class request {


	private :

		std::map<std::string, std::string> init_default_error();
		std::map<std::string, std::string> init_file_type();
};


#endif