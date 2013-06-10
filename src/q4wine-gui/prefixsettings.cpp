/***************************************************************************
 *   Copyright (C) 2008-2013 by Alexey S. Malakhov <brezerk@gmail.com>     *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#include "prefixsettings.h"

PrefixSettings::PrefixSettings(QString prefix_name, QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
    // Setup base UI
    setupUi(this);

    // Setting class prefix name
    this->prefix_name=prefix_name;

    // Loading libq4wine-core.so
#ifdef RELEASE
    libq4wine.setFileName(_CORELIB_PATH_);
#else
    libq4wine.setFileName("../q4wine-lib/libq4wine-core");
#endif
    if (!libq4wine.load()){
        libq4wine.load();
    }

    // Getting corelib calss pointer
    CoreLibClassPointer = (CoreLibPrototype *) libq4wine.resolve("createCoreLib");
    CoreLib.reset((corelib *)CoreLibClassPointer(true));

    this->addNew=false;
    this->loadThemeIcons();

    QHash<QString,QString> result = db_prefix.getByName(prefix_name);
    if (result.value("id").isEmpty())
        return;

    prefix_id=result.value("id");

    Version ver;
    ver.id_ = result.value("version_id");
    ver.load();
    version_name=ver.name_;

    txtMountPoint->setText(result.value("mount"));

    connect(txtMountPoint, SIGNAL(editingFinished()), this, SLOT(getWineCdromLetter()));

    if (prefix_name=="Default"){
        txtPrefixName->setEnabled(false);
        txtPrefixPath->setEnabled(false);

        cmdGetPrefixPath->setEnabled(false);

        txtWineLibs->setText("");
        txtWineLoaderBin->setText("");
        txtWineServerBin->setText("");
        txtWineBin->setText("");
    } else {
        cmdGetPrefixPath->installEventFilter(this);

        txtWineLibs->setText(result.value("libs"));
        txtWineLoaderBin->setText(result.value("loader"));
        txtWineServerBin->setText(result.value("server"));
        txtWineBin->setText(result.value("bin"));

        if (!result.value("libs").isEmpty())
            cmdClnWineLibs->setEnabled(true);

        if (!result.value("loader").isEmpty())
            cmdClnWineLoaderBin->setEnabled(true);

        if (!result.value("server").isEmpty())
            cmdClnWineServerBin->setEnabled(true);

        if (!result.value("bin").isEmpty())
            cmdClnWineBin->setEnabled(true);
    }

    if (!result.value("arch").isEmpty())
        comboArchList->setCurrentIndex(comboArchList->findText(result.value("arch")));
    if (!result.value("mountpoint_windrive").isEmpty()){
        comboWinDrive->setCurrentIndex(comboWinDrive->findText(result.value("mountpoint_windrive")));
    } else {
        this->getWineCdromLetter();
    }

    txtPrefixName->setText(prefix_name);
    txtPrefixPath->setText(result.value("path"));
    txtRunString->setText(result.value("run_string"));

    connect(cmdCancel, SIGNAL(clicked()), this, SLOT(cmdCancel_Click()));
    connect(cmdOk, SIGNAL(clicked()), this, SLOT(cmdOk_Click()));
    connect(cmdHelp, SIGNAL(clicked()), this, SLOT(cmdHelp_Click()));

    connect(cmdClnWineBin, SIGNAL(clicked()), this, SLOT(cmdClnWineBin_Click()));
    connect(cmdClnWineServerBin, SIGNAL(clicked()), this, SLOT(cmdClnWineServerBin_Click()));
    connect(cmdClnWineLoaderBin, SIGNAL(clicked()), this, SLOT(cmdClnWineLoaderBin_Click()));
    connect(cmdClnWineLibs, SIGNAL(clicked()), this, SLOT(cmdClnWineLibs_Click()));

    connect(comboVersionList, SIGNAL(currentIndexChanged(const QString)), this, SLOT(comboVersionList_Change(const QString)));
    connect(cmdAddVersion, SIGNAL(clicked()), this, SLOT(cmdAddVersion_Click()));
       // comboArchList->setEnabled(false);

    getVersionsList();

    cmdGetMountPoint->installEventFilter(this);

    cmdOk->setFocus(Qt::ActiveWindowFocusReason);

    return;
}

PrefixSettings::PrefixSettings(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
    // Setup base UI
    setupUi(this);

    this->setWindowTitle(tr("Add new Prefix"));
    this->lblCaption->setText(tr("Add new Prefix"));
    this->addNew=true;

    // Loading libq4wine-core.so
#ifdef RELEASE
    libq4wine.setFileName(_CORELIB_PATH_);
#else
    libq4wine.setFileName("../q4wine-lib/libq4wine-core");
#endif

    if (!libq4wine.load()){
        libq4wine.load();
    }

    // Getting corelib calss pointer
    CoreLibClassPointer = (CoreLibPrototype *) libq4wine.resolve("createCoreLib");
    CoreLib.reset((corelib *)CoreLibClassPointer(true));

    this->loadThemeIcons();

    cmdGetPrefixPath->installEventFilter(this);
    cmdGetMountPoint->installEventFilter(this);

    txtRunString->setText(RUN_STRING_TEMPLATE);

    connect(cmdCancel, SIGNAL(clicked()), this, SLOT(cmdCancel_Click()));
    connect(cmdOk, SIGNAL(clicked()), this, SLOT(cmdOk_Click()));
    connect(cmdHelp, SIGNAL(clicked()), this, SLOT(cmdHelp_Click()));

    connect(cmdClnWineBin, SIGNAL(clicked()), this, SLOT(cmdClnWineBin_Click()));
    connect(cmdClnWineServerBin, SIGNAL(clicked()), this, SLOT(cmdClnWineServerBin_Click()));
    connect(cmdClnWineLoaderBin, SIGNAL(clicked()), this, SLOT(cmdClnWineLoaderBin_Click()));
    connect(cmdClnWineLibs, SIGNAL(clicked()), this, SLOT(cmdClnWineLibs_Click()));

    connect(txtPrefixName, SIGNAL(textChanged(QString)), this, SLOT(setDefPath(QString)));

    connect(comboVersionList, SIGNAL(currentIndexChanged(const QString)), this, SLOT(comboVersionList_Change(const QString)));
    connect(cmdAddVersion, SIGNAL(clicked()), this, SLOT(cmdAddVersion_Click()));
       // comboArchList->setEnabled(false);

    getVersionsList();

    txtPrefixName->setFocus(Qt::ActiveWindowFocusReason);
    return;
}

QString PrefixSettings::getPrefixName(){
    return txtPrefixName->text();
}

void PrefixSettings::loadThemeIcons(){
    lblLogo->setPixmap(CoreLib->loadPixmap("data/exec.png"));

    cmdClnWineBin->setIcon(CoreLib->loadIcon("data/kill.png"));
    cmdClnWineServerBin->setIcon(CoreLib->loadIcon("data/kill.png"));
    cmdClnWineLoaderBin->setIcon(CoreLib->loadIcon("data/kill.png"));
    cmdClnWineLibs->setIcon(CoreLib->loadIcon("data/kill.png"));
    cmdGetMountPoint->setIcon(CoreLib->loadIcon("data/folder.png"));
    cmdGetPrefixPath->setIcon(CoreLib->loadIcon("data/folder.png"));
    cmdAddVersion->setIcon(CoreLib->loadIcon("data/add.png"));

    return;
}

void PrefixSettings::cmdCancel_Click(){
    reject();
    return;
}

void PrefixSettings::cmdOk_Click(){
    if (txtPrefixName->text().isEmpty()){
        QMessageBox::warning(this, tr("Error"), tr("Enter a prefix name"));
        return;
    }

    if (prefix_name!=txtPrefixName->text()){
        if (db_prefix.isExistsByName(txtPrefixName->text())){
            QMessageBox::warning(this, tr("Error"), tr("Sorry, but prefix named %1 already exists.").arg(txtPrefixName->text()));
            return;
        }
    }

    QString path = txtPrefixPath->text();
    if (path.right(1)=="/"){
        path=path.left(path.length()-1);
        txtPrefixPath->setText(path);
    }

    Version ver;
    ver.name_ = comboVersionList->currentText();
    if (!ver.load())
        return;

    if (this->addNew){

        QDir directory(path);
        if (directory.exists()){
            if(QMessageBox::warning(this, tr("Warning"), tr("Directory \"%1\" already exists. Do you wish to use it anyway?").arg(path), QMessageBox::Yes, QMessageBox::No)==QMessageBox::No)
                return;
        } else {
            // In case of non default arch, wine will fail to run wine programs from existent
            // empty prefix directory.
            // So we will try to create parent folders, but not prefix dir itself.
            if (!directory.mkpath("./../")) {
                QMessageBox::critical(this, tr("Error"), tr("The directory \"%1\" could not be created.").arg(path), QMessageBox::Ok);
                return;
            }
        }

        if (!db_prefix.addPrefix(txtPrefixName->text(),  txtPrefixPath->text(), txtWineBin->text(), txtWineServerBin->text(), txtWineLoaderBin->text(), txtWineLibs->text(), txtMountPoint->text(), comboArchList->currentText(), this->comboWinDrive->currentText(), this->txtRunString->text(), ver.id_))
            reject();

        CoreLib->createPrefixDBStructure(txtPrefixName->text());
#ifndef _OS_DARWIN_
        if (CoreLib->getSetting("Plugins", "enableMenuDesktop", false, true).toBool())
            sys_menu.generateSystemMenu(txtPrefixName->text());
#endif
    } else {
        if (!db_prefix.updatePrefix(txtPrefixName->text(), txtPrefixPath->text(), txtWineBin->text(), txtWineServerBin->text(), txtWineLoaderBin->text(), txtWineLibs->text(), txtMountPoint->text(), this->prefix_name, comboArchList->currentText(), this->comboWinDrive->currentText(), this->txtRunString->text(), ver.id_))
            reject();
#ifndef _OS_DARWIN_
        if (CoreLib->getSetting("Plugins", "enableMenuDesktop", false, true).toBool()){
            if (this->prefix_name != txtPrefixName->text()){
                sys_menu.remove_dir_info(this->prefix_name);
                sys_menu.generateSystemMenu(txtPrefixName->text());
            }
        }
#endif
    }

    accept();
    return;
}

bool PrefixSettings::eventFilter(QObject *obj, QEvent *event){
    /*
     * Select folder dialog
     */

    if (event->type() == QEvent::MouseButtonRelease) {

        QString file="";

        if (obj->objectName().right(3)=="Bin"){
#if QT_VERSION >= 0x040500
            QFileDialog::Options options;

            if (CoreLib->getSetting("advanced", "useNativeFileDialog", false, 1)==0)
                options = QFileDialog::DontUseNativeDialog;

            file = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), "All files (*)", 0, options);
