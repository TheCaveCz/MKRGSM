/*
  This file is part of the MKR GSM library.
  Copyright (C) 2018 Darren Jeacocke

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "GSMSecurity.h"

GSMSecurity::GSMSecurity(int id) :
  _id(id)
{
}

GSMSecurity::~GSMSecurity()
{
}

void GSMSecurity::begin()
{
  MODEM.begin();
}

int GSMSecurity::setValidation(int val)
{
  MODEM.sendf("AT+USECPRF=%d,0,%d", _id, val);
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMSecurity::setVersion(int val)
{
  MODEM.sendf("AT+USECPRF=%d,1,%d", _id, val);
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMSecurity::setCipher(int val)
{
  MODEM.sendf("AT+USECPRF=%d,2,%d", _id, val);
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMSecurity::setRootCertificate(const char* name)
{
  return setCertificate(SSL_ROOT_CERT, name);
}

int GSMSecurity::setClientCertificate(const char* name)
{
  return setCertificate(SSL_CLIENT_CERT, name);
}

int GSMSecurity::setPrivateKey(const char* name)
{
  return setCertificate(SSL_PRIVATE_KEY, name);
}

int GSMSecurity::setCertificate(int type, const char* name)
{
  int opCode;
  switch (type) {
  case SSL_ROOT_CERT:
    opCode = 3;
    break;
  case SSL_CLIENT_CERT:
    opCode = 5;
    break;
  case SSL_PRIVATE_KEY:
    opCode = 6;
    break;
  default:
    return -1;
  }

  MODEM.sendf("AT+USECPRF=%d,%d,\"%s\"", _id, opCode, name);
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}
