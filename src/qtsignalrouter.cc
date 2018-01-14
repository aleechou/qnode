
#define ConnectSignalAndSlot \    
	    \
	switch (wrapper->typeId) {    \
	    \
	    case 39:    \
	        switch(sigindex){    \
	    \
	        case 0:    \
	            QObject::connect((QObject*)wrapper->object, &QObject::destroyed, [](QObject*){    \
	            qDebug() << "destroyed(QObject*) <<<<<<<"  ;    \
	        }) ;    \
	            break;    \
	    \
	        case 1:    \
	            QObject::connect((QObject*)wrapper->object, &QObject::destroyed, [](){    \
	            qDebug() << "destroyed() <<<<<<<"  ;    \
	        }) ;    \
	            break;    \
	    \
	        case 2:    \
	            QObject::connect((QObject*)wrapper->object, &QObject::objectNameChanged, [](const QString & arg1){    \
	            qDebug() << "objectNameChanged(QString) <<<<<<<"  << arg1 ;    \
	        }) ;    \
	            break;    \
	    \
	    default:    \
	        Throw("unknow sigindex")    \
	        return ;    \
	    }    \
	    break;    \
	    \
	    case 1029:    \
	        switch(sigindex){    \
	    \
	        case 0:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::destroyed, [](QObject*){    \
	            qDebug() << "destroyed(QObject*) <<<<<<<"  ;    \
	        }) ;    \
	            break;    \
	    \
	        case 1:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::destroyed, [](){    \
	            qDebug() << "destroyed() <<<<<<<"  ;    \
	        }) ;    \
	            break;    \
	    \
	        case 2:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::objectNameChanged, [](const QString & arg1){    \
	            qDebug() << "objectNameChanged(QString) <<<<<<<"  << arg1 ;    \
	        }) ;    \
	            break;    \
	    \
	        case 5:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::windowTitleChanged, [](const QString & arg1){    \
	            qDebug() << "windowTitleChanged(QString) <<<<<<<"  << arg1 ;    \
	        }) ;    \
	            break;    \
	    \
	        case 6:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::windowIconChanged, [](const QIcon & arg1){    \
	            qDebug() << "windowIconChanged(QIcon) <<<<<<<"  ;    \
	        }) ;    \
	            break;    \
	    \
	        case 7:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::windowIconTextChanged, [](const QString & arg1){    \
	            qDebug() << "windowIconTextChanged(QString) <<<<<<<"  << arg1 ;    \
	        }) ;    \
	            break;    \
	    \
	        case 8:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::customContextMenuRequested, [](const QPoint & arg1){    \
	            qDebug() << "customContextMenuRequested(QPoint) <<<<<<<"  ;    \
	        }) ;    \
	            break;    \
	    \
	        case 32:    \
	            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::ready, [](const QString & arg1, const bool & arg2){    \
	            qDebug() << "ready(QString,bool) <<<<<<<"  << arg1 << arg2 ;    \
	        }) ;    \
	            break;    \
	    \
	    default:    \
	        Throw("unknow sigindex")    \
	        return ;    \
	    }    \
	    break;    \
	    \
	default:    \
	    Throw("unknow typeId")    \
	    return;    \
	}    \
	    \
