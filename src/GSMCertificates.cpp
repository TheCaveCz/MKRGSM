/*
  This file is part of the MKR GSM library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

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

#include "GSMCertificates.h"
#include "GSMSecurity.h"

GSMCertificates::GSMCertificates():
_certCount(0)
{
}

GSMCertificates::~GSMCertificates()
{
}

int GSMCertificates::typeFromCode(String& code) {
  if (code == "PK")
    return SSL_PRIVATE_KEY;
  else if (code == "CC")
    return SSL_CLIENT_CERT;
  else if (code == "CA")
    return SSL_ROOT_CERT;
  else
    return 255;
}

int GSMCertificates::list()
{
  String response;

  MODEM.sendf("AT+USECMNG=3");
  if (MODEM.waitForResponse(2000, &response) != 1) {
    return 0;
  }

  String buffer;
  buffer.reserve(32);

  int state = 0;
  int responseLen = response.length();
  int currentCert = 0;
  _certCount = 0;
  char c;
  for (int i=0; i<responseLen; i++) {
    c = response[i];
    if (c == '\r') continue;

    switch (state) {
      case 0:
        if (c == ',') {
          int type = typeFromCode(buffer);
          if (type == 255) {
            return 0;
          }
          _certs[currentCert].type = type;
          buffer = "";
          state = 1;
        } else if (c != '\n') {
          buffer += c;
        }
        break;

      case 1:
        if (c != '"') {
          return 0;
        }
        state = 2;
        break;

      case 2:
        if (c == '"') {
          strncpy(_certs[currentCert].name, buffer.c_str(), GSM_MAX_CERT_NAME_LEN - 1);
          _certs[currentCert].name[GSM_MAX_CERT_NAME_LEN - 1] = 0;
          buffer = "";
          currentCert++;
          if (currentCert >= GSM_MAX_CERTS) {
            return 0;
          }
          state = 3;
        } else {
          buffer += c;
        }
        break;

      case 3:
        if (c == '\n') {
          state = 0;
        }
        break;
    }
  }
  _certCount = currentCert;
  return 1;
}

int GSMCertificates::getNumCertificates()
{
  return _certCount;
}

int GSMCertificates::getCertificateType(int id)
{
  if (id < _certCount) {
    return _certs[id].type;
  } else {
    return -1;
  }
}

String GSMCertificates::getCertificateName(int id)
{
  if (id < _certCount) {
    return String(_certs[id].name);
  } else {
    return "";
  }
}

bool GSMCertificates::hasCertificate(const char* name)
{
  return indexOf(name) != -1;
}

int GSMCertificates::indexOf(const char* name)
{
  for (uint8_t i = 0; i < _certCount; i++) {
    if (strncmp(_certs[i].name, name, GSM_MAX_CERT_NAME_LEN) == 0) {
      return i;
    }
  }
  return -1;
}

int GSMCertificates::save(int type, const char* name, const char* data, size_t len, bool doList)
{
  MODEM.sendf("AT+USECMNG=0,%d,\"%s\",%d", type, name, len);
  if (MODEM.waitForPrompt() != 1) {
    return 0;
  }

  // send the cert contents
  MODEM.write((const uint8_t*)data, len);

  if (MODEM.waitForResponse(2000) != 1) {
    return 0;
  }

  if (doList) {
    list();
  }

  return 1;
}

int GSMCertificates::remove(int type, const char* name, bool doList)
{
  MODEM.sendf("AT+USECMNG=2,%d,\"%s\"", type, name);
  if (MODEM.waitForResponse(2000) != 1) {
    return 0;
  }

  if (doList) {
    list();
  }

  return 1;
}

int GSMCertificates::removeAll()
{
  for (uint8_t i = 0; i < _certCount; i++) {
    if (remove(_certs[i].type, _certs[i].name, false) != 1) {
      return 0;
    }
  }
  _certCount = 0;
  return 1;
}
