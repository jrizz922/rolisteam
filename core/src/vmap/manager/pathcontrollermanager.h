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
#ifndef PATHCONTROLLERMANAGER_H
#define PATHCONTROLLERMANAGER_H

#include <QObject>
#include <QPointer>
#include <memory>
#include <vector>

#include "visualitemcontrollermanager.h"

namespace vmap
{
class PathController;
}
class VectorialMapController;
class PathControllerUpdater;
class PathControllerManager : public VisualItemControllerManager
{
    Q_OBJECT
public:
    explicit PathControllerManager(VectorialMapController* ctrl, QObject* parent= nullptr);
    virtual ~PathControllerManager() override;

    QString addItem(const std::map<QString, QVariant>& params) override;
    void addController(vmap::VisualItemController* controller) override;
    void removeItem(const QString& id) override;
    void processMessage(NetworkMessageReader* msg) override;
    bool loadItem(const QString& id) override;
    const std::vector<vmap::PathController*> controllers() const;
signals:
    void pathControllerCreated(vmap::PathController* ctrl, bool editing);

private:
    void prepareController(vmap::PathController* ctrl);
    vmap::PathController* findController(const QString& id);

private:
    std::vector<std::unique_ptr<vmap::PathController>> m_controllers;
    QPointer<VectorialMapController> m_ctrl;
    std::unique_ptr<PathControllerUpdater> m_updater;
};

#endif // PATHCONTROLLERMANAGER_H
