#include "../header/Response.hpp"

void Response::urlDecodedInput()
{
	std::string tmp(_info._input.begin(), _info._input.end());
	size_t foundPos = tmp.find("textData=");
	std::string testData;
	if (foundPos != std::string::npos)
	{
		std::cout << "input: "<<tmp<<std::endl;
		size_t endPos = tmp.find("\0", foundPos);// CHanged from ; to ' '
		if (endPos != std::string::npos)
		{
			testData = tmp.substr(foundPos + 9, endPos - (foundPos + 9));

			std::cout << GRN"DEBUG: Text data: " << testData << ""RESET<< std::endl;
		}
	}

	while(true)
	{	
		
		std::string decodedInput = decodeURL(testData);
		// std::cout << "filename: "<<filename<<std::endl;
		_fileStreams[_info._clientSocket].open((UPLOAD_FOLDER + _info._postInfo._filename).c_str());
		// _fileStreams[_info._clientSocket].write((testData.c_str()),testData.size());
		_fileStreams[_info._clientSocket].write((decodedInput.c_str()),strlen(decodedInput.c_str()));
		_fileStreams[_info._clientSocket].close();
	}
	mySend(FILE_SAVED);
}



std::string Response::decodeURL(const std::string& input)
{
    std::string output;
    for (size_t i = 0; i < input.length(); ++i) 
	{
        if (input[i] == '%' && i + 2 < input.length()) 
		{
            int value;
            if (std::sscanf(input.substr(i + 1, 2).c_str(), "%x", &value) == 1) 
			{
                output += static_cast<char>(value);
                i += 2;
            } 
			else 
                output += input[i];
        } 
		else if (input[i] == '+') 
            output += ' ';
        else
            output += input[i];
    }
    return output;
}