
/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "Segment.h"
#include "SegmentPrivate.h"
#include "backend/worksheet/Image.h"
#include "backend/worksheet/CustomItem.h"
#include "backend/worksheet/Worksheet.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDesktopWidget>

#include <KLocale>

Segment::Segment(Image* image): d_ptr(new SegmentPrivate(this)),
    m_image(image), length(0) {
    m_image->scene()->addItem(this->graphicsItem());
    init();
}

Segment::~Segment() {
}

void Segment::init() {
    Q_D(Segment);
    d->scaleFactor = Worksheet::convertToSceneUnits(1, Worksheet::Inch)/QApplication::desktop()->physicalDpiX();
}

QGraphicsItem* Segment::graphicsItem() const {
    return d_ptr;
}

void Segment::setParentGraphicsItem(QGraphicsItem* item) {
    Q_D(Segment);
    d->setParentItem(item);
}

void Segment::retransform() {
    Q_D(Segment);
    d->retransform();
}

bool Segment::isVisible() const {
    Q_D(const Segment);
    return d->isVisible();
}

void Segment::setVisible(bool on) {
    Q_D(Segment);
    d->setVisible(on);
}
//##############################################################################
//####################### Private implementation ###############################
//##############################################################################
SegmentPrivate::SegmentPrivate(Segment *owner)
    : m_hovered(false),
      q(owner){
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setVisible(false);
}

/*!
    calculates the position and the bounding box of the item. Called on geometry or properties changes.
 */
void SegmentPrivate::retransform(){
    foreach (QLine* line, q->path){
        linePath.moveTo(line->p1());
        linePath.lineTo(line->p2());
    }
    boundingRectangle = linePath.boundingRect();
    recalcShapeAndBoundingRect();
}

/*!
    Returns the outer bounds of the item as a rectangle.
 */
QRectF SegmentPrivate::boundingRect() const{
    return transformedBoundingRectangle;
}

/*!
    Returns the shape of this item as a QPainterPath in local coordinates.
*/
QPainterPath SegmentPrivate::shape() const{
    return itemShape;
}

/*!
  recalculates the outer bounds and the shape of the item.
*/
void SegmentPrivate::recalcShapeAndBoundingRect(){
    prepareGeometryChange();
    QMatrix matrix;
    matrix.scale(scaleFactor, scaleFactor);
    transformedBoundingRectangle = matrix.mapRect(boundingRectangle);
    itemShape = QPainterPath();
    itemShape.addRect(transformedBoundingRectangle);
}

void SegmentPrivate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPen pen(Qt::green);
    QPen hoveredPen = QPen(QColor(128,179,255), 3, Qt::SolidLine);
    float hoveredOpacity = 0.6;
    QPen selectedPen = QPen(Qt::darkBlue, 3, Qt::SolidLine);
    float selectedOpacity = 0.3;

    painter->save();
    painter->setPen(pen);
    painter->scale(scaleFactor, scaleFactor);
    painter->drawPath(linePath);
    painter->restore();

    if (m_hovered && !isSelected()) {
        painter->setPen(hoveredPen);
        painter->setOpacity(hoveredOpacity);
        painter->drawPath(itemShape);
    }

    if (isSelected()) {
        painter->setPen(selectedPen);
        painter->setOpacity(selectedOpacity);
        painter->drawPath(itemShape);
    }
}

void SegmentPrivate::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
    if (!isSelected()) {
        m_hovered = true;
        update();
    }
}

void SegmentPrivate::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
    if (m_hovered) {
        m_hovered = false;
        update();
    }
}

QVariant SegmentPrivate::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if ( change == QGraphicsItem::ItemSelectedChange && value == true ) {
        int count = 0;
        q->m_image->beginMacro(i18n("%1:draw points over segment", q->m_image->name()));
        foreach (QLine* line, q->path) {
            int l = (line->y1() > line->y2())?line->y2():line->y1();
            int h = (line->y1() > line->y2())?line->y1():line->y2();
            for (int i = l; i <= h; i++) {
                if (count%q->m_image->pointSeparation() == 0) {
                    bool positionUsed = false;
                    QList<CustomItem*> imageItemsList = q->m_image->children<CustomItem>(AbstractAspect::IncludeHidden);
                    foreach (CustomItem* item, imageItemsList) {
                        if ( item->position().point == QPoint(line->x1(), i)*scaleFactor )
                            positionUsed = true;
                    }

                    if (!positionUsed) {
                        CustomItem* item = new CustomItem(i18n("CustomItem"));
                        item->setHidden(true);
                        item->setPosition(QPoint(line->x1(), i)*scaleFactor);
                        q->m_image->addChild(item);
                        q->m_image->updateData(item);
                    }
                }
                count++;
            }
        }
        q->m_image->endMacro();
    }
    return QGraphicsItem::itemChange(change, value);
}
