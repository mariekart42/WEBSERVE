#include "../header/utils.h"

void logg(const std::string &message)
{
	std::cout << message << std::endl;
}

void exitWithError(const std::string &msg)
{
	logg(RED"ERROR: " + msg + RESET);
	exit(1);
}


// if-statements for single characters so shit is not super slow
std::string comparerContentType(const std::string& fileExtension)
{
    if (fileExtension[0] == 'a') {
        if (fileExtension == "aac") return ("audio/aac");
        if (fileExtension == "abw") return ("application/x-abiword");
        if (fileExtension == "arc") return ("application/x-freearc");
        if (fileExtension == "avif") return ("image/avif");
        if (fileExtension == "avi") return ("video/x-msvideo");
        if (fileExtension == "azw") return ("application/vnd.amazon.ebook");}
    if (fileExtension[0] == 'b') {
        if (fileExtension == "bin") return ("application/octet-stream");
        if (fileExtension == "bmp") return ("image/bmp");
        if (fileExtension == "bz") return ("application/x-bzip");
        if (fileExtension == "bz2") return ("application/x-bzip2");}
    if (fileExtension[0] == 'c') {
        if (fileExtension == "cda") return ("application/x-cdf");
        if (fileExtension == "csh") return ("application/x-csh");
        if (fileExtension == "css") return ("text/css");
        if (fileExtension == "csv") return ("text/csv");}
    if (fileExtension[0] == 'd') {
        if (fileExtension == "doc") return ("application/msword");
        if (fileExtension == "docx") return ("application/vnd.openxmlformats-officedocument.wordprocessingml.document");}
    if (fileExtension[0] == 'e') {
        if (fileExtension == "eot") return ("application/vnd.ms-fontobject");
        if (fileExtension == "epub") return ("application/epub+zip");}
    if (fileExtension[0] == 'g') {
        if (fileExtension == "gz") return ("application/gzip");
        if (fileExtension == "gif") return ("image/gif");}
    if (fileExtension[0] == 'h') {
        if (fileExtension == "html") return ("text/html");
        if (fileExtension == "htm") return ("text/html");}
    if (fileExtension[0] == 'i') {
        if (fileExtension == "ico") return ("image/vnd.microsoft.icon");
        if (fileExtension == "ics") return ("text/calendar");}
    if (fileExtension[0] == 'j') {
        if (fileExtension == "jar") return ("application/java-archive");
        if (fileExtension == "jpeg") return ("image/jpeg");
        if (fileExtension == "jpg") return ("image/jpeg");
        if (fileExtension == "js") return ("text/javascript");
        if (fileExtension == "json") return ("application/json");
        if (fileExtension == "jsonld") return ("application/ld+json");}
    if (fileExtension[0] == 'm') {
        if (fileExtension == "mid") return ("audio/midi");
        if (fileExtension == "midi") return ("audio/midi");
        if (fileExtension == "mid") return ("audio/x-midi");
        if (fileExtension == "midi") return ("audio/x-midi");
        if (fileExtension == "mjs") return ("text/javascript");
        if (fileExtension == "mp3") return ("audio/mpeg");
        if (fileExtension == "mp4") return ("video/mp4");
        if (fileExtension == "mpeg") return ("video/mpeg");
        if (fileExtension == "mpkg") return ("application/vnd.apple.installer+xml");}
    if (fileExtension[0] == 'o') {
        if (fileExtension == "odp") return ("application/vnd.oasis.opendocument.presentation");
        if (fileExtension == "ods") return ("application/vnd.oasis.opendocument.spreadsheet");
        if (fileExtension == "odt") return ("application/vnd.oasis.opendocument.text");
        if (fileExtension == "oga") return ("audio/ogg");
        if (fileExtension == "ogv") return ("video/ogg");
        if (fileExtension == "ogx") return ("application/ogg");
        if (fileExtension == "opus") return ("audio/opus");
        if (fileExtension == "otf") return ("font/otf");}
    if (fileExtension[0] == 'p') {
        if (fileExtension == "png") return ("image/png");
        if (fileExtension == "pdf") return ("application/pdf");
        if (fileExtension == "php") return ("application/x-httpd-php");
        if (fileExtension == "ppt") return ("application/vnd.ms-powerpoint");
        if (fileExtension == "pptx") return ("application/vnd.openxmlformats-officedocument.presentationml.presentation");}
    if (fileExtension[0] == 'r') {
        if (fileExtension == "rar") return ("application/vnd.rar");
        if (fileExtension == "rtf") return ("application/rtf");}
    if (fileExtension[0] == 's') {
        if (fileExtension == "sh") return ("application/x-sh");
        if (fileExtension == "svg") return ("image/svg+xml");}
    if (fileExtension[0] == 't') {
        if (fileExtension == "tar") return ("application/x-tar");
        if (fileExtension == "tiff") return ("image/tiff");
        if (fileExtension == "tif") return ("image/tiff");
        if (fileExtension == "ts") return ("video/mp2t");
        if (fileExtension == "ttf") return ("font/ttf");
        if (fileExtension == "txt") return ("text/plain");}
    if (fileExtension[0] == 'v')
        if (fileExtension == "vsd") return ("application/vnd.visio");
    if (fileExtension[0] == 'w') {
        if (fileExtension == "wav") return ("audio/wav");
        if (fileExtension == "weba") return ("audio/webm");
        if (fileExtension == "webm") return ("video/webm");
        if (fileExtension == "webp") return ("image/webp");
        if (fileExtension == "woff") return ("font/woff");
        if (fileExtension == "woff2") return ("font/woff2");}
    if (fileExtension[0] == 'x') {
        if (fileExtension == "xhtml") return ("application/xhtml+xml");
        if (fileExtension == "xls") return ("application/vnd.ms-excel");
        if (fileExtension == "xlsx") return ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
        if (fileExtension == "xml") return ("application/xml");
        if (fileExtension == "xul") return ("application/vnd.mozilla.xul+xml");}
    else {
        if (fileExtension == "zip") return ("application/zip");
        if (fileExtension == "3gp") return ("video/3gpp");
        if (fileExtension == "3g2") return ("video/3gpp2");
        if (fileExtension == "7z") return ("application/x-7z-compressed");}
    return ("FAILURE");
}