#else
            file = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), "All files (*)");
#endif
        } else {
#if QT_VERSION >= 0x040500
            QFileDialog::Options options;

            if (CoreLib->getSetting("advanced", "useNativeFileDialog", false, 1)==0)
                options = QFileDialog::DontUseNativeDialog;

            file = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), options);
#else
            file = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::DontResolveSymlinks);
#endif
        }

        if (!file.isEmpty()){
            if (file.right(1)=="/"){
                file=file.left(file.length()-1);
            }

            QString a="";

            a.append("txt");
            a.append(obj->objectName().right(obj->objectName().length()-6));

            std::auto_ptr<QLineEdit> lineEdit (findChild<QLineEdit *>(a));

            if (lineEdit.get()){
                lineEdit->setText(file);
            } else {
                qDebug("Error");
            }

            lineEdit.release();

            if (obj->objectName()=="cmdGetPrefixPath"){
                txtPrefixName->setText(txtPrefixPath->text().split("/").last());
            }

            if (obj->objectName()=="cmdGetMountPoint"){
                this->getWineCdromLetter();
            }

        }
    }

    return false;
}

void PrefixSettings::cmdHelp_Click(){
    QString rawurl;
    switch (twbGeneral->currentIndex()){
    case 0:
        rawurl = "10-prefix-settings.html#general";
    break;
    case 1:
        rawurl = "10-prefix-settings.html#winepath";
    break;
    case 2:
        rawurl = "10-prefix-settings.html#advanced";
    break;
    }

    CoreLib->openHelpUrl(rawurl);
}

