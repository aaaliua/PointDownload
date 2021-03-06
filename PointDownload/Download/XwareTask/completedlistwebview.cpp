/***********************************************************************
*PointDownload
*Copyright (C) 2014  PointTeam
*
* Author:     Choldrim <choldrim@foxmail.com>
* Maintainer: Choldrim <choldrim@foxmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "completedlistwebview.h"

#define CHECK_COMPLETED_TASK_INTERVAL 2000
#define AUTO_CLEAR_INTERVAL 5 // clear time = CHECK_COMPLETED_TASK_INTERVAL * AUTO_CLEAR_INTERVAL


CompletedListWebView::CompletedListWebView(QObject *parent)
{
    Q_UNUSED(parent);
}

void CompletedListWebView::initConnection()
{
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(loadFinishHandle()));

    connect(autoClearTimer, SIGNAL(timeout()), this, SLOT(clearAllCompletedTask()));
}

void CompletedListWebView::init()
{
    this->page()->setNetworkAccessManager(
                XwareWebController::getInstance()->getWebView()->page()->networkAccessManager());

    // direct to the downloading URL
    this->setUrl(XwareWebController::getInstance()->getWebView()->url());

    autoClearTimer = new QTimer();
    autoClearTimer->setInterval(CHECK_COMPLETED_TASK_INTERVAL);
    autoClearTimer->start();

    initConnection();

    firstCheckNewCompletedTask = true;
    checkCompletedTaskIntervalCounter = 1;
}

void CompletedListWebView::evaluateDefaultJS()
{
    // default js
}

void CompletedListWebView::evaluateJS(QString js)
{
    if(XWARE_CONSTANTS_STRUCT.DEBUG)
        qDebug()<<"completed webview js : => "<<js;
    this->page()->mainFrame()->evaluateJavaScript(js);
}

CompletedListWebView * CompletedListWebView::completedListWebView = NULL;
CompletedListWebView * CompletedListWebView::getInstance()
{
    if(completedListWebView == NULL)
    {
        completedListWebView = new CompletedListWebView();
    }
    return completedListWebView;
}

void CompletedListWebView::clearAllCompletedTask(bool deleteFile)
{
    QMap<QString, QString> map = getAllCompletedTaskFromJson();

    // check whether there is new completed tasks
    checkNewCompletedTask(map);

    QStringList list = map.keys();

    if(list.length() == 0)return;

    if(checkCompletedTaskIntervalCounter < AUTO_CLEAR_INTERVAL)
    {
        ++checkCompletedTaskIntervalCounter;
        return;
    }
    checkCompletedTaskIntervalCounter = 1;

    if(deleteFile)
    {
        QString js;
        foreach(QString id, list)
        {
            js += QString("if(App.get(\"task.type\") == 1){");
            js += QString("App.set(\"task.remove\", "+ id +");");
            js += QString("App.set(\"dialogs.removeTasks.checked\", 1);");
            js += QString("App.set(\"dialogs.removeTasks.confirmEVT\", 1);");
            js += QString("}");
        }
        evaluateJS(js);
    }
    else
    {
        QString js;
        foreach(QString id, list)
        {
            js += QString("if(App.get(\"task.type\") == 1){");
            js += QString("App.set(\"task.remove\", "+ id +");");
            js += QString("App.set(\"dialogs.removeTasks.confirmEVT\", 1);");
            js += QString("}");
        }
        evaluateJS(js);
    }

    this->reload();
}

void CompletedListWebView::changeToCompletedList()
{
     evaluateJS("App.set(\"task.type\", 1);");
}

void CompletedListWebView::loadFinishHandle()
{
   QTimer::singleShot(1000, this, SLOT(changeToCompletedList()));
}

void CompletedListWebView::removeComletedTask(QString taskId, bool deleteFile)
{
    Q_UNUSED(taskId);
    if(deleteFile)
    {

    }
    else
    {

    }
}


QMap<QString, QString> CompletedListWebView::getAllCompletedTaskFromJson()
{
    QUrl url(XWARE_CONSTANTS_STRUCT.URLSTR + "list?v=2&type=1&pos=0&number=99999&needUrl=1");

    QEventLoop loop;

    // request
    reply = manager.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec(); // wait for message
    reply->disconnect();

    // 读到的信息
    QString jsonStr = QUrl::fromPercentEncoding(reply->readAll());
    QJsonDocument jsd = QJsonDocument::fromJson(jsonStr.toUtf8());
    QMap<QString, QVariant> jsonMap = jsd.toVariant().toMap();

    QList<QVariant> completedTaskList = jsonMap.value("tasks").toList();

    QMap<QString, QString> map;
    for(int i = 0; i < completedTaskList.length(); ++i )
    {
        QString id = completedTaskList.at(i).toMap().value("id").toString();
        QString url = completedTaskList.at(i).toMap().value("url").toString();
        map.insert(id, url);
    }

    return map;
}

void CompletedListWebView::checkNewCompletedTask(QMap<QString, QString> map)
{
    if(firstCheckNewCompletedTask)
    {
        firstCheckNewCompletedTask = false;
        completedTaskMap = map;
        return;
    }

    QStringList completedTasksIdList = map.keys();
    QStringList completedTasksIdList_old = completedTaskMap.keys();

    QString id;
    QString id_old;
    bool isMatch;
    foreach(id, completedTasksIdList)
    {
        isMatch = false;

        foreach(id_old, completedTasksIdList_old)
        {
            if(id == id_old)
            {
                isMatch = true;
                break;
            }
        }

        if(!isMatch)
        {
            emit sNewCompletedTask(map.value(id));

            // 如果有新任务则重置计数器
            checkCompletedTaskIntervalCounter = 1;
        }
    }

    completedTaskMap = map;
}

