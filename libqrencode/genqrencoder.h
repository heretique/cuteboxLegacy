#ifndef GENQRENCODER_H
#define GENQRENCODER_H

#include <QPixmap>

#include "qrencode.h"

typedef struct GenQRConfig
{
        enum SelectionMode
        {
                Selection, Clipboard
        };

        enum ErrorCorrectionMode
        {
                L, M, Q, H
        };

        SelectionMode selectionMode;
        unsigned int moduleSize;
        unsigned int margin;
        ErrorCorrectionMode errorCorrectionMode;

        /**
         * Whether to encode directly while the user
         * is typing.
         */
        bool directEncode;

        /*
         * Whether to use UTF-8 BOM prefix
         */
        bool useBom;
} GenQRConfig;

class GenQREncoder
{

public:
        GenQREncoder();
        QPixmap encodeText(QString text, bool structured = false);

        void setModuleSize( unsigned int _size );
        unsigned int moduleSize();

        void setMargin( unsigned int margin );
        unsigned int margin();

        void setErrorCorrection( GenQRConfig::ErrorCorrectionMode e );
        GenQRConfig::ErrorCorrectionMode errorCorrection();

        void setUseBom( bool useBom );
        bool useBom();

private:
        QRcode *encode(const char *intext);
        QRcode_List *encodeStructured(const char *intext);
private:
    int _caseSensitive;
    int _eightBit;
    int _version;
    int _size;
    int _margin;
    int _structured;
    QRecLevel _level;
    QRencodeMode _hint;
    bool _useBom;
};


#endif // GENQRENCODER_H
