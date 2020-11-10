/*  QESpectrogram.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  Copyright (c) 2020 Australian Synchrotron
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
 *    andrews@ansto.gov.au
 */

#include "QESpectrogram.h"
#include <QDebug>
#include <QPainter>
#include <QECommon.h>

#define DEBUG qDebug () << "QESpectrogram" << __LINE__ << __FUNCTION__ << "  "

static const double minSpan = 1.0e-3;     /// duplicate

//------------------------------------------------------------------------------
//
QESpectrogram:: QESpectrogram (QWidget* parent) :
   QEAbstract2DData (parent)
{
   this->commonSetup();
}

//------------------------------------------------------------------------------
//
QESpectrogram::QESpectrogram (const QString& dataVariableName,
                              QWidget* parent):
   QEAbstract2DData (dataVariableName, parent)
{
   this->commonSetup();
}

//------------------------------------------------------------------------------
//
QESpectrogram::QESpectrogram (const QString& dataVariableName,
                              const QString& widthVariableName,
                              QWidget* parent) :
   QEAbstract2DData (dataVariableName, widthVariableName, parent)
{
   this->commonSetup();
}

//------------------------------------------------------------------------------
//
QESpectrogram::~QESpectrogram () {}

//------------------------------------------------------------------------------
//
void QESpectrogram::commonSetup ()
{
   // Create internal widget.
   //
   this->plotArea = new QWidget (NULL);
   this->layout = new QHBoxLayout (this);
   this->layout->setMargin (4);    // make a property?
   this->layout->setSpacing (0);
   this->layout->addWidget (this->plotArea);
   this->plotArea->setMouseTracking (true);
   this->plotArea->installEventFilter (this);

   // Set default property values.
   //
   this->setMinimumWidth  (40);
   this->setMinimumHeight (40);

   this->breakPoint1 = 32;    // default - sensible value
   this->breakPoint2 = 224;   // default - sensible value

   this->mUseFalseColour = true;
   this->mScaleWrap = 1;
   this->mOrientation = Qt::Horizontal;

   // Set up the pixel maps
   //
   for (int j = 0; j < 256; j++) {
      // Mono chrome grey scale - easy.
      //
      this->grayScalePixelMap [j].p[0] = j;
      this->grayScalePixelMap [j].p[1] = j;
      this->grayScalePixelMap [j].p[2] = j;
      this->grayScalePixelMap [j].p[3] = 0xFF;  // Alpha always 100%

      this->falseColourPixelMap [j] = this->getFalseColor (j);
   }
}

//------------------------------------------------------------------------------
//
QSize QESpectrogram::sizeHint () const
{
   return QSize (200, 100);
}

