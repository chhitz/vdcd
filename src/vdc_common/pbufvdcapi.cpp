//
//  pbufvdcapi.cpp
//  vdcd
//
//  Created by Lukas Zeller on 05.12.13.
//  Copyright (c) 2013 plan44.ch. All rights reserved.
//

#include "pbufvdcapi.hpp"


using namespace p44;



#pragma mark - PbufApiValue

PbufApiValue::PbufApiValue() :
  allocatedType(apivalue_null)
{
}


PbufApiValue::~PbufApiValue()
{
  clear();
}


ApiValuePtr PbufApiValue::newValue(ApiValueType aObjectType)
{
  ApiValuePtr newVal = ApiValuePtr(new PbufApiValue);
  newVal->setType(aObjectType);
  return newVal;
}



void PbufApiValue::clear()
{
  // forget allocated type
  if (allocatedType!=apivalue_null) {
    memset(&objectValue, 0, sizeof(objectValue));
    switch (allocatedType) {
      case apivalue_string:
        if (objectValue.stringP) delete objectValue.stringP;
        objectValue.stringP = NULL;
        break;
      case apivalue_object:
        if (objectValue.objectMapP) delete objectValue.objectMapP;
        objectValue.objectMapP = NULL;
        break;
      case apivalue_array:
        if (objectValue.arrayVectorP) delete objectValue.arrayVectorP;
        objectValue.arrayVectorP = NULL;
        break;
      default:
        break;
    }
    allocatedType = apivalue_null;
  }
}


void PbufApiValue::allocate()
{
  if (allocatedType!=getType() && allocatedType==apivalue_null) {
    allocatedType = getType();
    switch (allocatedType) {
      case apivalue_string:
        objectValue.stringP = new string;
        break;
      case apivalue_object:
        objectValue.objectMapP = new ApiValueFieldMap;
        break;
      case apivalue_array:
        objectValue.arrayVectorP = new ApiValueArray;
        break;
      default:
        break;
    }
  }
}


bool PbufApiValue::allocateIf(ApiValueType aIsType)
{
  if (getType()==aIsType) {
    allocate();
    return true;
  }
  return false;
}



void PbufApiValue::add(const string &aKey, ApiValuePtr aObj)
{
  PbufApiValuePtr val = boost::dynamic_pointer_cast<PbufApiValue>(aObj);
  if (val && allocateIf(apivalue_object)) {
    (*(objectValue.objectMapP))[lowerCase(aKey)] = val;
  }
}


ApiValuePtr PbufApiValue::get(const string &aKey)
{
  if (allocatedType==apivalue_object) {
    ApiValueFieldMap::iterator pos = objectValue.objectMapP->find(lowerCase(aKey));
    if (pos!=objectValue.objectMapP->end())
      return pos->second;
  }
  return ApiValuePtr();
}


void PbufApiValue::del(const string &aKey)
{
  if (allocatedType==apivalue_object) {
    objectValue.objectMapP->erase(aKey);
  }
}


int PbufApiValue::arrayLength()
{
  if (allocatedType==apivalue_array) {
    return (int)objectValue.arrayVectorP->size();
  }
  return 0;
}


void PbufApiValue::arrayAppend(ApiValuePtr aObj)
{
  PbufApiValuePtr val = boost::dynamic_pointer_cast<PbufApiValue>(aObj);
  if (val && allocateIf(apivalue_array)) {
    objectValue.arrayVectorP->push_back(val);
  }
}


ApiValuePtr PbufApiValue::arrayGet(int aAtIndex)
{
  if (allocatedType==apivalue_array) {
    if (aAtIndex<objectValue.arrayVectorP->size()) {
      return objectValue.arrayVectorP->at(aAtIndex);
    }
  }
  return ApiValuePtr();
}


void PbufApiValue::arrayPut(int aAtIndex, ApiValuePtr aObj)
{
  PbufApiValuePtr val = boost::dynamic_pointer_cast<PbufApiValue>(aObj);
  if (val && allocateIf(apivalue_array)) {
    if (aAtIndex<objectValue.arrayVectorP->size()) {
      ApiValueArray::iterator i = objectValue.arrayVectorP->begin() + aAtIndex;
      objectValue.arrayVectorP->erase(i);
      objectValue.arrayVectorP->insert(i, val);
    }
  }
}



bool PbufApiValue::resetKeyIteration()
{
  if (allocatedType==apivalue_object) {
    keyIterator = objectValue.objectMapP->begin();
  }
  return false; // cannot be iterated
}


bool PbufApiValue::nextKeyValue(string &aKey, ApiValuePtr &aValue)
{
  if (allocatedType==apivalue_object) {
    if (keyIterator!=objectValue.objectMapP->end()) {
      aKey = keyIterator->first;
      aValue = keyIterator->second;
      keyIterator++;
      return true;
    }
  }
  return false;
}



uint64_t PbufApiValue::uint64Value()
{
  if (allocatedType==apivalue_uint64) {
    return objectValue.uint64Val;
  }
  return 0;
}



int64_t PbufApiValue::int64Value()
{
  if (allocatedType==apivalue_int64) {
    return objectValue.int64Val;
  }
  return 0;
}


