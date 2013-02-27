/****************************************************************************
**
** Copyright (c) 2010 Girish Ramakrishnan <girish@forwardbias.in>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "genjson.h"
#include "genjsonparser.cpp"

namespace Json {

QVariant parse(const QByteArray &json, QString *error)
{
    JsonLexer lexer(json);
    JsonParser parser;
    if (!parser.parse(&lexer) && error) {
        *error = parser.errorMessage();
    }
    return parser.result();
}

static QByteArray escape(const QVariant &variant)
{
    QString str = variant.toString();
    QByteArray res;
    res.reserve(str.length());
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\b') {
            res += "\\b";
        } else if (str[i] == '\f') {
            res += "\\f";
        } else if (str[i] == '\n') {
            res += "\\n";
        } else if (str[i] == '\r') {
            res += "\\r";
        } else if (str[i] == '\t') {
            res += "\\t";
        } else if (str[i] == '\"') {
            res += "\\\"";
        } else if (str[i] == '\\') {
            res += "\\\\";
        } else if (str[i].unicode() > 127) {
            res += "\\u" + QString::number(str[i].unicode(), 16).rightJustified(4, '0');
        } else {
            res += str[i].toAscii();
        }
    }
    return res;
}

QByteArray stringify(const QVariant &variant)
{
    QByteArray result;
    if (variant.type() == QVariant::List || variant.type() == QVariant::StringList) {
        result += "[";
        QVariantList list = variant.toList();
        for (int i = 0; i < list.count(); i++) {
            if (i != 0)
                result += ",";
            result += stringify(list[i]);
        }
        result += "]";
    } else if (variant.type() == QVariant::Map) {
        QVariantMap map = variant.toMap();
        QVariantMap::const_iterator it = map.constBegin();
        result += "{";
        while (it != map.constEnd()) {
            if (it != map.constBegin())
                result += ",";
            result += "\"" + escape(it.key()) + "\":";
            result += stringify(it.value());
            ++it;
        }
        result += "}";
    } else if (variant.type() == QVariant::String || variant.type() == QVariant::ByteArray) {
        result = "\"" + escape(variant) + "\"";
    } else if (variant.type() == QVariant::Double || (int)variant.type() == (int)QMetaType::Float) {
        result.setNum(variant.toDouble(), 'g', 15);
    } else if (variant.type() == QVariant::Bool) {
        result = variant.toBool() ? "true" : "false";
    } else if (variant.type() == QVariant::Invalid) {
        result = "null";
    } else if (variant.type() == QVariant::ULongLong) {
        result = QByteArray::number(variant.toULongLong());
    } else if (variant.canConvert<qlonglong>()) {
        result = QByteArray::number(variant.toLongLong());
    } else if (variant.canConvert<QString>()) {
        result = stringify(variant);
    }

    return result;
}

}

