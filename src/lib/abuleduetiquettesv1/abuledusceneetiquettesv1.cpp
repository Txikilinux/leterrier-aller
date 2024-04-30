/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GPL)
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

#include "abuledusceneetiquettesv1.h"

AbulEduSceneEtiquettesV1::AbulEduSceneEtiquettesV1(QObject *parent) :
    QGraphicsScene(parent)
{
    m_localDebug = false;
}

AbulEduSceneEtiquettesV1::~AbulEduSceneEtiquettesV1()
{
}

void AbulEduSceneEtiquettesV1::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    event->ignore();
}
