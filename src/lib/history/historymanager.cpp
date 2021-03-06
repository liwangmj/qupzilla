/* ============================================================
* QupZilla - WebKit based browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "historymanager.h"
#include "ui_historymanager.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "history.h"
#include "browsinglibrary.h"
#include "tabwidget.h"
#include "tabbedwebview.h"
#include "historymodel.h"
#include "headerview.h"
#include "qzsettings.h"

#include <QMessageBox>

HistoryManager::HistoryManager(BrowserWindow* window, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HistoryManager)
    , m_window(window)
{
    ui->setupUi(this);

    connect(ui->historyTree, SIGNAL(openLink(QUrl,HistoryView::OpenBehavior)), this, SLOT(openLink(QUrl,HistoryView::OpenBehavior)));

    connect(ui->deleteB, SIGNAL(clicked()), ui->historyTree, SLOT(removeItems()));
    connect(ui->clearAll, SIGNAL(clicked()), this, SLOT(clearHistory()));

    ui->historyTree->setFocus();
}

BrowserWindow* HistoryManager::getQupZilla()
{
    if (!m_window) {
        m_window = mApp->getWindow();
    }
    return m_window.data();
}

void HistoryManager::setMainWindow(BrowserWindow* window)
{
    if (window) {
        m_window = window;
    }
}

void HistoryManager::restoreState(const QByteArray &state)
{
    ui->historyTree->header()->restoreState(state);
}

QByteArray HistoryManager::saveState()
{
    return ui->historyTree->header()->saveState();
}

void HistoryManager::clearHistory()
{
    QMessageBox::StandardButton button = QMessageBox::warning(this, tr("Confirmation"),
                                         tr("Are you sure to delete all history?"), QMessageBox::Yes | QMessageBox::No);
    if (button != QMessageBox::Yes) {
        return;
    }

    mApp->history()->clearHistory();
    mApp->history()->optimizeHistory();
}

void HistoryManager::search(const QString &searchText)
{
    ui->historyTree->filterModel()->setFilterFixedString(searchText);
}

void HistoryManager::openLink(const QUrl &url, HistoryView::OpenBehavior openIn)
{
    if (openIn == HistoryView::OpenInNewTab) {
        getQupZilla()->tabWidget()->addView(url, qzSettings->newTabPosition);
    }
    else {
        getQupZilla()->weView()->load(url);
    }
}

HistoryManager::~HistoryManager()
{
    delete ui;
}
