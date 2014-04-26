/************************************************************************************

Filename    :   OVR_Win32_HIDDevice.h
Content     :   Win32 HID device implementation.
Created     :   February 22, 2013
Authors     :   Lee Cooper

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#ifndef OVR_Win32_HIDDevice_h
#define OVR_Win32_HIDDevice_h

#include "../OVR_Platform.h"
#include "OVR_HIDDevice.h"

#include <windows.h>
#include <setupapi.h>

//-------------------------------------------------------------------------------------
// Define needed "hidsdi.h" functionality to avoid requiring DDK installation.
// #include "hidsdi.h"

#ifndef _HIDSDI_H
#define _HIDSDI_H
#include <pshpack4.h>

#define HIDP_STATUS_SUCCESS (0x11 << 16)
struct HIDP_PREPARSED_DATA;

struct HIDD_ATTRIBUTES
{
    ULONG   Size; // = sizeof (struct _HIDD_ATTRIBUTES)
    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;
};

struct HIDP_CAPS
{
    USHORT   Usage;
    USHORT   UsagePage;
    USHORT   InputReportByteLength;
    USHORT   OutputReportByteLength;
    USHORT   FeatureReportByteLength;
    USHORT   Reserved[17];

    USHORT   NumberLinkCollectionNodes;
    USHORT   NumberInputButtonCaps;
    USHORT   NumberInputValueCaps;
    USHORT   NumberInputDataIndices;
    USHORT   NumberOutputButtonCaps;
    USHORT   NumberOutputValueCaps;
    USHORT   NumberOutputDataIndices;
    USHORT   NumberFeatureButtonCaps;
    USHORT   NumberFeatureValueCaps;
    USHORT   NumberFeatureDataIndices;
};

#include <poppack.h>
#endif


namespace OVR { namespace Platform { 

class Win32HIDDeviceManager;
class Win32DeviceManagerThread;
class DeviceManager;

//-------------------------------------------------------------------------------------
// ***** Win32 HIDDevice

class Win32HIDDevice : public HIDDevice
{
public:

  Win32HIDDevice(HIDDeviceManager* manager);

    // This is a minimal constructor used during enumeration for us to pass
    // a HIDDevice to the visit function (so that it can query feature reports). 
  Win32HIDDevice(HIDDeviceManager* manager, HANDLE device);

  virtual ~Win32HIDDevice();

    bool HIDInitialize(const String& path);
    void HIDShutdown();

    // OVR::HIDDevice
	bool SetFeatureReport(UByte* data, UInt32 length);
	bool GetFeatureReport(UByte* data, UInt32 length);
    

    // DeviceManagerThread::Notifier
    void OnReadReadyEvent();
    double OnTicks(double tickSeconds);
    bool OnDeviceMessage(DeviceMessageType messageType, const String& devicePath, bool* error);

private:
    Win32HIDDeviceManager * getHidManager();
    Win32DeviceManagerThread * getThread();

    bool openDevice();
    bool initInfo();
    bool initializeRead();
    bool processReadResult();
    void closeDevice();
    void closeDeviceOnIOError();

	HANDLE              Device;
    HIDDeviceDesc       DevDesc; 

    OVERLAPPED          ReadOverlapped;
    bool                ReadRequested;

};

//-------------------------------------------------------------------------------------
// ***** Win32 HIDDeviceManager

class Win32HIDDeviceManager : public HIDDeviceManager
{
  friend class Win32HIDDevice;
public:

  Win32HIDDeviceManager(DeviceManager* manager);
    virtual ~Win32HIDDeviceManager();

    virtual bool Enumerate(HIDEnumerateVisitor* enumVisitor);
    virtual OVR::HIDDevice* Open(const String& path);

    // Fills HIDDeviceDesc by using the path.
    // Returns 'true' if successful, 'false' otherwise.
    bool GetHIDDeviceDesc(const String& path, HIDDeviceDesc* pdevDesc) const;

    GUID GetHIDGuid() { return HidGuid; }
    
    static HIDDeviceManager* CreateInternal(DeviceManager* manager);

private:

    HMODULE hHidLib;
    GUID    HidGuid;

    // Macros to declare and resolve needed functions from library.
#define OVR_DECLARE_HIDFUNC(func, rettype, args)   \
typedef rettype (__stdcall *PFn_##func) args;  \
static PFn_##func      func;
#define OVR_RESOLVE_HIDFUNC(func) \
func = (PFn_##func)::GetProcAddress(hHidLib, #func)

    OVR_DECLARE_HIDFUNC(HidD_GetHidGuid,            void,    (GUID *hidGuid));
    OVR_DECLARE_HIDFUNC(HidD_SetNumInputBuffers,    BOOLEAN, (HANDLE hidDev, ULONG numberBuffers));
    OVR_DECLARE_HIDFUNC(HidD_GetFeature,            BOOLEAN, (HANDLE hidDev, PVOID buffer, ULONG bufferLength));
    OVR_DECLARE_HIDFUNC(HidD_SetFeature,            BOOLEAN, (HANDLE hidDev, PVOID buffer, ULONG bufferLength));
    OVR_DECLARE_HIDFUNC(HidD_GetAttributes,         BOOLEAN, (HANDLE hidDev, HIDD_ATTRIBUTES *attributes));
    OVR_DECLARE_HIDFUNC(HidD_GetManufacturerString, BOOLEAN, (HANDLE hidDev, PVOID buffer, ULONG bufferLength));
    OVR_DECLARE_HIDFUNC(HidD_GetProductString,      BOOLEAN, (HANDLE hidDev, PVOID buffer, ULONG bufferLength));
    OVR_DECLARE_HIDFUNC(HidD_GetSerialNumberString, BOOLEAN, (HANDLE hidDev, PVOID buffer, ULONG bufferLength));
    OVR_DECLARE_HIDFUNC(HidD_GetPreparsedData,      BOOLEAN, (HANDLE hidDev, HIDP_PREPARSED_DATA **preparsedData));
    OVR_DECLARE_HIDFUNC(HidD_FreePreparsedData,     BOOLEAN, (HIDP_PREPARSED_DATA *preparsedData));
    OVR_DECLARE_HIDFUNC(HidP_GetCaps,               NTSTATUS,(HIDP_PREPARSED_DATA *preparsedData, HIDP_CAPS* caps));

    // Helper functions to fill in HIDDeviceDesc from open device handle.
    bool initVendorProductVersion(HANDLE hidDev, HIDDeviceDesc* desc) const;
    bool initUsage(HANDLE hidDev, HIDDeviceDesc* desc) const;
    void initStrings(HANDLE hidDev, HIDDeviceDesc* desc) const;

    bool getFullDesc(HANDLE hidDev, HIDDeviceDesc* desc) const;
};

}} // namespace OVR::Win32

#endif // OVR_Win32_HIDDevice_h
