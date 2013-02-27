#include "genfilemanager.h"

GenFileManager::GenFileManager(QObject *parent)
    :QObject(parent), _spaceLimit(200)
{

}

GenFileManager::~GenFileManager()
{

}

int GenFileManager::spaceLimit() const
{
    return _spaceLimit;
}

void GenFileManager::setSpaceLimit(int spaceLimit)
{
    _spaceLimit = spaceLimit;
}

int GenFileManager::availableSpace() const
{
    // TODO: compute real space limit
    return _spaceLimit;
}

void GenFileManager::handleSpaceLimitChanged(int spaceLimit)
{
    _spaceLimit = spaceLimit;
}

void GenFileManager::handleNextInitStep()
{

}
