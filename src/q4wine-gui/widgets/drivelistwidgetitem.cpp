/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011 by Malakhov Alexey                            *
 *   brezerk@gmail.com                                                     *
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

#include "drivelistwidgetitem.h"

DriveListWidgetItem::DriveListWidgetItem(QListWidget *parent, int type): QObject( parent), QListWidgetItem(parent, type)
{
	// Loading libq4wine-core.so
#ifdef RELEASE
    libq4wine.setFileName("libq4wine-core");
#else
    libq4wine.setFileName("../q4wine-lib/libq4wine-core");
#endif

	if (!libq4wine.load()){
		  libq4wine.load();
	}

	// Getting corelib calss pointer
	CoreLibClassPointer = (CoreLibPrototype *) libq4wine.resolve("createCoreLib");
	CoreLib.reset((corelib *)CoreLibClassPointer(true));
	return;
}

void DriveListWidgetItem::setDrive(QString letter, QString path, QString type){
	this->letter=letter;
	this->path=path;
	if (type.isEmpty()){
		this->type="auto";
	} else {
		this->type=type;
	}
	this->setText(QString("%1 %2\nType: %3").arg(letter).arg(path).arg(this->type));
	this->setIcon(CoreLib->loadIcon(this->getDrivePic(type)));
	return;
}

QString DriveListWidgetItem::getLetter(){
	return this->letter;
}

QString DriveListWidgetItem::getPath(){
	return this->path;
}

QString DriveListWidgetItem::getType(){
	return this->type;
}

QString DriveListWidgetItem::getDrivePic(QString driveType){
	QString pic;
	if (driveType.isEmpty()){
		pic = "data/drive_menu.png";
	} else {
		if (driveType=="hd"){
			pic = "data/drive_menu.png";
		} else if (driveType=="network"){
			pic = "data/drive_menu.png";
		} else if (driveType=="floppy"){
			pic = "data/drive_menu.png";
		} else if (driveType=="cdrom"){
			pic = "data/cdrom_menu.png";
		} else {
			pic = "data/drive_menu.png";
		}
	}
	return pic;
}
