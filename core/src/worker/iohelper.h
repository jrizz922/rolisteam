/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef IOHELPER_H
#define IOHELPER_H

#include <QJsonDocument>
#include <QString>
#include <QVariant>
#include <map>

class VMap;
class CleverURI;
class VectorialMapController;
class MediaControllerBase;
class IOHelper
{
public:
    IOHelper();

    static QByteArray loadFile(const QString& file);
    static bool loadVMap(VMap* vmap, CleverURI* uri, VectorialMapController* ctrl);
    static bool loadToken(const QString& filename, std::map<QString, QVariant>& params);

    static std::map<QString, QVariant> saveController(MediaControllerBase* media);

    static QJsonObject byteArrayToJsonObj(const QByteArray& data);
    static QJsonArray byteArrayToJsonArray(const QByteArray& data);
};

#endif // IOHELPER_H