void PrefixSettings::getWineCdromLetter(){
    if (!txtMountPoint->text().isEmpty()){
        QChar letter = CoreLib->getCdromWineDrive(db_prefix.getPath(prefix_name), txtMountPoint->text());
        if (!letter.isNull()){
            comboWinDrive->setCurrentIndex(comboWinDrive->findText(QString("%1:").arg(letter.toUpper())));
        }
    }
}

void PrefixSettings::setDefPath(QString prefix_name){
    QString path = CoreLib->getSetting("advanced", "prefixDefaultPath").toString();
    if (this->txtPrefixPath->text().isEmpty() or this->txtPrefixPath->text().startsWith(path)){
        if (!path.endsWith("/"))
            path.append("/");
        path.append(prefix_name);

        this->txtPrefixPath->setText(path);
    }
}

void PrefixSettings::cmdClnWineBin_Click(){
    txtWineBin->clear();
}

void PrefixSettings::cmdClnWineServerBin_Click(){
    txtWineServerBin->clear();
}

void PrefixSettings::cmdClnWineLoaderBin_Click(){
    txtWineLoaderBin->clear();
}

void PrefixSettings::cmdClnWineLibs_Click(){
    txtWineLibs->clear();
}

void PrefixSettings::cmdAddVersion_Click(){
    VersionManager* vers = new VersionManager();
    if (!comboVersionList->currentText().isEmpty())
        vers->setVersionFocus(comboVersionList->currentText());
    connect(vers, SIGNAL(setVersion(QString)), this, SLOT(setVersion(QString)));
    vers->exec();
    disconnect(this, SLOT(setVersion(QString)));
    delete(vers);

}

void PrefixSettings::getVersionsList(){
    comboVersionList->clear();
    Version ver;
    QList<Version> list = ver.load_all();

    for (int i = 0; i < list.size(); ++i)
        comboVersionList->addItem(list.at(i).name_);

    comboVersionList->setCurrentIndex(comboVersionList->findText(version_name));
}

void PrefixSettings::comboVersionList_Change(const QString & text){
    //version_name = text;
}

void PrefixSettings::setVersion(QString version_name){
    this->version_name = version_name;
    getVersionsList();
}
