#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_


#include "gentypes.h"

const int NOID = -1;
const int VIEW_ANIMATION_DURATION = 300;
const int POPUP_ANIMATION_DURATION = 200;
const int MENU_CHANGE_ANIM_DURATION = 150;
const int NOTIFICATION_ANIMATION_DURATION = 150;

const int FILE_SIZE_UPLOAD_LIMIT = 300*1024*1024;

const int NOTIFICATION_AUTOCLOSE_TIME = 3000; // msec

const int LONG_TAP_THRESHOLD = 800; // miliseconds


// ACTION IDS
const ActionId ActionNONE = 0;
const ActionId ActionSETTINGS = 1;
const ActionId ActionADDNEW = 2;
const ActionId ActionLOCALFILESSELECTED = 3;
const ActionId ActionDROPBOXFILES = 4;
const ActionId ActionFILEACTIONS = 5;
const ActionId ActionBACK = 6;
const ActionId ActionEXIT = 7;
const ActionId ActionDROPBOXFILEDETAILS = 8;
const ActionId ActionDOWNLOADFILES = 9;
const ActionId ActionSENDLINK = 10;
const ActionId ActionGETTHUMBNAIL = 11;
const ActionId ActionLOCALFILEDETAILS = 12;
const ActionId ActionGOTOROOT = 13;
const ActionId ActionSELECTDOWNLOADFILES = 14;
const ActionId ActionOK = 15;
const ActionId ActionCANCEL = 16;
const ActionId ActionCANCELCURRENTFILEACTION = 17;
const ActionId ActionCOPYFILES = 18;
const ActionId ActionMOVEFILES = 19;
const ActionId ActionDELETEFILES = 20;
const ActionId ActionSTARTDOWNLOADFILES = 21;
const ActionId ActionSTARTCOPYFILES = 22;
const ActionId ActionSTARTMOVEFILES = 23;
const ActionId ActionSTARTDELETEFILES = 24;
const ActionId ActionADDNEWFOLDER = 25;
const ActionId ActionCREATEFOLDER = 26;
const ActionId ActionADDNEWPICTURE = 27;
const ActionId ActionADDNEWLOCALFILE = 28;
const ActionId ActionSTARTUPLOADFILES = 29;
const ActionId ActionOPENFILE = 30;
const ActionId ActionCHANGEDOWNLOADLOCATION = 31;
const ActionId ActionDOWNLOADLOCATIONSELECTED = 32;
const ActionId ActionUNLINKDEVICE = 33;
const ActionId ActionGETACCOUNTINFO = 34;
const ActionId ActionADDNEWAUDIO = 35;
const ActionId ActionADDNEWVIDEO = 36;
const ActionId ActionCOPYLINKTOCLIPBOARD = 37;
const ActionId ActionSENDLINKBYSMS = 38;
const ActionId ActionSENDLINKBYEMAIL = 39;
const ActionId ActionHELP = 40;
const ActionId ActionFORWARD = 41;
const ActionId ActionSHARE = 42;
const ActionId ActionQRCODE = 43;
const ActionId ActionCANCELSHARE = 44;

#ifdef FEATURE_PHOTO_WALL
const ActionId ActionOPENPHOTOWALL = 100;
#endif

#endif // _CONSTANTS_H_
