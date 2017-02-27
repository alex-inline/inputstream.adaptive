#pragma once

//Functionality wich is supported by the Decrypter
class AP4_CencSingleSampleDecrypter;
class AP4_DataBuffer;

namespace SSD
{
  struct SSD_PICTURE;

  //Functionality wich is supported by the Addon
  class SSD_HOST
  {
  public:
    enum CURLOPTIONS
    {
      OPTION_PROTOCOL,
      OPTION_HEADER
    };
    static const uint32_t version = 6;

    virtual const char *GetLibraryPath() const = 0;
    virtual const char *GetProfilePath() const = 0;
    virtual void* CURLCreate(const char* strURL) = 0;
    virtual bool CURLAddOption(void* file, CURLOPTIONS opt, const char* name, const char* value) = 0;
    virtual bool CURLOpen(void* file) = 0;
    virtual size_t ReadFile(void* file, void* lpBuf, size_t uiBufSize) = 0;
    virtual void CloseFile(void* file) = 0;
    virtual bool CreateDirectory(const char *dir) = 0;
    virtual bool GetBuffer(void* instance, SSD_PICTURE &picture) = 0;

    enum LOGLEVEL
    {
      LL_DEBUG,
      LL_INFO,
      LL_ERROR
    };

    virtual void Log(LOGLEVEL level, const char *msg) = 0;
  };

  /****************************************************************************************************/
  // keep those values in track with xbmc\addons\kodi-addon-dev-kit\include\kodi\kodi_videocodec_types.h
  /****************************************************************************************************/

  enum SSD_VIDEOFORMAT
  {
    UnknownVideoFormat = 0,
    VideoFormatYV12,
    VideoFormatI420,
    MaxVideoFormats
  };

  struct SSD_VIDEOINITDATA
  {
    enum Codec {
      CodecUnknown = 0,
      CodecVp8,
      CodecH264,
      CodecVp9
    } codec;

    enum CodecProfile
    {
      CodecProfileUnknown = 0,
      CodecProfileNotNeeded,
      H264CodecProfileBaseline,
      H264CodecProfileMain,
      H264CodecProfileExtended,
      H264CodecProfileHigh,
      H264CodecProfileHigh10,
      H264CodecProfileHigh422,
      H264CodecProfileHigh444Predictive
    } codecProfile;

    const SSD_VIDEOFORMAT *videoFormats;

    uint32_t width, height;

    const uint8_t *extraData;
    unsigned int extraDataSize;
  };

  struct SSD_PICTURE
  {
    enum VideoPlane {
      YPlane = 0,
      UPlane,
      VPlane,
      MaxPlanes = 3,
    };

    enum Flags :uint32_t {
      FLAG_DROP,
      FLAG_DRAIN
    };

    SSD_VIDEOFORMAT videoFormat;
    uint32_t flags;

    uint32_t width, height;

    uint8_t *decodedData;
    size_t decodedDataSize;

    uint32_t planeOffsets[VideoPlane::MaxPlanes];
    uint32_t stride[VideoPlane::MaxPlanes];

    int64_t pts;
  };

  typedef struct SSD_SAMPLE
  {
    const uint8_t *data;
    uint32_t dataSize;

    int64_t pts;

    uint16_t numSubSamples; //number of subsamples
    uint16_t flags; //flags for later use

    uint16_t *clearBytes; // numSubSamples uint16_t's wich define the size of clear size of a subsample
    uint32_t *cipherBytes; // numSubSamples uint32_t's wich define the size of cipher size of a subsample

    uint8_t *iv;  // initialization vector
    uint8_t *kid; // key id
  } SSD_SAMPLE;

  enum SSD_DECODE_RETVAL
  {
    VC_NONE = 0,        //< noop
    VC_ERROR,           //< an error occured, no other messages will be returned
    VC_BUFFER,          //< the decoder needs more data
    VC_PICTURE,         //< the decoder got a picture
    VC_EOF,             //< the decoder signals EOF
  };

  class SSD_DECRYPTER
  {
  public:
    enum SSD_CAPS : uint32_t
    {
      SSD_SUPPORTS_DECODING = 1,
      SSD_SECURE_PATH = 2,
      SSD_ANNEXB_REQUIRED = 4,
      SSD_HDCP_RESTRICTED = 8
    };

    // Return supported URN if type matches to capabilities, otherwise null
    virtual const char *Supported(const char* licenseType, const char *licenseKey) = 0;
    virtual AP4_CencSingleSampleDecrypter *CreateSingleSampleDecrypter(AP4_DataBuffer &serverCertificate) = 0;
    virtual size_t CreateSession(AP4_DataBuffer &streamCodec) = 0;
    virtual void CloseSession(size_t sessionHandle) = 0;

    virtual uint32_t GetCapabilities(size_t sessionHandle, bool has_hdcp_testkey) = 0;
    virtual const char *GetSessionId(size_t sessionHandle) = 0;

    virtual bool OpenVideoDecoder(const SSD_VIDEOINITDATA *initData) = 0;
    virtual SSD_DECODE_RETVAL DecodeVideo(void* instance, SSD_SAMPLE *sample, SSD_PICTURE *picture) = 0;
    virtual void ResetVideo() = 0;
  };
}; // namespace
