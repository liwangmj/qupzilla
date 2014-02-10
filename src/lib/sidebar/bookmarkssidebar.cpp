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
#include "bookmarkssidebar.h"
#include "ui_bookmarkssidebar.h"
#include "bookmarkstools.h"
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "mainapplication.h"

#include <QMenu>

BookmarksSidebar::BookmarksSidebar(QupZilla* mainClass, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BookmarksSideBar)
    , m_window(mainClass)
    , m_bookmarks(mApp->bookmarks())
{
    ui->setupUi(this);
    ui->tree->setViewType(BookmarksTreeView::BookmarksSidebarViewType);

    connect(ui->tree, SIGNAL(bookmarkActivated(BookmarkItem*)), this, SLOT(bookmarkActivated(BookmarkItem*)));
    connect(ui->tree, SIGNAL(bookmarkCtrlActivated(BookmarkItem*)), this, SLOT(bookmarkCtrlActivated(BookmarkItem*)));
    connect(ui->tree, SIGNAL(bookmarkShiftActivated(BookmarkItem*)), this, SLOT(bookmarkShiftActivated(BookmarkItem*)));
    connect(ui->tree, SIGNAL(contextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)));

    connect(ui->search, SIGNAL(textChanged(QString)), ui->tree, SLOT(search(QString)));
}

BookmarksSidebar::~BookmarksSidebar()
{
    delete ui;
}

void BookmarksSidebar::bookmarkActivated(BookmarkItem* item)
{
    openBookmark(item);
}

void BookmarksSidebar::bookmarkCtrlActivated(BookmarkItem* item)
{
    openBookmarkInNewTab(item);
}

void BookmarksSidebar::bookmarkShiftActivated(BookmarkItem* item)
{
    openBookmarkInNewWindow(item);
}

void BookmarksSidebar::openBookmark(BookmarkItem* item)
{
    item = item ? item : ui->tree->selectedBookmark();
    BookmarksTools::openBookmark(m_window, item);
}

void BookmarksSidebar::openBookmarkInNewTab(BookmarkItem* item)
{
    item = item ? item : ui->tree->selectedBookmark();
    BookmarksTools::openBookmarkInNewTab(m_window, item);
}

void BookmarksSidebar::openBookmarkInNewWindow(BookmarkItem* item)
{
    item = item ? item : ui->tree->selectedBookmark();
    BookmarksTools::openBookmarkInNewWindow(item);
}

void BookmarksSidebar::deleteBookmarks()
{
    QList<BookmarkItem*> items = ui->tree->selectedBookmarks();

    foreach (BookmarkItem* item, items) {
        if (m_bookmarks->canBeModified(item)) {
            m_bookmarks->removeBookmark(item);
        }
    }
}

void BookmarksSidebar::createContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction* actNewTab = menu.addAction(QIcon::fromTheme("tab-new", QIcon(":/icons/menu/tab-new.png")), tr("Open in new tab"));
    QAction* actNewWindow = menu.addAction(QIcon::fromTheme("window-new"), tr("Open in new window"));
    menu.addSeparator();
    QAction* actDelete = menu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));

    connect(actNewTab, SIGNAL(triggered()), this, SLOT(openBookmarkInNewTab()));
    connect(actNewWindow, SIGNAL(triggered()), this, SLOT(openBookmarkInNewWindow()));
    connect(actDelete, SIGNAL(triggered()), this, SLOT(deleteBookmarks()));

    bool canBeDeleted = false;
    QList<BookmarkItem*> items = ui->tree->selectedBookmarks();

    foreach (BookmarkItem* item, items) {
        if (m_bookmarks->canBeModified(item)) {
            canBeDeleted = true;
            break;
        }
    }

    if (!canBeDeleted) {
        actDelete->setDisabled(true);
    }

    if (!ui->tree->selectedBookmark() || !ui->tree->selectedBookmark()->isUrl()) {
        actNewTab->setDisabled(true);
        actNewWindow->setDisabled(true);
    }

    menu.exec(pos);
}
