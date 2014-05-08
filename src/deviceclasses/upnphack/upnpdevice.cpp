//
//  upnpdevice.cpp
//  vdcd
//

#include "upnpdevice.hpp"

#include "fnv.hpp"

#include "buttonbehaviour.hpp"
#include "lightbehaviour.hpp"


using namespace p44;


UpnpDevice::UpnpDevice(UpnpDeviceContainer *aClassContainerP, string aLocation, string aUuid) :
  Device((DeviceClassContainer *)aClassContainerP),
  descriptionURL(aLocation),
  upnpDeviceUUID(aUuid),
  presenceTicket(0)
{
  // a demo device is a light which shows its dimming value as a string of 0..50 hashes on the console
  // - is a audio device
  primaryGroup = group_cyan_audio;
  // - derive a dSUID
	deriveDsUid();
}



void UpnpDevice::checkPresence(PresenceCB aPresenceResultHandler)
{
  SsdpSearchPtr srch = SsdpSearchPtr(new SsdpSearch(SyncIOMainLoop::currentMainLoop()));
  presenceTicket = MainLoop::currentMainLoop().executeOnce(boost::bind(&UpnpDevice::timeoutHandler, this, aPresenceResultHandler, srch), 3*Second);
  srch->startSearch(boost::bind(&UpnpDevice::presenceHandler, this, aPresenceResultHandler, _1, _2), upnpDeviceUUID.c_str(), true);
}



void UpnpDevice::presenceHandler(PresenceCB aPresenceResultHandler, SsdpSearch *aSsdpSearchP, ErrorPtr aError)
{
  printf("Ping response notify\n%s\n", aSsdpSearchP->response.c_str());
  aPresenceResultHandler(true);
  aSsdpSearchP->stopSearch();
  MainLoop::currentMainLoop().cancelExecutionTicket(presenceTicket);
}


void UpnpDevice::timeoutHandler(PresenceCB aPresenceResultHandler, SsdpSearchPtr aSrch)
{
  aSrch->stopSearch();
  aPresenceResultHandler(false);
  presenceTicket = 0;
}



void UpnpDevice::deriveDsUid()
{
  Fnv64 hash;

  // UPnP devices do have a uuid, use this as dSUID
  dSUID.setAsString(upnpDeviceUUID);
}



#pragma mark - property access


enum {
  descriptionURL_key,
  numProperties
};

static char upnpDevice_key;


int UpnpDevice::numProps(int aDomain, PropertyDescriptorPtr aParentDescriptor)
{
  return inherited::numProps(aDomain, aParentDescriptor)+numProperties;
}


PropertyDescriptorPtr UpnpDevice::getDescriptorByIndex(int aPropIndex, int aDomain, PropertyDescriptorPtr aParentDescriptor)
{
  static const PropertyDescription properties[numProperties] = {
    { "descriptionURL", apivalue_string, descriptionURL_key, OKEY(upnpDevice_key) }, // custom UPnP property revealing the description URL
  };
  return PropertyDescriptorPtr(new StaticPropertyDescriptor(&properties[aPropIndex]));
}


// access to all fields
bool UpnpDevice::accessField(PropertyAccessMode aMode, ApiValuePtr aPropValue, PropertyDescriptorPtr aPropertyDescriptor)
{
  if (aPropertyDescriptor->hasObjectKey(upnpDevice_key)) {
    if (aMode==access_read) {
      // read properties
      switch (aPropertyDescriptor->fieldKey()) {
          // Description properties
        case descriptionURL_key:
          aPropValue->setStringValue(descriptionURL); return true;
          return true;
      }
    }
    else {
      // write properties
    }
  }
  // not my field, let base class handle it
  return inherited::accessField(aMode, aPropValue, aPropertyDescriptor);
}



#pragma mark - description/shortDesc


string UpnpDevice::hardwareGUID()
{
  return "uuid:" + upnpDeviceUUID;
}


string UpnpDevice::modelName()
{
  return "Demo UPnP";
}


string UpnpDevice::description()
{
  string s = inherited::description();
  string_format_append(s, "- UPnP device with description URL: %s and UUID: %s\n", descriptionURL.c_str(), upnpDeviceUUID.c_str());
  return s;
}

