#include <stdio.h>
#include <stdlib.h>
#include <QPainter>

#include "genqrencoder.h"

#define MAX_DATA_SIZE (7090 * 16) /* from the specification */
const char BOM[3] = { 0xEF, 0xBB, 0xBF };

GenQREncoder::GenQREncoder()
{
        _caseSensitive = 1;
        _eightBit = 0;
        _version = 0;
        _size = 6;
        _margin = 3;
        _structured = 0;
        _level = QR_ECLEVEL_L;
        _hint = QR_MODE_8;
}

QPixmap GenQREncoder::encodeText(QString text, bool structured)
{
        QByteArray a = text.toUtf8();
        if ( _useBom )
        {
                // insert BOM
                a.insert( 0, BOM );
        }
        QRcode* qrcode = encode( a.data() );
        if ( qrcode == NULL )
        {
                return NULL;
        }

        int realwidth = (qrcode->width + _margin * 2) * _size;
        QImage* image = new QImage( realwidth, realwidth, QImage::Format_Mono );
        QPainter painter(image);

        painter.fillRect( QRect( 0, 0, realwidth, realwidth ), QColor( Qt::white ) );

        QColor black( Qt::black );

        int i = 0;
        int x = 0;
        int y = 0;
        QRect rect( 0, 0, _size, _size );
        while ( y < qrcode->width )
        {
                x = 0;
                while ( x < qrcode->width )
                {
                        // only if a block is present (black)
                        if ( qrcode->data[i] & 1 )
                        {
                                rect.moveTo(_size * (_margin + x),
                                                _size * (_margin + y));
                                painter.fillRect( rect, black );
                        }
                        i++;
                        x++;
                }
                y++;
        }

        painter.end();
        QRcode_free(qrcode);
        QPixmap pixmap = QPixmap::fromImage(*image);
        return pixmap;
}


void GenQREncoder::setModuleSize( unsigned int size )
{
        _size = size;
}

unsigned int GenQREncoder::moduleSize()
{
        return _size;
}

void GenQREncoder::setMargin( unsigned int margin )
{
        _margin = margin;
}

unsigned int GenQREncoder::margin()
{
        return _margin;
}

void GenQREncoder::setUseBom( bool useBom )
{
        _useBom = useBom;
}

bool GenQREncoder::useBom()
{
        return _useBom;
}


void GenQREncoder::setErrorCorrection( GenQRConfig::ErrorCorrectionMode e )
{
        switch (e)
        {
                case GenQRConfig::L: _level = QR_ECLEVEL_L; break;
                case GenQRConfig::M: _level = QR_ECLEVEL_M; break;
                case GenQRConfig::Q: _level = QR_ECLEVEL_Q; break;
                case GenQRConfig::H: _level = QR_ECLEVEL_H; break;
                default:
                        _level = QR_ECLEVEL_L;
        }
}

GenQRConfig::ErrorCorrectionMode GenQREncoder::errorCorrection()
{
        switch (_level)
        {
                case QR_ECLEVEL_L: return GenQRConfig::L;
                case QR_ECLEVEL_M: return GenQRConfig::M;
                case QR_ECLEVEL_Q: return GenQRConfig::Q;
                case QR_ECLEVEL_H: return GenQRConfig::H;
                default:
                        return GenQRConfig::L;
        }
}


QRcode *GenQREncoder::encode(const char *intext)
{
        QRcode *code;

        if(_eightBit) {
                code = QRcode_encodeString8bit(intext, _version, _level);
        } else {
                code = QRcode_encodeString(intext, _version, _level, _hint, _caseSensitive);
        }

        return code;
}

QRcode_List *GenQREncoder::encodeStructured(const char *intext)
{
        QRcode_List *list;

        if(_eightBit) {
                list = QRcode_encodeString8bitStructured(intext, _version, _level);
        } else {
                list = QRcode_encodeStringStructured(intext, _version, _level, _hint, _caseSensitive);
        }

        return list;
}
