/*  QBitStatus.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  Copyright (c) 2011-2024 Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef Q_BIT_STATUS_H
#define Q_BIT_STATUS_H

#include <QString>
#include <QRect>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QEFrameworkLibraryGlobal.h>

/// Bit wise display of integer values.
///
/// This class is a BitStatus widget based on directly QWidget.
/// It provides similar functionality to that provided by the edm/medm/dephi
/// widgets of the same name.
///
class QE_FRAMEWORK_LIBRARY_SHARED_EXPORT QBitStatus : public QWidget {
   Q_OBJECT

public:
   enum Orientations { LSB_On_Right, LSB_On_Bottom, LSB_On_Left, LSB_On_Top };
   Q_ENUM (Orientations)

   enum Shapes { Rectangle, Circle };
   Q_ENUM (Shapes)

   // Declare in type order.
   //
   Q_PROPERTY (int     value               READ getValue                WRITE setValue)
   Q_PROPERTY (int     numberOfBits        READ getNumberOfBits         WRITE setNumberOfBits)
   Q_PROPERTY (int     shift               READ getShift                WRITE setShift)

   Q_PROPERTY (Orientations Orientation    READ getOrientation          WRITE setOrientation)
   Q_PROPERTY (Shapes  shape               READ getShape                WRITE setShape)

   // If draw borders are off, a gap of zero means right and left pixel positions of
   // consecutive items are adjactent pixels. If draw borders are on, a gap of zero means
   // the borders of consecutive items use the same pixel position.
   //
   Q_PROPERTY (int     gap                 READ getGap                  WRITE setGap)

   /* The revserve polarity mask applies to value AFTER the shift.
    */
   Q_PROPERTY (QString reversePolarityMask READ getReversePolarityMask  WRITE setReversePolarityMask)

   /* The clear (aka inhibit aka not applicable) masks apply to value AFTER reversePolarityMask.
    */
   Q_PROPERTY (QString onClearMask         READ getOnClearMask          WRITE setOnClearMask)
   Q_PROPERTY (QString offClearMask        READ getOffClearMask         WRITE setOffClearMask)

   Q_PROPERTY (QColor  boarderColour       READ getBorderColour         WRITE setBorderColour)
   Q_PROPERTY (QColor  invalidColour       READ getInvalidColour        WRITE setInvalidColour)
   Q_PROPERTY (QColor  onColour            READ getOnColour             WRITE setOnColour)
   Q_PROPERTY (QColor  offColour           READ getOffColour            WRITE setOffColour)
   Q_PROPERTY (QColor  clearColour         READ getClearColour          WRITE setClearColour)

   Q_PROPERTY (bool    drawBorder          READ getDrawBorder           WRITE setDrawBorder)
   Q_PROPERTY (bool    isValid             READ getIsValid              WRITE setIsValid)
   Q_PROPERTY (bool    isActive            READ getIsActive             WRITE setIsActive)

private:
   // class member variable names start with m so as not to clash with
   // the propery names.
   // NOTE: Where possible I spell colour properly.
   //
   QColor mBorderColour;
   QColor mOnColour;
   QColor mOffColour;
   QColor mInvalidColour;
   QColor mClearColour;

   bool mDrawBorder;
   int  mGap;
   int  mNumberOfBits;      // 1 .. 32
   int  mShift;             // 0 .. 31
   int  mReversePolarityMask;
   int  mOnClearMask;
   int  mOffClearMask;
   bool mIsActive;          // i.e. is connected in CA speak
   bool mIsValid;
   int mValue;
   enum Orientations mOrientation;
   enum Shapes mShape;


   // Note: the getXxxxColour functions (line 125-ish) gets the Xxxx property colour.
   // The getXxxxPaintColour functions return actual colour to for drawing the widget.
   //
   QColor getBorderPaintColour () const;
   QColor getOffPaintColour () const;
   QColor getOnPaintColour () const;
   QColor getInvalidPaintColour () const;

   // Like painter drawRect or drawEllipse, but bounded by rect, i.e.sensible.
   //
   void drawItem  (QPainter & painter, const QRect & rect);

   void paintEvent (QPaintEvent *event);

   static QString intToMask (const int n);
   static int maskToInt (const QString mask);

protected:
   void setIsActive (const bool value);
   bool getIsActive () const;

public:
   // Constructor
   //
   explicit QBitStatus (QWidget *parent = 0);
   virtual ~QBitStatus () {}

   virtual QSize sizeHint () const;


   // Property functions
   //
   void setBorderColour (const QColor value);
   QColor getBorderColour () const;

   void setOnColour (const QColor value);
   QColor getOnColour () const;

   void setOffColour (const QColor value);
   QColor getOffColour () const;

   void setInvalidColour (const QColor value);
   QColor getInvalidColour () const;

   void setClearColour (const QColor value);
   QColor getClearColour () const;

   void setDrawBorder (const bool value);
   bool getDrawBorder () const;

   void setNumberOfBits (const int value);
   int getNumberOfBits () const;

   void setGap (const int value);
   int getGap () const;

   void setShift (const int value);
   int getShift () const;

   void setOnClearMask (const QString value);
   QString getOnClearMask () const;

   void setOffClearMask (const QString value);
   QString getOffClearMask () const;

   void setReversePolarityMask (const QString value);
   QString getReversePolarityMask () const;

   void setIsValid (const bool value);
   bool getIsValid () const;

   void setOrientation (const enum Orientations value);
   enum Orientations getOrientation () const;

   void setShape (const enum Shapes value);
   enum Shapes getShape () const;

public slots:
   void setValue (const int value);

public:
   int getValue () const;

};

#ifdef QE_DECLARE_METATYPE_IS_REQUIRED
Q_DECLARE_METATYPE (QBitStatus::Orientations)
Q_DECLARE_METATYPE (QBitStatus::Shapes)
#endif

#endif // Q_BIT_STATUS_H
