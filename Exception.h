/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

class Exception : public std::exception
{
protected:
    std::string _message;

public:
    Exception(const std::string& message)
        : _message(message)
    {}

    ~Exception() throw(){}

    virtual const char* what() const throw()
    {
        return _message.c_str();
    }
};

#endif // EXCEPTION_H
