//
//  jsoncomm.hpp
//  p44bridged
//
//  Created by Lukas Zeller on 22.05.13.
//  Copyright (c) 2013 plan44.ch. All rights reserved.
//

#ifndef __p44bridged__jsoncomm__
#define __p44bridged__jsoncomm__

#include "socketcomm.hpp"

#include <json/json.h>

using namespace std;

namespace p44 {

  // Errors
  typedef enum json_tokener_error JsonCommErrors;

  class JsonCommError : public Error
  {
  public:
    static const char *domain() { return "JsonComm"; }
    virtual const char *getErrorDomain() const { return JsonCommError::domain(); };
    JsonCommError(JsonCommErrors aError) : Error(ErrorCode(aError)) {};
    JsonCommError(JsonCommErrors aError, std::string aErrorMessage) : Error(ErrorCode(aError), aErrorMessage) {};
  };


  class JsonComm;
  class JsonObject;

  /// shared pointer for JSON object
  typedef boost::shared_ptr<JsonObject> JsonObjectPtr;

  class JsonObject
  {
    friend class JsonComm;

    struct json_object *json_obj;

    /// private constructor from existing JSON object
    JsonObject(struct json_object *obj);
  public:
    JsonObject();
    virtual ~JsonObject();

    struct json_object *obj() { return json_obj; };
    const char *c_str();
  };



  /// generic callback for signalling completion (with success/error reporting)
  typedef boost::function<void (JsonComm *aJsonCommP, ErrorPtr aError, JsonObjectPtr aJsonObject)> JSonMessageCB;

  typedef boost::shared_ptr<JsonComm> JsonCommPtr;
  /// A class providing low level access to the DALI bus
  class JsonComm : public SocketComm
  {
    typedef SocketComm inherited;

    JSonMessageCB jsonMessageHandler;

    // JSON parsing
    struct json_tokener* tokener;
    bool ignoreUntilNextEOM;

    // JSON sending
    string transmitBuffer;

  public:

    JsonComm(SyncIOMainLoop *aMainLoopP);
    virtual ~JsonComm();

    /// install callback for data becoming ready to read
    /// @param aCallBack will be called when data is ready for reading (receiveBytes()) or an asynchronous error occurs on the connection
    void setMessageHandler(JSonMessageCB aJsonMessageHandler);

    /// send a JSON message
    void sendMessage(JsonObjectPtr aJsonObject, ErrorPtr &aError);

  private:
    void gotData(ErrorPtr aError);
    void canSendData(ErrorPtr aError);
    
  };
  
} // namespace p44


#endif /* defined(__p44bridged__jsoncomm__) */