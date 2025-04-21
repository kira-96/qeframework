/*  QEFileBrowser.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  Copyright (c) 2012-2024 Australian Synchrotron
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
 *    Ricardo Fernandes
 *  Contact details:
 *    ricardo.fernandes@synchrotron.org.au
 */

#ifndef QE_FILE_BROWSER_H
#define QE_FILE_BROWSER_H

#include <QTableWidget>
#include <QPushButton>
#include <QEEnums.h>
#include <QEWidget.h>
#include <QELineEdit.h>

// =============================================================================
//  _QTABLEWIDGETFILEBROWSER CLASS
// =============================================================================
//
class _QTableWidgetFileBrowser:public QTableWidget
{
   Q_OBJECT

private:
   bool initialized;

protected:

public:
   explicit _QTableWidgetFileBrowser(QWidget * pParent = 0);
   void refreshSize();
   void resizeEvent(QResizeEvent *);
   void resize(int w, int h);
};


// =============================================================================
//  QEFILEBROWSER METHODS
// =============================================================================
/*!
  The QEFileBrowser widget allows the user to browse existing files from a certain directory.
*/
class QE_FRAMEWORK_LIBRARY_SHARED_EXPORT QEFileBrowser:public QWidget, public QEWidget
{
   Q_OBJECT
protected:
   QELineEdit *qeLineEditDirectoryPath;
   QPushButton *qPushButtonDirectoryBrowser;
   QPushButton *qPushButtonRefresh;
   _QTableWidgetFileBrowser *qTableWidgetFileBrowser;
   QString fileFilter;
   bool showFileExtension;
   bool fileDialogDirectoriesOnly;
   QE::LayoutOptions optionsLayout;

public:
   explicit QEFileBrowser(QWidget *pParent = 0);
   virtual ~QEFileBrowser(){}

   void setVariableName(QString pValue);
   QString getVariableName();

   void setVariableNameSubstitutions(QString pValue);
   QString getVariableNameSubstitutions();

   void setDirectoryPath(QString pValue);
   QString getDirectoryPath();

   void setShowDirectoryPath(bool pValue);
   bool getShowDirectoryPath();

   void setShowDirectoryBrowser(bool pValue);
   bool getShowDirectoryBrowser();

   void setShowRefresh(bool pValue);
   bool getShowRefresh();

   void setShowTable(bool pValue);
   bool getShowTable();

   void setShowColumnTime(bool pValue);
   bool getShowColumnTime();

   void setShowColumnSize(bool pValue);
   bool getShowColumnSize();

   void setShowColumnFilename(bool pValue);
   bool getShowColumnFilename();

   void setShowFileExtension(bool pValue);
   bool getShowFileExtension();

   void setFileFilter(QString pValue);
   QString getFileFilter();

   void setFileDialogDirectoriesOnly(bool pValue);
   bool getFileDialogDirectoriesOnly();

   void setOptionsLayout(QE::LayoutOptions pValue);
   QE::LayoutOptions getOptionsLayout();

   void updateTable();

   // Note: variable is not a PV variable, rather a directory name variable.
   //
   /// EPICS variable name (CA PV).
   /// This variable is used for both writing and reading the directory to be used by the widget.
   Q_PROPERTY(QString variable READ getVariableName WRITE setVariableName)

   /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2... Values may be quoted strings. For example, 'PUMP=PMP3, NAME = "My Pump"'
   /// These substitutions are applied to variable names for all QE widgets. In some widgets are are also used for other purposes.
   Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutions WRITE setVariableNameSubstitutions)

   /// Default directory where to browse files when QEFileBrowser is launched for the first time
   Q_PROPERTY(QString directoryPath READ getDirectoryPath WRITE setDirectoryPath)

   /// Show/hide directory path line edit where the user can specify the directory to browse files
   Q_PROPERTY(bool showDirectoryPath READ getShowDirectoryPath WRITE setShowDirectoryPath)

   /// Show/hide button to open the dialog window to browse for directories and files
   Q_PROPERTY(bool showDirectoryBrowser READ getShowDirectoryBrowser WRITE setShowDirectoryBrowser)

   /// Show/hide button to refresh the table containing the list of files being browsed
   Q_PROPERTY(bool showRefresh READ getShowRefresh WRITE setShowRefresh)

   /// Show/hide table containing the list of files being browsed
   Q_PROPERTY(bool showTable READ getShowTable WRITE setShowTable)

   /// Show/hide column containing the time of creation of files
   Q_PROPERTY(bool showColumnTime READ getShowColumnTime WRITE setShowColumnTime)

   /// Show/hide column containing the size (in bytes) of files
   Q_PROPERTY(bool showColumnSize READ getShowColumnSize WRITE setShowColumnSize)

   /// Show/hide column containing the name of files
   Q_PROPERTY(bool showColumnFilename READ getShowColumnFilename WRITE setShowColumnFilename)

   /// Show/hide the extension of files
   Q_PROPERTY(bool showFileExtension READ getShowFileExtension WRITE setShowFileExtension)

   /// Enable/disable the browsing of directories-only when opening the dialog window
   Q_PROPERTY(bool fileDialogDirectoriesOnly READ getFileDialogDirectoriesOnly WRITE setFileDialogDirectoriesOnly)

   /// Specify which files to browse. To specify more than one filter, please separate them with a ";".
   /// Example: *.py;*.ui (this will only display files with an extension .py or .ui).
   Q_PROPERTY(QString fileFilter READ getFileFilter WRITE setFileFilter)

   /// Change the order of the widgets. Valid orders are: TOP, BOTTOM, LEFT and RIG
   Q_PROPERTY(QE::LayoutOptions optionsLayout READ getOptionsLayout WRITE setOptionsLayout)

   // BEGIN-STANDARD-V2-PROPERTIES ===================================================
   // Standard properties
   // These properties should be identical for every widget using them.
   // WHEN MAKING CHANGES: Use the update_widget_properties script in the
   // resources directory.
