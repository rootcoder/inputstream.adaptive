/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include "../../common/AdaptiveCencSampleDecrypter.h"
#include "../IDecrypter.h"

#include <map>

#include <bento4/Ap4.h>

class CWVCdmAdapterA;
class CWVDecrypterA;

class ATTR_DLL_LOCAL CWVCencSingleSampleDecrypterA : public Adaptive_CencSingleSampleDecrypter
{
public:
  // methods
  CWVCencSingleSampleDecrypterA(CWVCdmAdapterA& drm,
                               AP4_DataBuffer& pssh,
                               const char* optionalKeyParameter,
                               std::string_view defaultKeyId,
                               CWVDecrypterA* host);
  virtual ~CWVCencSingleSampleDecrypterA();

  bool StartSession(bool skipSessionMessage) { return KeyUpdateRequest(true, skipSessionMessage); };
  const std::vector<char>& GetSessionIdRaw() { return m_sessionId; };
  virtual const char* GetSessionId() override;
  std::vector<char> GetChallengeData();
  virtual bool HasLicenseKey(const uint8_t* keyId);

  virtual AP4_Result SetFragmentInfo(AP4_UI32 poolId,
                                     const AP4_UI08* key,
                                     const AP4_UI08 nalLengthSize,
                                     AP4_DataBuffer& annexbSpsPps,
                                     AP4_UI32 flags,
                                     CryptoInfo cryptoInfo) override;
  virtual AP4_UI32 AddPool() override;
  virtual void RemovePool(AP4_UI32 poolId) override;

  virtual AP4_Result DecryptSampleData(
      AP4_UI32 poolId,
      AP4_DataBuffer& dataIn,
      AP4_DataBuffer& dataOut,

      // always 16 bytes
      const AP4_UI08* iv,

      // pass 0 for full decryption
      unsigned int subsampleCount,

      // array of <subsample_count> integers. NULL if subsample_count is 0
      const AP4_UI16* bytesOfCleartextData,

      // array of <subsample_count> integers. NULL if subsample_count is 0
      const AP4_UI32* bytesOfEncryptedData) override;

  void GetCapabilities(const uint8_t* keyId,
                       uint32_t media,
                       DRM::IDecrypter::DecrypterCapabilites& caps);

  void RequestNewKeys() { m_isKeyUpdateRequested = true; };

private:
  bool ProvisionRequest();
  bool GetKeyRequest(std::vector<char>& keyRequestData);
  bool KeyUpdateRequest(bool waitForKeys, bool skipSessionMessage);
  bool SendSessionMessage(const std::vector<char>& keyRequestData);

  CWVCdmAdapterA& m_mediaDrm;
  std::vector<char> m_pssh;
  std::vector<char> m_initialPssh;
  std::map<std::string, std::string> m_optParams;
  CWVDecrypterA* m_host;

  std::vector<char> m_sessionId;
  std::vector<char> m_keySetId;
  std::vector<char> m_keyRequestData;

  char m_sessionIdChar[128];
  bool m_isProvisioningRequested;
  bool m_isKeyUpdateRequested;

  std::string m_defaultKeyId;

  struct FINFO
  {
    const AP4_UI08* m_key;
    AP4_UI08 m_nalLengthSize;
    AP4_UI16 m_decrypterFlags;
    AP4_DataBuffer m_annexbSpsPps;
  };
  std::vector<FINFO> m_fragmentPool;
  int m_hdcpLimit;
  int m_resolutionLimit;
};
