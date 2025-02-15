#include "utils.h"
#include <QChar>
#include <QTextCodec>

Utils::Utils()
{

}

QString Utils::toLuaUnicode(QString &s)
{
    QString rt = "";
    QTextCodec *gbk = QTextCodec::codecForLocale();
    foreach(QChar qc, s) {
        ushort u = qc.unicode();
        if(u > 127) {
            QByteArray encoded = gbk->fromUnicode(qc);
            for(int i=0; i<encoded.length(); i++)
                rt += QString("\\x%1").arg(QString::number((int)encoded.at(i)&0xff, 16));
        } else {
            rt += qc;
        }
    }
    return rt;
}
