/*
 * gendropboxrequests.h
 *
 *  Created on: Jan 25, 2011
 *      Author: catalin.moldovan
 */

#ifndef GENDROPBOXREQUESTS_H_
#define GENDROPBOXREQUESTS_H_

#include "gentypes.h"

const QString DROPBOX_HOST = "api.dropbox.com";
const QString DROPBOX_FILES = "api-content.dropbox.com";
const QString DROPBOX_AUTHORIZE = "www.dropbox.com";
const QString REDIRECT_URL = "http://genera.ro";
const QString REDIRECT_HOST = "www.genera.ro";
const QString DROPBOX_VERSION = "/1";

enum GenDropboxWSRequestIDs
{
    WSReqNone,
    WSReqToken,
    WSReqAutorize,
    WSReqAccessToken,
    WSReqAccountInfo,
    WSReqFileUpload,
    WSReqFileDownload,
    WSReqGetMetadata,
    WSReqGetThumbnails,
    WSReqFileCopy,
    WSReqCreateFolder,
    WSReqFileDelete,
    WSReqFileMove
};

static GenWSRequestInfo GenDropboxWSRequests[] = 
{
    {
        WSReqToken,
        DROPBOX_HOST,
        "/oauth/request_token",
        HttpPOST,
        AuthApp,
        true,
        true
    },
    {
        WSReqAutorize,
        DROPBOX_AUTHORIZE,
        "/oauth/authorize",
        HttpGET,
        AuthNone,
        true,
        true
    },
    {
        WSReqAccessToken,
        DROPBOX_HOST,
        "/oauth/access_token",
        HttpPOST,
        AuthUser,
        true,
        false
    },
    {
        WSReqAccountInfo,
        DROPBOX_HOST,
        "/account/info",
        HttpGET,
        AuthUser,
        true,
        false
    },
    {
        WSReqGetMetadata,
        DROPBOX_HOST,
        "/metadata/",
        HttpGET,
        AuthUser,
        true,
        false
    },
    {
        WSReqFileDownload,
        DROPBOX_FILES,
        "/files/",
        HttpGET,
        AuthUser,
        true,
        false
    },
    {
        WSReqFileUpload,
        DROPBOX_FILES,
        "/files/",
        HttpPOST,
        AuthUser,
        true,
        false
    },
    {
        WSReqGetThumbnails,
        DROPBOX_FILES,
        "/thumbnails/",
        HttpGET,
        AuthUser,
        true,
        true
    },
    {
        WSReqFileCopy,
        DROPBOX_HOST,
        "/fileops/copy",
        HttpGET,
        AuthUser,
        true,
        true
    },
    {
        WSReqCreateFolder,
        DROPBOX_HOST,
        "/fileops/create_folder",
        HttpGET,
        AuthUser,
        true,
        true
    },
    {
        WSReqFileDelete,
        DROPBOX_HOST,
        "/fileops/delete",
        HttpGET,
        AuthUser,
        true,
        true
    },
    {
        WSReqFileMove,
        DROPBOX_HOST,
        "/fileops/move",
        HttpGET,
        AuthUser,
        true,
        true
    }
};

static const int GenDropboxWSRequestsCount = sizeof( GenDropboxWSRequests ) / sizeof(GenWSRequestInfo);

const GenWSRequestInfo* GetDropboxRequestById( int requestId );


#endif /* GENDROPBOXREQUESTS_H_ */
