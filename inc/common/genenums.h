#ifndef _VIEWIDS_H_
#define _VIEWIDS_H_



enum GenViewAnimType
{
    GenFromRight,
    GenFromLeft,
    GenFromTop,
    GenFromBottom
};

enum GenWatchedDrive
    {
        DriveC = 0,
        DriveD,
        DriveE,
        DriveF
    };

enum GenHTTPRequestType
{
    HttpGET, HttpPOST, HttpDELETE
};

enum GenWSAuthenticationType
    {
    AuthNone, AuthApp, AuthUser
    };

#endif // _VIEWIDS_H_
