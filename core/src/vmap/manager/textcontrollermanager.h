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
#ifndef TEXTCONTROLLERMANAGER_H
#define TEXTCONTROLLERMANAGER_H

#include <QObject>
#include <QPointer>
#include <memory>
#include <vector>

#include "updater/textcontrollerupdater.h"
#include "visualitemcontrollermanager.h"

namespace vmap
{
class TextController;
}
class VectorialMapController;
class TextControllerUpdater;
class TextControllerManager : public VisualItemControllerManager
{
    Q_OBJECT
public:
    TextControllerManager(VectorialMapController* ctrl);

    QString addItem(const std::map<QString, QVariant>& params) override;
    void addController(vmap::VisualItemController* controller) override;
    void removeItem(const QString& id) override;
    void processMessage(NetworkMessageReader* msg) override;
    bool loadItem(const QString& id) override;

    const std::vector<vmap::TextController*> controllers() const;

signals:
    void textControllerCreated(vmap::TextController* ctrl);

private:
    vmap::TextController* findController(const QString& id);

private:
    std::vector<std::unique_ptr<vmap::TextController>> m_controllers;
    QPointer<VectorialMapController> m_ctrl;
    std::unique_ptr<TextControllerUpdater> m_updater;
};

#endif // TEXTCONTROLLERMANAGER_H
