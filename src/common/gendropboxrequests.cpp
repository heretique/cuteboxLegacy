/*
 * gendropboxrequests.cpp
 *
 *  Created on: Jan 25, 2011
 *      Author: catalin.moldovan
 */

#include "gendropboxrequests.h"

const GenWSRequestInfo* GetDropboxRequestById(int requestId)
{
    for (int i = 0; i < GenDropboxWSRequestsCount; ++i)
        if (GenDropboxWSRequests[i]._id == requestId)
            return &(GenDropboxWSRequests[i]);

    return NULL;
}
