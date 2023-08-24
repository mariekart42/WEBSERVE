std::string lol = "<!DOCTYPE html><html lang=\"en\">           <head>           <meta charset=\"UTF-8\">                         <title></title><script src=\"./config.js\"></script><link rel=\"stylesheet\" href=\"./pretty-autoindex.css\">"
std::string filesList = generateList(path);
std::string lol2 = "</head><body><div class=\"container\"><app></app></div><script src=\"./pretty-autoindex.js\"></script></body></html>";

std::string result = lol + filesList + lol2;