double PbufApiValue::doubleValue()
{
  if (allocatedType==apivalue_double) {
    return objectValue.doubleVal;
  }
  return 0.0;
}


bool PbufApiValue::boolValue()
{
  if (allocatedType==apivalue_bool) {
    return objectValue.boolVal;
  }
  return false;
}


string PbufApiValue::stringValue()
{
  if (allocatedType==apivalue_string) {
    return *(objectValue.stringP);
  }
  // let base class render the contents as string
  return inherited::stringValue();
}


const char *PbufApiValue::charPointerValue()
{
  if (allocatedType==apivalue_string) {
    return objectValue.stringP->c_str();
  }
  return NULL;
}


void PbufApiValue::setUint64Value(uint64_t aUint64)
{
  if (allocateIf(apivalue_uint64)) {
    objectValue.uint64Val = aUint64;
  }
}


void PbufApiValue::setInt64Value(int64_t aInt64)
{
  if (allocateIf(apivalue_int64)) {
    objectValue.int64Val = aInt64;
  }
}


void PbufApiValue::setDoubleValue(double aDouble)
{
  if (allocateIf(apivalue_double)) {
    objectValue.doubleVal = aDouble;
  }
}


void PbufApiValue::setBoolValue(bool aBool)
{
  if (allocateIf(apivalue_bool)) {
    objectValue.boolVal = aBool;
  }
}


bool PbufApiValue::setStringValue(const string &aString)
{
  if (allocateIf(apivalue_string)) {
    objectValue.stringP->assign(aString);
    return true;
  }
  else {
    // let base class try to convert to type of object
    return inherited::setStringValue(aString);
  }
}


void PbufApiValue::setNull()
{
  clear();
}



void PbufApiValue::getValueFromMessageField(const ProtobufCFieldDescriptor &aFieldDescriptor, const ProtobufCMessage &aMessage)
{
  const uint8_t *baseP = (const uint8_t *)(&aMessage);
  const uint8_t *fieldBaseP = baseP+aFieldDescriptor.offset;
  // check quantifier
  if (aFieldDescriptor.label==PROTOBUF_C_LABEL_REPEATED) {
    // repeated field, pack into array
    size_t arraySize = *((size_t *)(baseP+aFieldDescriptor.quantifier_offset));
    setType(apivalue_array);
    for (int i = 0; i<arraySize; i++) {
      PbufApiValuePtr element = PbufApiValuePtr(new PbufApiValue);
      element->setValueFromField(aFieldDescriptor.type, fieldBaseP, i, arraySize);
      arrayAppend(element);
    }
  }
  else {
    // not repeated (array), but single field
    bool hasField = false;
    if (aFieldDescriptor.label==PROTOBUF_C_LABEL_OPTIONAL) {
      if (aFieldDescriptor.quantifier_offset) {
        // scalar that needs quantifier
        hasField = *((protobuf_c_boolean *)(baseP+aFieldDescriptor.quantifier_offset));
      }
      else {
        // value is a pointer, exists if not NULL
        hasField = *((const void **)(baseP+aFieldDescriptor.offset))!=NULL;
      }
    }
    else {
      // must be mandatory
      hasField = true;
    }
    // get value, if available
    if (!hasField) {
      // optional field is not present
      setNull();
    }
    else {
      // get value
      setValueFromField(aFieldDescriptor.type, fieldBaseP, 0, -1); // not array
    }
  }
}



void PbufApiValue::putValueIntoMessageField(const ProtobufCFieldDescriptor &aFieldDescriptor, const ProtobufCMessage &aMessage)
{
  uint8_t *baseP = (uint8_t *)(&aMessage);
  uint8_t *fieldBaseP = baseP+aFieldDescriptor.offset;
  // check quantifier
  if (aFieldDescriptor.label==PROTOBUF_C_LABEL_REPEATED) {
    // repeated field. If this value is an array, assign elements
    if (getType()==apivalue_array) {
      // set size
      *((size_t *)(baseP+aFieldDescriptor.quantifier_offset)) = arrayLength();
      // iterate over existing elements
      for (int i = 0; i<arrayLength(); i++) {
        PbufApiValuePtr element = boost::dynamic_pointer_cast<PbufApiValue>(arrayGet(i));
        element->putValueIntoField(aFieldDescriptor.type, fieldBaseP, i, ApiValue::arrayLength());
      }
    }
  }
  else {
    // not repeated (array), but single field
    bool hasField;
    if (aFieldDescriptor.label==PROTOBUF_C_LABEL_OPTIONAL) {
      hasField = !isNull();
      if (aFieldDescriptor.quantifier_offset) {
        // scalar that needs quantifier
        *((protobuf_c_boolean *)(baseP+aFieldDescriptor.quantifier_offset)) = hasField;
      }
    }
    else {
      // must be mandatory
      hasField = true;
    }
    // put value, if available
    if (hasField) {
      putValueIntoField(aFieldDescriptor.type, fieldBaseP, 0, -1); // not array
    }
  }
}