//------------------------------------------------------------------------------
//
bool QESpectrogram::eventFilter (QObject* watched, QEvent* event)
{
   const QEvent::Type type = event->type ();

   bool result = false;

   switch (type) {

      case QEvent::MouseMove:
         if (watched == this->plotArea) {
            const QMouseEvent* mouseEvent = static_cast<QMouseEvent *> (event);
            const QPoint pos = mouseEvent->pos ();
            this->spectrogramMouseMove (pos);
            result = true;   // event has been handled.
         }
         break;

      case QEvent::Paint:
         if (watched == this->plotArea) {
            this->paintSpectrogram ();
            result = true;   // event has been handled.
         }
         break;

      default:
         break;
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QESpectrogram::updateDataVisulation ()
{
   const TwoDimensionalData data = this->getData();
   const int number = data.count();

   // Any data?
   //
   if (number < 1) {
      // No - nothing to do - clear image.
      //
      this->image = QImage ();
      this->plotArea->update ();
      return;
   }

   double min = this->getMinimum();
   double max = this->getMaximum();

   if (this->getAutoScale()) {
      this->getDataMinMaxValues (min, max);
   }

   // Sanity check.
   //
   if (min > max) {
      // Nothing to do - clear image.
      //
      this->image = QImage ();
      this->plotArea->update ();
      return;
   }

   // Must avoid the divide by zero.
   //
   max = MAX (max, min + minSpan);

   const int scaledMin = 0;
   const int maxLoops = this->getScaleWrap() - 1;   // zero based count

   // Calc the spread between two break points - in this region only the
   // hue of the colout changes, not brightness or intensity.
   //
   const int wrapSpread = (this->breakPoint2 - this->breakPoint1);
   const int scaledMax = 255 + (maxLoops * wrapSpread);

   // Calc linear scaling constants m,c for y = m.x + c scaling
   //
   const double scale = double (scaledMax - scaledMin)/(max - min);
   const double offset = -scale * min;

   // 8 cases:
   // grey scale vs. false colour
   // data source: 1D vs. 2D
   // orientation: vertical vs horizontal
   //
   RgbPixelMaps* pixelMap = this->getUseFalseColour() ?
            &this->falseColourPixelMap :
            &this->grayScalePixelMap;

   int imageWidth;
   int imageHeight;
   this->getNumberRowsAndCols (true, imageHeight, imageWidth);

   // Swap width/height when not horizontal
   //
   if (this->getOrientation() != Qt::Horizontal) {
      int temp = imageWidth;
      imageWidth = imageHeight;
      imageHeight = temp;
   }

   // Create the new image
   //
   QImage workImage = QImage (imageWidth, imageHeight, QImage::Format_RGB32);

   for (int row = 0; row < imageHeight; row++) {
      rgbPixel* rowOut = (rgbPixel*)(workImage.scanLine (row));

      for (int col = 0; col < imageWidth; col++) {

         // Find source data row and col.
         //
         const int srcRow = this->getOrientation() == Qt::Horizontal ? row : col;
         const int srcCol = this->getOrientation() == Qt::Horizontal ? col : row;

         double value = this->getValue (srcRow, srcCol, min);

         // scale (y = m.x + c), wrap and limit.
         //
         value = scale*value + offset;
         value = LIMIT (value, scaledMin, scaledMax);

         int index = int (value);
         int loops = (index - this->breakPoint1) / wrapSpread;
         loops = MIN (loops, maxLoops);

         index = index - (loops*wrapSpread);    // wrap index.
         index = LIMIT (index, 0, 255);         // belts 'n' braces limit
         rowOut [col] = (*pixelMap) [index];    // look up and assign to image pixel.
      }
   }

   // Update "the" image and trigger a paintEvent.
   //
   this->image = workImage;
   this->plotArea->update ();
}

//------------------------------------------------------------------------------
// Essentially just paints the image - does no processing per se.
//
void QESpectrogram::paintSpectrogram ()
{
   QPainter painter (this->plotArea);

   QRect rect = this->plotArea->geometry();

   // We want rect in the plot area, not plot area in QESpectrogram.
   //
   rect.moveTo (0, 0);

   if (!this->image.isNull()) {
      painter.drawImage (rect, this->image);
   } else {
      // If the image does not exists, blank the image.
      //
      QColor bg ("#e0dcda");
      painter.fillRect (rect, bg);
   }
}

//------------------------------------------------------------------------------
//
void QESpectrogram::spectrogramMouseMove (const QPoint& pos)
{
   int numDataRows;
   int numDataCols;
   this->getNumberRowsAndCols (true, numDataRows, numDataCols);

   // Fractional position 0.0 .. 0.99999
   //
   const double fracX = double (pos.x ()) / double (this->plotArea->width());
   const double fracY = double (pos.y ()) / double (this->plotArea->height());

   // Swap width/height when not horizontal
   //
   int row;
   int col;
   if (this->getOrientation() == Qt::Horizontal) {
      row = int (fracY * numDataRows);
      col = int (fracX * numDataCols);
   } else {
      row = int (fracX * numDataRows);
      col = int (fracY * numDataCols);
   }

   // Special value used to indicate a no value.
   // This number is chosen as -(2.0 ** 48) because it:
   // a) can be exactly represented as a double, and
   // b) is a value that is not ever expected to 'turn up' as an actual value.
   //
   static const double noValue = -281474976710656.0;
   const double value = this->getValue (row, col, noValue);

   QString message;
   if (value != noValue) {
      message = QString ("row:%1  col:%2  value: %3")
            .arg (row, 3)
            .arg (col, 3)
            .arg (value);
   } else {
      message = "";
   }

   this->setReadOut (message);
}

//------------------------------------------------------------------------------
// Calculates a false color representation for an entry in the colour lookup table.
// Lifted from QEImage-imageProcessor.
//
QESpectrogram::rgbPixel
QESpectrogram::getFalseColor (const unsigned char value)
{
   const int max = 0xFF;
   const int half = 0x80;
   const int lightness_slope = 4;
   const int low_hue = 240;    // blue.
   const int high_hue = 0;     // red

   rgbPixel result;
   int h, l;
   QColor c;

   // Range of inputs broken into three bands:
   // [0 .. bp1], [bp1 .. bp2] and [bp2 .. max]
   //
   this->breakPoint1 = half / lightness_slope;                // 32
   this->breakPoint2 = max - (max - half) / lightness_slope;  // 224

   if (value < this->breakPoint1) {
      // Constant hue (blue), lightness ramps up to 128
      //
      h = low_hue;
      l = lightness_slope*value;
   }
   else if (value > this->breakPoint2) {
      // Constant hue (red), lightness ramps up from 128 to 255
      //
      h = high_hue;
      l = max - lightness_slope*(max-value);
   }
   else {
      // The bit in the middle.
      // Contant lightness, hue varies blue to red.
      //
      h = ((value - this->breakPoint1)*high_hue + (this->breakPoint2 - value)*low_hue) /
            (this->breakPoint2 - this->breakPoint1);
      l = half;
   }

   c.setHsl (h, max, l);     // Saturation always 100%

   result.p[0] = (unsigned char) c.blue();
   result.p[1] = (unsigned char) c.green();
   result.p[2] = (unsigned char) c.red();
   result.p[3] = (unsigned char) max;   // Alpha always 100%

   return result;
}

//==============================================================================
// Properties
//==============================================================================
//
void QESpectrogram::setUseFalseColour (const bool useFalseColour)
{
   this->mUseFalseColour = useFalseColour;
   this->updateDataVisulation();
}

//------------------------------------------------------------------------------
//
bool QESpectrogram::getUseFalseColour () const
{
   return this->mUseFalseColour;
}

//------------------------------------------------------------------------------
//
void QESpectrogram::setScaleWrap (const int scaleWrap)
{
   this->mScaleWrap = LIMIT (scaleWrap, 1, 10);
   this->updateDataVisulation();
}

//------------------------------------------------------------------------------
//
int QESpectrogram::getScaleWrap () const
{
   return this->mScaleWrap;
}

//------------------------------------------------------------------------------
//
void QESpectrogram::setOrientation (const Qt::Orientation orientation)
{
   this->mOrientation = orientation;
   this->updateDataVisulation();
}

//------------------------------------------------------------------------------
//
Qt::Orientation QESpectrogram::getOrientation () const
{
   return this->mOrientation;
}

// end