public slots:
   /// Slot to set the visibility of a QE widget, taking into account the user level.
   /// Widget will be hidden if hidden by a call this slot, by will only be made visible by a calll to this slot if the user level allows.
   void setManagedVisible( bool v ){ setRunVisible( v ); }
public:
   /// variableAsToolTip not applicale to V2 options.
   /// Class should ensure this option is initialised to false.

   /// allowDrop not applicale to V2 options.
   /// Class should ensure this option is initialised to false.

   /// Display the widget. Default is true.
   /// Setting this property false is usefull if widget is only used to provide a signal - for example, when supplying data to a QELink widget.
   /// Note, when false the widget will still be visible in Qt Designer.
   Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)

   /// Set the ID used by the message filtering system. Default is zero.
   /// Widgets or applications that use messages from the framework have the option of filtering on this ID.
   /// For example, by using a unique message source ID a QELog widget may be set up to only log messages from a select set of widgets.
   Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

   /// Hide style sheet from designer as style calculation by the styleManager and not directly setable per se.
   /// This also stops transient styles being saved to the ui file.
   Q_PROPERTY(QString styleSheet   READ styleSheet       WRITE setStyleSheet  DESIGNABLE false)

   /// Style Sheet string to be applied before, i.e. lower priority than, any other style, e.g. alarm style and/or user level style.
   /// Default is an empty string.
   Q_PROPERTY(QString defaultStyle READ getStyleDefault  WRITE setStyleDefault)

   /// Style Sheet string to be applied when the widget is displayed in 'User' mode. Default is an empty string.
   /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
   /// This Style Sheet string will be applied by the styleManager class.
   /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
   /// and any Style Sheet strings generated during the display of data.
   Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)

   /// Style Sheet string to be applied when the widget is displayed in 'Scientist' mode. Default is an empty string.
   /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
   /// This Style Sheet string will be applied by the styleManager class.
   /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
   /// and any Style Sheet strings generated during the display of data.
   Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)

   /// Style Sheet string to be applied when the widget is displayed in 'Engineer' mode. Default is an empty string.
   /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
   /// This Style Sheet string will be applied by the styleManager class.
   /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
   /// and any Style Sheet strings generated during the display of data.
   Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)

   /// Lowest user level at which the widget is visible. Default is 'User'.
   /// Used when designing GUIs that display more and more detail according to the user mode.
   /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
   /// Widgets that are always visible should be visible at 'User'.
   /// Widgets that are only used by scientists managing the facility should be visible at 'Scientist'.
   /// Widgets that are only used by engineers maintaining the facility should be visible at 'Engineer'.
   ///
   Q_PROPERTY(QE::UserLevels userLevelVisibility READ getUserLevelVisibility WRITE setUserLevelVisibility)

   /// Lowest user level at which the widget is enabled. Default is 'User'.
   /// Used when designing GUIs that allow access to more and more detail according to the user mode.
   /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
   /// Widgets that are always accessable should be visible at 'User'.
   /// Widgets that are only accessable to scientists managing the facility should be visible at 'Scientist'.
   /// Widgets that are only accessable to engineers maintaining the facility should be visible at 'Engineer'.
   ///
   Q_PROPERTY(QE::UserLevels userLevelEnabled READ getUserLevelEnabled WRITE setUserLevelEnabled)

   /// displayAlarmStateOption not applicale to V2 options.
   /// Class should ensure this option is initialised to DISPLAY_ALARM_STATE_NEVER.

public:
   // END-STANDARD-V2-PROPERTIES =====================================================

private slots:
   void lineEditDirectoryPathChanged(QString);

   void buttonDirectoryBrowserClicked();

   void buttonRefreshClicked();

   void itemActivated(QTableWidgetItem *);

signals:
   /// Signal that is generated every time the user double-clicks a certain file.
   /// This signals emits a string that contains the full path and the name of the selected file.
   /// This signal may be captured by other widgets that perform further operations (for instance,
   /// the QEImage displays the content of this file if it is a graphical one).
   void selected(QString pFilename);

};

#endif // QE_FILE_BROWSER_H