void PbufApiValue::getObjectFromMessageFields(const ProtobufCMessage &aMessage)
{
  // must be an object
  setType(apivalue_object);
  // iterate over fields
  const ProtobufCFieldDescriptor *fieldDescP = aMessage.descriptor->fields;
  for (unsigned f = 0; f<aMessage.descriptor->n_fields; f++) {
    PbufApiValuePtr field = PbufApiValuePtr(new PbufApiValue);
    field->getValueFromMessageField(*fieldDescP, aMessage);
    if (field->getType()!=apivalue_null) {
      // do not add NULL fields
      add(fieldDescP->name, field);
    }
    fieldDescP++; // next field descriptor
  }
}



void PbufApiValue::putObjectIntoMessageFields(ProtobufCMessage &aMessage)
{
  // only if we actually have any object data
  if (allocatedType==apivalue_object) {
    // iterate over fields
    const ProtobufCFieldDescriptor *fieldDescP = aMessage.descriptor->fields;
    for (unsigned f = 0; f<aMessage.descriptor->n_fields; f++) {
      // see if value object has a key for this field
      PbufApiValuePtr val = boost::dynamic_pointer_cast<PbufApiValue>(get(fieldDescP->name));
      if (val) {
        val->putValueIntoMessageField(*fieldDescP, aMessage);
      }
      fieldDescP++; // next field descriptor
    }
  }
}



