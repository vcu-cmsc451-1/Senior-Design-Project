#ifndef OVR_WRAPPER_H
#define OVR_WRAPPER_H

#include <cstddef>
#include <unordered_map>
#include <string>
#include <OVR_ErrorCode.h>
#include <OVR_CAPI_0_5_0.h>

namespace ovr {

std::unordered_map <ovrResult, std::string> const exception_msg = {
    {  0   , "Success" },
    {  1001, "Success (Not Visible)" },
    { -1000, "Memory Allocation Failure" },
    { -1001, "Socket Creation Failure" },
    { -1002, "Invalid HMD" },
    { -1003, "Timed Out" },
    { -1004, "System Or Component Not Initialized" },
    { -1005, "Invalid Parameter" },
    { -1006, "Generic Service Error" },
    { -1007, "HMD Does Not Exist" },
    { -2000, "First Audio Error" },
    { -2999, "Last Audio Error" },
    { -3000, "Generic Initialization Error" },
    { -3001, "Couldn't Load LibOVRT" },
    { -3002, "LibOVRRT Version Incompatibility" },
    { -3003, "Couldn't Connect To OVR Service" },
    { -3004, "OVR Service Version Incompatibility" },
    { -3005, "OS Version Incompatibility" },
    { -3006, "Unable To Initialize HMD Display" },
    { -3007, "Unable To Start Server" },
    { -3008, "Attempted To Reinitialize" },
    { -4000, "Headset Has No Bundle Adjustment Data" },
    { -4001, "USB Cannot Handle Frame Bandwidth" }
};

struct exception
{
    ovrResult err;

    operator std::string() const { return exception_msg.at(err); }
    exception (ovrResult e): err(e) {}
};

struct ovr
{
    ovr()
    {
	auto result = ovr_Initialize(nullptr);
	if (result < 0)
	    throw exception(result);
    }

    ~ovr()
    {
	ovr_Shutdown();
    }
};

class sensor: protected ovr
{
    ovrHmd session;
    ovrHmdDesc desc;

public:
    auto getTrackingState() { return ovrHmd_GetTrackingState (session, ovr_GetTimeInSeconds()); }
    auto orientation() { return getTrackingState().HeadPose.ThePose.Orientation; }

    explicit operator bool() { return session; }

    sensor()
    {
	session = ovrHmd_Create(0);

        if (session)
	    desc = *session;

	//res = desc.Resolution;
    }

    ~sensor()
    {
	ovrHmd_Destroy(session);
    }
};

} // namespace ovr

#endif // OVR_WRAPPER_H
