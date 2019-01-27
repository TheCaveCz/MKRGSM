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

#ifndef _CERTIFICATES_INCLUDED_H
#define _CERTIFICATES_INCLUDED_H


#include <Arduino.h>

#include "Modem.h"

class GSMCertificates
{

public:
  GSMCertificates();
  virtual ~GSMCertificates();

  int list();

  int getNumCertificates();
  int getCertificateType(int id);
  String getCertificateName(int id);

  int indexOf(const char* name);
  int indexOf(String& name) { return indexOf(name.c_str()); }

  bool hasCertificate(const char * name);
  bool hasCertificate(String& name) { return hasCertificate(name.c_str()); }

  int save(int type, const char* name, const char* data, size_t len);
  int save(int type, String& name, const char* data, size_t len) { return save(type, name.c_str(), data, len); }

  int remove(int type, const char* name);
  int remove(int type, String& name) { return remove(type, name.c_str()); }

  int removeAll();

private:
  int typeFromCode(String& code);

  #define GSM_MAX_CERTS 16
  #define GSM_MAX_CERT_NAME_LEN 32
  uint8_t _certCount;
  struct {
    int type;
    char name[GSM_MAX_CERT_NAME_LEN];
  } _certs[GSM_MAX_CERTS];
};

#endif
