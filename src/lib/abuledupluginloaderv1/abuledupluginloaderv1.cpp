/**
  * @author Icham Sirat <icham.sirat@ryxeo.com>
  * @date 2013 09 24
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduPluginLoaderV1
  * @see The GNU Public License (GPL)
  *
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program; if not, write to the Free Software Foundation, Inc.,
  * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  */

#include "abuledupluginloaderv1.h"

#ifndef DEBUG_ABULEDUPLUGINLOADERV1
    #include "abuledudisableloggerv1.h"
#endif

bool AbulEduPluginLoaderV1::loadPlugins(const QString& pluginsLocation)
{
    _listPlugins = pluginByDir<QObject>(pluginsLocation);
    if(_listPlugins.isEmpty())
        return false;

//    qDebug() << "Nombre Plugins charges : " << _listPlugins.count();
    return true;
}
