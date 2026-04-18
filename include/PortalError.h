#ifndef PORTAL_ERROR_H
#define PORTAL_ERROR_H

#include <string>
#include <map>

extern const std::map<std::string, std::string> PORTAL_ERRORS;

std::string getPortalErrorMessage(const std::string& ecode);

#endif
