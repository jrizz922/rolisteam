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
#ifndef ABSTRACTMEDIACONTROLLER_H
#define ABSTRACTMEDIACONTROLLER_H

#include <QObject>
#include <memory>

class CleverURI;
class QUndoCommand;
class AbstractMediaContainerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString uuid READ uuid NOTIFY uuidChanged)
    Q_PROPERTY(CleverURI* uri READ uri WRITE setUri NOTIFY uriChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool localGM READ localGM WRITE setLocalGM NOTIFY localGMChanged)
    Q_PROPERTY(QString ownerId READ ownerId WRITE setOwnerId NOTIFY ownerIdChanged)
public:
    AbstractMediaContainerController(CleverURI* uri, QObject* parent= nullptr);
    virtual ~AbstractMediaContainerController() override;

    QString name() const;
    QString uuid() const;
    CleverURI* uri() const;
    virtual QString title() const;
    bool isActive() const;
    bool localGM() const;

    QString ownerId() const;

    virtual void saveData() const= 0;
    virtual void loadData() const= 0;

signals:
    void nameChanged();
    void uuidChanged(QString);
    void uriChanged();
    void closeContainer();
    void titleChanged();
    void activeChanged();
    void localGMChanged();
    void performCommand(QUndoCommand* cmd);
    void ownerIdChanged(QString id);

public slots:
    void setUri(CleverURI* uri);
    void setName(const QString& name);
    void aboutToClose();
    void setActive(bool b);
    void setLocalGM(bool b);
    void setUuid(const QString& uuid);
    void setOwnerId(const QString& id);

protected:
    QString m_name;
    std::unique_ptr<CleverURI> m_uri;
    bool m_active= false;
    bool m_localGM= false;
    QString m_ownerId;
};

#endif // ABSTRACTMEDIACONTROLLER_H
