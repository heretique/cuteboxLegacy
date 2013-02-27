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
const QString DROPBOX_VERSION = "/0";

enum GenDropboxWSRequestIDs
{
    WSReqNone,
    WSReqToken,
    WSReqAccountInfo,
    WSReqAccount,
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
        "/token",
        HttpGET,
        AuthNone,
        true,
        true
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
        WSReqAccount,
        DROPBOX_HOST,
        "/account",
        HttpGET,
        AuthNone,
        true,
        true
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