void PbufApiValue::setValueFromField(ProtobufCType aProtobufType, const void *aData, size_t aIndex, ssize_t aArraySize)
{
  if (aArraySize>=0) {
    // is an array, dereference the data pointer once to get to elements
    aData = *((void **)aData);
  }
  switch (aProtobufType) {
    case PROTOBUF_C_TYPE_BOOL:
      setType(apivalue_bool);
      setBoolValue(*((protobuf_c_boolean *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_INT32:
    case PROTOBUF_C_TYPE_SINT32:
    case PROTOBUF_C_TYPE_SFIXED32:
      setType(apivalue_int64);
      setInt32Value(*((int32_t *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_INT64:
    case PROTOBUF_C_TYPE_SINT64:
    case PROTOBUF_C_TYPE_SFIXED64:
      setType(apivalue_int64);
      setInt64Value(*((int64_t *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_UINT32:
    case PROTOBUF_C_TYPE_FIXED32:
      setType(apivalue_uint64);
      setUint32Value(*((uint32_t *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_UINT64:
    case PROTOBUF_C_TYPE_FIXED64:
      setType(apivalue_uint64);
      setUint64Value(*((uint64_t *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_FLOAT:
      setType(apivalue_double);
      setDoubleValue(*((float *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_DOUBLE:
      setType(apivalue_double);
      setDoubleValue(*((double *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_ENUM:
      setType(apivalue_int64);
      setInt32Value(*((int *)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_STRING:
      setType(apivalue_string);
      setStringValue(*((const char **)aData+aIndex));
      break;
    case PROTOBUF_C_TYPE_BYTES:
      // TODO: implement it
      break;
    case PROTOBUF_C_TYPE_MESSAGE: {
      // submessage, pack into object value
      setType(apivalue_object);
      const ProtobufCMessage *subMessageP = *((const ProtobufCMessage **)aData+aIndex);
      getObjectFromMessageFields(*subMessageP);
      break;
    }
    default:
      setType(apivalue_null);
      break;
  }
}


void PbufApiValue::putValueIntoField(ProtobufCType aProtobufType, void *aData, size_t aIndex, ssize_t aArraySize)
{
  // check array case
  bool allocArray = false;
  void *dataP = aData; // default to in-place data (no array)
  if (aArraySize>=0) {
    // check if array is allocated already
    if (*((void **)aData)==NULL) {
      // array does not yet exist
      allocArray = true;
      dataP = NULL;
    }
    else {
      // array does exist, data pointer can be used as-is
      dataP = *((void **)aData); // dereference once
    }
  }
  switch (aProtobufType) {
    case PROTOBUF_C_TYPE_BOOL:
      if (allocatedType==apivalue_bool) {
        if (allocArray) dataP = new protobuf_c_boolean[aArraySize];
        *((protobuf_c_boolean *)dataP+aIndex) = boolValue();
      }
      break;
    case PROTOBUF_C_TYPE_INT32:
    case PROTOBUF_C_TYPE_SINT32:
    case PROTOBUF_C_TYPE_SFIXED32:
      if (allocatedType==apivalue_int64) {
        if (allocArray) dataP = new int32_t[aArraySize];
        *((int32_t *)dataP+aIndex) = int32Value();
      }
      break;
    case PROTOBUF_C_TYPE_INT64:
    case PROTOBUF_C_TYPE_SINT64:
    case PROTOBUF_C_TYPE_SFIXED64:
      if (allocatedType==apivalue_int64) {
        if (allocArray) dataP = new int64_t[aArraySize];
        *((int64_t *)dataP+aIndex) = int64Value();
      }
      break;
    case PROTOBUF_C_TYPE_UINT32:
    case PROTOBUF_C_TYPE_FIXED32:
      if (allocatedType==apivalue_uint64) {
        if (allocArray) dataP = new uint32_t[aArraySize];
        *((uint32_t *)dataP+aIndex) = uint32Value();
      }
      break;
    case PROTOBUF_C_TYPE_UINT64:
    case PROTOBUF_C_TYPE_FIXED64:
      if (allocatedType==apivalue_uint64) {
        if (allocArray) dataP = new uint64_t[aArraySize];
        *((uint64_t *)dataP+aIndex) = uint64Value();
      }
      break;
    case PROTOBUF_C_TYPE_FLOAT:
      if (allocatedType==apivalue_double) {
        if (allocArray) dataP = new float[aArraySize];
        *((float *)dataP+aIndex) = doubleValue();
      }
      break;
    case PROTOBUF_C_TYPE_DOUBLE:
      if (allocatedType==apivalue_double) {
        if (allocArray) dataP = new double[aArraySize];
        *((double *)dataP+aIndex) = doubleValue();
      }
      break;
    case PROTOBUF_C_TYPE_ENUM:
      if (allocatedType==apivalue_uint64) {
        if (allocArray) dataP = new int[aArraySize];
        *((int *)dataP+aIndex) = int32Value();
      }
      break;
    case PROTOBUF_C_TYPE_STRING:
      if (allocatedType==apivalue_string) {
        if (allocArray) dataP = new char*[aArraySize];
        *((const char **)dataP+aIndex) = charPointerValue();
      }
      break;
    case PROTOBUF_C_TYPE_BYTES:
      // TODO: implement it
      break;
    case PROTOBUF_C_TYPE_MESSAGE: {
      // submessage, pack into object value
      #warning "// TODO: implement generic submessages
      break;
    }
    default:
      // do nothing
      break;
  }
  if (allocArray) {
    // array was allocated new, link into message
    *((void **)aData) = dataP;
  }
}



//void PbufApiValue::getValueFromFieldNamed(const string &aFieldName, const ProtobufCMessage &aMessage)
//{
//  const ProtobufCFieldDescriptor *fieldDescP = protobuf_c_message_descriptor_get_field_by_name(aMessage.descriptor, aFieldName.c_str());
//  if (fieldDescP) {
//    getValueFromMessageField(*fieldDescP, aMessage);
//  }
//}



#pragma mark - VdcPbufApiServer


VdcApiConnectionPtr VdcPbufApiServer::newConnection()
{
  // create the right kind of API connection
  return VdcApiConnectionPtr(static_cast<VdcApiConnection *>(new VdcPbufApiConnection()));
}



#pragma mark - VdcPbufApiRequest


VdcPbufApiRequest::VdcPbufApiRequest(VdcPbufApiConnectionPtr aConnection, uint32_t aRequestId)
{
  pbufConnection = aConnection;
  reqId = aRequestId;
  responseType = VDCAPI__TYPE__GENERIC_RESPONSE;
}


VdcApiConnectionPtr VdcPbufApiRequest::connection()
{
  return pbufConnection;
}



ErrorPtr VdcPbufApiRequest::sendResult(ApiValuePtr aResult)
{
  ErrorPtr err;
  if (!aResult || aResult->isNull()) {
    // empty result is like sending no error
    err = sendError(0);
    LOG(LOG_INFO,"vdSM <- vDC (pbuf) result sent: requestid='%d', result=NULL\n", reqId);
  }
  else {
    // we might have a specific result
    PbufApiValuePtr result = boost::dynamic_pointer_cast<PbufApiValue>(aResult);
    ProtobufCMessage *subMessageP = NULL;
    // create a message
    Vdcapi__Message msg = VDCAPI__MESSAGE__INIT;
    msg.has_message_id = true; // is response to a previous method call message
    msg.message_id = reqId; // use same message id as in method call
    // set correct type and generate appropriate submessage
    msg.type = responseType;
    switch (responseType) {
      case VDCAPI__TYPE__VDC_RESPONSE_HELLO:
        msg.vdc_response_hello = new Vdcapi__VdcResponseHello;
        vdcapi__vdc__response_hello__init(msg.vdc_response_hello);
        subMessageP = &(msg.vdc_response_hello->base);
        break;
      case VDCAPI__TYPE__VDC_RESPONSE_GET_PROPERTY:
        msg.vdc_response_get_property = new Vdcapi__VdcResponseGetProperty;
        vdcapi__vdc__response_get_property__init(msg.vdc_response_get_property);
        subMessageP = &(msg.vdc_response_get_property->base);
        break;
      default:
        LOG(LOG_INFO,"vdSM <- vDC (pbuf) response '%s' cannot be sent because no message is implemented for it at the pbuf level\n", aResult->description().c_str());
        return ErrorPtr(new VdcApiError(500,"Error: Method is not implemented in the pbuf API"));
    }
    // now fill parameters into submessage
    if (result) {
      result->putObjectIntoMessageFields(*subMessageP);
    }
    // send
    err = pbufConnection->sendMessage(&msg);
    // dispose allocated submessage
    protobuf_c_message_free_unpacked(subMessageP, NULL);
    // log
    LOG(LOG_INFO,"vdSM <- vDC (pbuf) result sent: requestid='%d', result=%s\n", reqId, aResult ? aResult->description().c_str() : "<none>");
  }
  return err;
}


ErrorPtr VdcPbufApiRequest::sendError(uint32_t aErrorCode, string aErrorMessage, ApiValuePtr aErrorData)
{
  ErrorPtr err;
  // create a message
  Vdcapi__Message msg = VDCAPI__MESSAGE__INIT;
  Vdcapi__GenericResponse resp = VDCAPI__GENERIC_RESPONSE__INIT;
  // error response is a generic message
  #warning "// TODO: one of the message_id's is redundant, I'd recommend to get rid of the id in the generic response as there is on at the message level anyway"
  msg.generic_response = &resp;
  msg.has_message_id = true; // is response to a previous method call message
  msg.message_id = reqId; // use same message id as in method call
  resp.message_id = reqId;
  resp.result = VdcPbufApiConnection::internalToPbufError(aErrorCode);
  resp.description = (char *)(aErrorMessage.size()>0 ? aErrorMessage.c_str() : NULL);
  err = pbufConnection->sendMessage(&msg);
  // log (if not just OK)
  if (aErrorCode!=ErrorOK)
    LOG(LOG_INFO,"vdSM <- vDC (pbuf) error sent: requestid='%d', error=%d (%s)\n", reqId, aErrorCode, aErrorMessage.c_str());
  // done
  return err;
}



#pragma mark - VdcPbufApiConnection


VdcPbufApiConnection::VdcPbufApiConnection() :
  closeWhenSent(false),
  expectedMsgBytes(0),
  requestIdCounter(0)
{
  socketComm = SocketCommPtr(new SocketComm(SyncIOMainLoop::currentMainLoop()));
  // install data handler
  socketComm->setReceiveHandler(boost::bind(&VdcPbufApiConnection::gotData, this, _2));
}



void VdcPbufApiConnection::gotData(ErrorPtr aError)
{
  // got data
  if (Error::isOK(aError)) {
    // no error
    size_t dataSz = socketComm->numBytesReady();
    // read data we've got so far
    if (dataSz>0) {
      // temporary buffer
      uint8_t *buf = new uint8_t[dataSz];
      size_t receivedBytes = socketComm->receiveBytes(dataSz, buf, aError);
      if (Error::isOK(aError)) {
        // append to receive buffer
        receivedMessage.append((const char *)buf, receivedBytes);
        // single message extraction
        while(true) {
          if(expectedMsgBytes==0 && receivedMessage.size()>=4) {
            // got header, decode it
            const uint8_t *sz = (const uint8_t *)receivedMessage.c_str();
            expectedMsgBytes =
              (sz[0]<<24) +
              (sz[1]<<16) +
              (sz[2]<<8) +
              sz[3];
            receivedMessage.erase(0,4);
          }
          // check for complete message
          if (expectedMsgBytes && (receivedMessage.size()>=expectedMsgBytes)) {
            // process message
            aError = processMessage(receivedMessage);
            // erase processed message
            receivedMessage.erase(0,expectedMsgBytes);
            expectedMsgBytes = 0; // reset to unknown
            // repeat evaluation with remaining bytes (could be another message)
          }
          else {
            // no complete message yet, done for now
            break;
          }
        }
      }
      // forget buffer
      delete buf; buf = NULL;
    } // some data seems to be ready
  } // no connection error
  if (!Error::isOK(aError)) {
    // error occurred
    // pbuf API cannot resynchronize, close connection
    LOG(LOG_WARNING,"Error occurred on protobuf connection - cannot be re-synced, closing: %s\n", aError->description().c_str());
    closeConnection();
  }
}


ErrorPtr VdcPbufApiConnection::sendMessage(const Vdcapi__Message *aVdcApiMessage)
{
  ErrorPtr err;
  // generate the binary message
  size_t packedSize = vdcapi__message__get_packed_size(aVdcApiMessage);
  uint8_t *packedMsg = new uint8_t[packedSize+4]; // leave room for header
  // - add the header
  packedMsg[0] = (packedSize>>24) & 0xFF;
  packedMsg[1] = (packedSize>>16) & 0xFF;
  packedMsg[2] = (packedSize>>8) & 0xFF;
  packedMsg[3] = packedSize & 0xFF;
  // - add the message data
  vdcapi__message__pack(aVdcApiMessage, packedMsg+4);
  // - adjust the total message length
  packedSize += 4;
  // send the message
  if (transmitBuffer.size()>0) {
    // other messages are already waiting, append entire message
    transmitBuffer.append((const char *)packedMsg, packedSize);
  }
  else {
    // nothing in buffer yet, start new send
    size_t sentBytes = socketComm->transmitBytes(packedSize, packedMsg, err);
    if (Error::isOK(err)) {
      // check if all could be sent
      if (sentBytes<packedSize) {
        // Not everything (or maybe nothing, transmitBytes() can return 0) was sent
        // - enable callback for ready-for-send
        socketComm->setTransmitHandler(boost::bind(&VdcPbufApiConnection::canSendData, this, _2));
        // buffer the rest, canSendData handler will take care of writing it out
        transmitBuffer.assign((char *)packedMsg+sentBytes, packedSize-sentBytes);
      }
			else {
				// all sent
				// - disable transmit handler
        socketComm->setTransmitHandler(NULL);
			}
    }
  }
  // return the buffer
  delete packedMsg;
  // done
  return err;
}


void VdcPbufApiConnection::canSendData(ErrorPtr aError)
{
  size_t bytesToSend = transmitBuffer.size();
  if (bytesToSend>0 && Error::isOK(aError)) {
    // send data from transmit buffer
    size_t sentBytes = socketComm->transmitBytes(bytesToSend, (const uint8_t *)transmitBuffer.c_str(), aError);
    if (Error::isOK(aError)) {
      if (sentBytes==bytesToSend) {
        // all sent
        transmitBuffer.erase();
				// - disable transmit handler
        socketComm->setTransmitHandler(NULL);
      }
      else {
        // partially sent, remove sent bytes
        transmitBuffer.erase(0, sentBytes);
      }
      // check for closing connection when no data pending to be sent any more
      if (closeWhenSent && transmitBuffer.size()==0) {
        closeWhenSent = false; // done
        closeConnection();
      }
    }
  }
}





ErrorCode VdcPbufApiConnection::pbufToInternalError(Vdcapi__ResultCode aVdcApiResultCode)
{
  ErrorCode errorCode;
  switch (aVdcApiResultCode) {
    case VDCAPI__RESULT_CODE__RESULT_OK: errorCode = 0; break;
    case VDCAPI__RESULT_CODE__RESULT_MESSAGE_UNKNOWN: errorCode = 405; break;
    case VDCAPI__RESULT_CODE__RESULT_NOT_FOUND: errorCode = 404; break;
    case VDCAPI__RESULT_CODE__RESULT_INCOMPATIBLE_API: errorCode = 505; break;
    case VDCAPI__RESULT_CODE__RESULT_SERVICE_NOT_AVAILABLE: errorCode = 503; break;
    case VDCAPI__RESULT_CODE__RESULT_INSUFFICIENT_STORAGE: errorCode = 507; break;
    case VDCAPI__RESULT_CODE__RESULT_FORBIDDEN: errorCode = 403; break;
    case VDCAPI__RESULT_CODE__RESULT_NOT_AUTHORIZED: errorCode = 401; break;
    case VDCAPI__RESULT_CODE__RESULT_NOT_IMPLEMENTED: errorCode = 501; break;
    case VDCAPI__RESULT_CODE__RESULT_NO_CONTENT_FOR_ARRAY: errorCode = 204; break;
    case VDCAPI__RESULT_CODE__RESULT_INVALID_VALUE_TYPE: errorCode = 415; break;
    // TODO: what were these intended for?
    case VDCAPI__RESULT_CODE__RESULT_MISSING_SUBMESSAGE: errorCode = 400; break;
    case VDCAPI__RESULT_CODE__RESULT_MISSING_DATA: errorCode = 400; break;
    default: errorCode = 500; break; // general error
  }
  return errorCode;
}


Vdcapi__ResultCode VdcPbufApiConnection::internalToPbufError(ErrorCode aErrorCode)
{
  Vdcapi__ResultCode res;
  switch (aErrorCode) {
    case 0:
    case 200: res = VDCAPI__RESULT_CODE__RESULT_OK; break;
    case 405: res = VDCAPI__RESULT_CODE__RESULT_MESSAGE_UNKNOWN; break;
    case 404: res = VDCAPI__RESULT_CODE__RESULT_NOT_FOUND; break;
    case 505: res = VDCAPI__RESULT_CODE__RESULT_INCOMPATIBLE_API; break;
    case 503: res = VDCAPI__RESULT_CODE__RESULT_SERVICE_NOT_AVAILABLE; break;
    case 507: res = VDCAPI__RESULT_CODE__RESULT_INSUFFICIENT_STORAGE; break;
    case 403: res = VDCAPI__RESULT_CODE__RESULT_FORBIDDEN; break;
    case 401: res = VDCAPI__RESULT_CODE__RESULT_NOT_AUTHORIZED; break;
    case 501: res = VDCAPI__RESULT_CODE__RESULT_NOT_IMPLEMENTED; break;
    case 204: res = VDCAPI__RESULT_CODE__RESULT_NO_CONTENT_FOR_ARRAY; break;
    case 415: res = VDCAPI__RESULT_CODE__RESULT_INVALID_VALUE_TYPE; break;
    default: res = VDCAPI__RESULT_CODE__RESULT_GENERAL_ERROR; break; // general error
  }
  return res;
}






ErrorPtr VdcPbufApiConnection::processMessage(const string &aPackedMessage)
{
  Vdcapi__Message *decodedMsg;
  ProtobufCMessage *paramsMsg = NULL;
  ErrorPtr err;

  decodedMsg = vdcapi__message__unpack(NULL, aPackedMessage.size(), (const uint8_t *)aPackedMessage.c_str()); // Deserialize the serialized input
  if (decodedMsg == NULL) {
    err = ErrorPtr(new VdcApiError(400,"error unpacking incoming message"));
  }
  else {
    // successful message decoding
    string method;
    int responseType = 0; // none
    int32_t responseForId = -1; // none
    bool emptyResult = false;
    switch (decodedMsg->type) {
      // incoming method calls
      case VDCAPI__TYPE__VDSM_REQUEST_HELLO:
        method = "hello";
        paramsMsg = &(decodedMsg->vdsm_request_hello->base);
        responseType = VDCAPI__TYPE__VDC_RESPONSE_HELLO;
        break;
      case VDCAPI__TYPE__VDSM_REQUEST_GET_PROPERTY:
        method = "getProperty";
        paramsMsg = &(decodedMsg->vdsm_request_get_property->base);
        responseType = VDCAPI__TYPE__VDC_RESPONSE_GET_PROPERTY;
        break;
      case VDCAPI__TYPE__VDSM_SEND_SET_PROPERTY:
        method = "setProperty";
        paramsMsg = &(decodedMsg->vdsm_send_set_property->base);
        responseType = VDCAPI__TYPE__GENERIC_RESPONSE;
        break;
      case VDCAPI__TYPE__VDSM_SEND_REMOVE:
        method = "remove";
        paramsMsg = &(decodedMsg->vdsm_send_remove->base);
        responseType = VDCAPI__TYPE__GENERIC_RESPONSE;
        break;
      case VDCAPI__TYPE__VDSM_SEND_BYE:
        method = "bye";
        paramsMsg = NULL; // no params (altough there is a message with no fields)
        responseType = VDCAPI__TYPE__GENERIC_RESPONSE;
        break;
      // Notifications
      case VDCAPI__TYPE__VDSM_NOTIFICATION_PING:
        method = "ping";
        paramsMsg = &(decodedMsg->vdsm_send_ping->base);
        break;
      case VDCAPI__TYPE__VDSM_NOTIFICATION_CALL_SCENE:
        method = "callScene";
        paramsMsg = &(decodedMsg->vdsm_send_call_scene->base);
        break;
      case VDCAPI__TYPE__VDSM_NOTIFICATION_SAVE_SCENE:
        method = "saveScene";
        paramsMsg = &(decodedMsg->vdsm_send_save_scene->base);
        break;
      case VDCAPI__TYPE__VDSM_NOTIFICATION_UNDO_SCENE:
        method = "undoScene";
        paramsMsg = &(decodedMsg->vdsm_send_undo_scene->base);
        break;
      case VDCAPI__TYPE__VDSM_NOTIFICATION_SET_LOCAL_PRIO:
        method = "setLocalPriority";
        paramsMsg = &(decodedMsg->vdsm_send_set_local_prio->base);
        break;
      case VDCAPI__TYPE__VDSM_NOTIFICATION_SET_CONTROL_VALUE:
        method = "setControlValue";
        paramsMsg = &(decodedMsg->vdsm_send_set_control_value->base);
        break;
      case VDCAPI__TYPE__VDSM_NOTIFICATION_CALL_MIN_SCENE:
        method = "callSceneMin";
        paramsMsg = &(decodedMsg->vdsm_send_call_min_scene->base);
        break;
      case VDCAPI__TYPE__VDSM_NOTIFICATION_IDENTIFY:
        method = "identify";
        paramsMsg = &(decodedMsg->vdsm_send_identify->base);
        break;
      // incoming responses
      case VDCAPI__TYPE__GENERIC_RESPONSE: {
        // error or NULL response
        _Vdcapi__GenericResponse *genericResponse = (_Vdcapi__GenericResponse *)decodedMsg;
        if (genericResponse->result!=VDCAPI__RESULT_CODE__RESULT_OK) {
          // convert to HTTP-style internal codes
          ErrorCode errorCode = pbufToInternalError(genericResponse->result);
          err = ErrorPtr(new VdcApiError(errorCode, nonNullCStr(genericResponse->description)));
        }
        responseForId = genericResponse->message_id;
        emptyResult = true; // do not convert message fields to result
        break;
      }
      // unknown message type
      default:
        method = string_format("unknownMethod_%d", decodedMsg->type);
        responseType = VDCAPI__TYPE__GENERIC_RESPONSE;
        break;
    }
    // dispatch between incoming method calls/notifications and responses for outgoing calls
    if (responseForId>=0) {
      // This is a response
      PbufApiValuePtr result = PbufApiValuePtr(new PbufApiValue);
      if (Error::isOK(err) && !emptyResult) {
        // convert fields of response to API object
        result->getObjectFromMessageFields(decodedMsg->base);
      }
      // find the originating request
      PendingAnswerMap::iterator pos = pendingAnswers.find(responseForId);
      if (pos==pendingAnswers.end()) {
        // errors without ID cannot be associated with calls made earlier, so just log the error
        LOG(LOG_WARNING,"vdSM -> vDC (pbuf) error: Received response with unknown 'id'=%d, error=%s\n", responseForId, Error::isOK(err) ? "<none>" : err->description().c_str());
      }
      else {
        // found callback
        VdcApiResponseCB cb = pos->second;
        pendingAnswers.erase(pos); // erase
        // create request object just to hold the response ID
        VdcPbufApiRequestPtr request = VdcPbufApiRequestPtr(new VdcPbufApiRequest(VdcPbufApiConnectionPtr(this), responseForId));
        if (Error::isOK(err)) {
          LOG(LOG_INFO,"vdSM -> vDC (pbuf) result received: id='%s', result=%s\n", request->requestId().c_str(), result ? result->description().c_str() : "<none>");
        }
        else {
          LOG(LOG_INFO,"vdSM -> vDC (pbuf) error received: id='%s', error=%s, errordata=%s\n", request->requestId().c_str(), err->description().c_str(), result ? result->description().c_str() : "<none>");
        }
        cb(this, request, err, result); // call handler
      }
      err.reset(); // delivered
    }
    else {
      // this is a method call or notification
      VdcPbufApiRequestPtr request;
      // - get the params
      PbufApiValuePtr params = PbufApiValuePtr(new PbufApiValue);
      params->getObjectFromMessageFields(*paramsMsg);
      // - dispatch between methods and notifications
      if (decodedMsg->has_message_id) {
        // method call, we need a request reference object
        request = VdcPbufApiRequestPtr(new VdcPbufApiRequest(VdcPbufApiConnectionPtr(this), decodedMsg->message_id));
        request->responseType = (Vdcapi__Type)responseType; // save the response type for sending answers later
        LOG(LOG_INFO,"vdSM -> vDC (pbuf) method call received: requestid='%d', method='%s', params=%s\n", request->reqId, method.c_str(), params ? params->description().c_str() : "<none>");
      }
      else {
        LOG(LOG_INFO,"vdSM -> vDC (pbuf) notification received: method='%s', params=%s\n", method.c_str(), params ? params->description().c_str() : "<none>");
      }
      // call handler
      apiRequestHandler(VdcPbufApiConnectionPtr(this), request, method, params);
    }
  }
  // free the unpacked message
  vdcapi__message__free_unpacked(decodedMsg, NULL); // Free the message from unpack()
  // return error, in case one is left
  return err;
}


void VdcPbufApiConnection::closeAfterSend()
{
  closeWhenSent = true;
}


ApiValuePtr VdcPbufApiConnection::newApiValue()
{
  return ApiValuePtr(new PbufApiValue);
}


ErrorPtr VdcPbufApiConnection::sendRequest(const string &aMethod, ApiValuePtr aParams, VdcApiResponseCB aResponseHandler)
{
  PbufApiValuePtr params = boost::dynamic_pointer_cast<PbufApiValue>(aParams);
  ErrorPtr err;

  // create a message
  Vdcapi__Message msg = VDCAPI__MESSAGE__INIT;
  // find out which type and which submessage applies
  ProtobufCMessage *subMessageP = NULL;
  if (aMethod=="pong") {
    msg.type = VDCAPI__TYPE__VDC_NOTIFICATION_PONG;
    msg.vdc_send_pong = new Vdcapi__VdcNotificationPong;
    vdcapi__vdc__notification_pong__init(msg.vdc_send_pong);
    subMessageP = &(msg.vdc_send_pong->base);
  }
  else if (aMethod=="announce") {
    msg.type = VDCAPI__TYPE__VDC_SEND_ANNOUNCE;
    msg.vdc_send_announce = new Vdcapi__VdcSendAnnounce;
    vdcapi__vdc__send_announce__init(msg.vdc_send_announce);
    subMessageP = &(msg.vdc_send_announce->base);
  }
  else if (aMethod=="vanish") {
    msg.type = VDCAPI__TYPE__VDC_NOTIFICATION_VANISH;
    msg.vdc_send_vanish = new Vdcapi__VdcNotificationVanish;
    vdcapi__vdc__notification_vanish__init(msg.vdc_send_vanish);
    subMessageP = &(msg.vdc_send_vanish->base);
  }
  else if (aMethod=="pushProperty") {
    msg.type = VDCAPI__TYPE__VDC_NOTIFICATION_PUSH_PROPERTY;
    msg.vdc_send_push_property = new Vdcapi__VdcNotificationPushProperty;
    vdcapi__vdc__notification_push_property__init(msg.vdc_send_push_property);
    subMessageP = &(msg.vdc_send_push_property->base);
  }
  else {
    // no suitable submessage, cannot send
    LOG(LOG_INFO,"vdSM <- vDC (pbuf) method '%s' cannot be sent because no message is implemented for it at the pbuf level\n", aMethod.c_str());
    return ErrorPtr(new VdcApiError(500,"Error: Method is not implemented in the pbuf API"));
  }
  if (Error::isOK(err)) {
    if (aResponseHandler) {
      // method call expecting response
      msg.has_message_id = true; // has a messageID
      msg.message_id = ++requestIdCounter; // use new ID
      // save response handler into our map so that it can be called later when answer arrives
      pendingAnswers[requestIdCounter] = aResponseHandler;
    }
    // now fill parameters into submessage
    if (params) {
      params->putObjectIntoMessageFields(*subMessageP);
    }
    // send
    err = sendMessage(&msg);
    // dispose allocated submessage
    protobuf_c_message_free_unpacked(subMessageP, NULL);
    // log
    if (aResponseHandler) {
      LOG(LOG_INFO,"vdSM <- vDC (pbuf) method call sent: requestid='%d', method='%s', params=%s\n", requestIdCounter, aMethod.c_str(), aParams ? aParams->description().c_str() : "<none>");
    }
    else {
      LOG(LOG_INFO,"vdSM <- vDC (pbuf) notification sent: method='%s', params=%s\n", aMethod.c_str(), aParams ? aParams->description().c_str() : "<none>");
    }
  }
  // done
  return err;
}